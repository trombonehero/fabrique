/** @file Parser.cc    Definition of @ref Parser. */
/*
 * Copyright (c) 2013 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "Lexer.h"
#include "Parser.h"

using std::auto_ptr;
using std::string;


Parser::Parser(const Lexer& lex)
	: lex(lex), savedLoc(SourceRange::None())
{
	scopes.push(new Scope());
}

Parser::~Parser()
{
	for (auto *err : errs)
		delete err;

	for (auto i : simpleTypes)
		delete i.second;
}


void Parser::SetRoot(PtrVec<Value> *values)
{
	auto_ptr<PtrVec<Value> > v(values);
	for (auto *v : *values)
		root.Register(v);
}


const Type* Parser::getType(const string& name, const PtrVec<Type>& params)
{
	auto qualifiedName(std::make_pair(name, params));

	auto i = simpleTypes.find(qualifiedName);
	if (i != simpleTypes.end())
		return i->second;

	return simpleTypes[qualifiedName] = Type::Create(name, params);
}

const Type* Parser::TakeType(Identifier *name, const PtrVec<Type>* params)
{
	auto_ptr<Identifier> i(name);
	auto_ptr<const PtrVec<Type> > p(params);
	static PtrVec<Type> empty;

	assert(name != NULL);

	return getType(name->name(), params ? *params : empty);
}


Scope& Parser::CurrentScope()
{
	// We must always have at least a top-level scope on the stack.
	assert(scopes.size() > 0);
	return *scopes.top();
}

void Parser::EnterScope() { scopes.push(new Scope(&CurrentScope())); }
void Parser::ExitScope() { scopes.pop(); }


SourceRange* Parser::CurrentTokenRange() const
{
	return new SourceRange(lex.CurrentTokenRange());
}

void Parser::SaveLoc() { savedLoc = lex.CurrentTokenRange(); }

Value* Parser::Define(Identifier *id, Expression *e)
{
	if (e == NULL)
		return NULL;

	SourceRange range(id->getSource().begin, e->getSource().end);
	auto& scope(CurrentScope());

	if (scope.Find(id) != NULL)
	{
		ReportError("redefining value", range);
		return NULL;
	}

	if (id->isTyped() and !e->getType().isSupertype(*id->getType()))
	{
		ReportError("type mismatch", range);
		return NULL;
	}

	Value *v = new Value(id, e);
	scope.Register(v);
	return v;
}


SymbolReference* Parser::Reference(Identifier *id)
{
	const Expression *e = CurrentScope().Find(id);
	if (e == NULL)
	{
		ReportError("reference to undefined value", *id);
		return NULL;
	}

	return new SymbolReference(id, e, id->getSource());
}


Action* Parser::DefineAction(PtrVec<Argument>* args, SourceRange *start)
{
	auto_ptr<PtrVec<Argument> > a(args);
	auto_ptr<SourceRange> s(start);
	SourceRange current(lex.CurrentTokenRange());

	SourceRange loc(start->begin, current.end);

	const Type *fileType = getType("file");
	const Type *fileListType = getType("list", PtrVec<Type>(1, fileType));

	return new Action(*args, *fileListType, loc);
}


Function* Parser::DefineFunction(PtrVec<Parameter> *params, const Type *ty,
                                 PtrVec<Value> *values, Expression *result)
{
	auto_ptr<PtrVec<Parameter> > p(params);
	auto_ptr<PtrVec<Value> > v(values);

	assert(params != NULL);
	assert(ty != NULL);
	assert(values != NULL);
	assert(result != NULL);

	if (!result->getType().isSupertype(*ty))
	{
		ReportError(
			"wrong return type ("
			+ result->getType().str() + " != " + ty->str()
			+ ")", *result);
		return NULL;
	}

	SourceRange loc(savedLoc.begin, lex.CurrentTokenRange().end);
	ExitScope();

	return new Function(*params, *ty, *values, result, loc);
}


Call* Parser::CreateCall(Identifier *name, PtrVec<Argument> *args)
{
	auto_ptr<Identifier> n(name);
	auto_ptr<PtrVec<Argument> > a(args);

	assert(name != NULL);
	assert(args != NULL);

	SourceRange loc(name->getSource().begin, lex.CurrentTokenRange().end);

	auto *fn(Reference(n.release()));
	if (fn == NULL)
		return NULL;

	return new Call(fn, *args, loc);
}


Conditional* Parser::IfElse(SourceRange *ifLoc, Expression *condition,
	                    Expression *thenResult, Expression *elseResult)
{
	auto_ptr<SourceRange> src(ifLoc);

	const Type &tt(thenResult->getType()), &et(elseResult->getType());
	if (!tt.isSupertype(et) and !et.isSupertype(tt))
	{
		ReportError("incompatible types",
		            SourceRange::Over(thenResult, elseResult));
		return NULL;
	}

	return new Conditional(*ifLoc, condition, thenResult, elseResult,
	                       Type::GetSupertype(tt, et));
}


List* Parser::ListOf(ExprVec* elements)
{
	auto_ptr<ExprVec> e(elements);
	assert(elements != NULL);

	PtrVec<Type> typeParams;
	if (!elements->empty())
	{
		const Expression *elem = *elements->begin();
		typeParams.push_back(&elem->getType());
	}

	const Type *ty = getType("list", typeParams);
	SourceRange loc(savedLoc.begin, lex.CurrentTokenRange().end);

	return new List(*elements, *ty, loc);
}


#if 0
List* List::Take(ExprVec* elements)
{
	std::auto_ptr<PtrVec<Expression> > e(elements);

	assert(elements != NULL);
	// TODO: type
	return NULL;
	//return new List(*e, NULL);
}

List* List::of(ExprVec& elements)
{
	// TODO: actual type
	return NULL;
	//return new List(elements, NULL);
}
#endif


File* Parser::Source(string *name, SourceRange *source, PtrVec<Argument> *args)
{
	auto_ptr<string> n(name);
	auto_ptr<SourceRange> r(source);
	auto_ptr<PtrVec<Argument> > a(args);
	static PtrVec<Argument> empty;

	assert(name != NULL);
	assert(source != NULL);

	return new File(*name, args ? *args : empty, *getType("file"), *source);
}

#if 0

	/**
	 * Create a target file, which only exists at build time as a result
	 * of a build action.
	 */
	File* Target(string *name, const SourceRange *source,
	             PtrVec<Argument> *arguments = NULL);
File* File::Source(string *name, PtrVec<Argument> *args, const Location& loc)
{
	std::auto_ptr<string> n(name);
	std::auto_ptr<PtrVec<Argument> > a(args);
	static PtrVec<Argument> empty;

	assert(name != NULL);

	// TODO: type
	return NULL;
	//return new File(*name, args ? *args : empty, NULL);
}

File* File::Source(const File *orig, PtrVec<Argument> *arguments)
{
	//! This is where (all concatenated) arguments go.
	PtrVec<Argument> args(orig->args);

	std::set<string> argNames;
	for (auto *a : args)
		if (a->hasName())
			argNames.insert(a->getName().name());

	for (auto *a : *arguments)
	{
		auto *arg = dynamic_cast<const Argument*>(a);
		assert(arg->hasName());
		if (argNames.find(arg->getName().name()) == argNames.end())
			args.push_back(arg);
	}

	// TODO: type
	return new File(orig->name, args, NULL);
}


File* File::Target(string *name, PtrVec<Argument> *args)
{
	std::auto_ptr<string> n(name);
	std::auto_ptr<PtrVec<Argument> > a(args);
	static PtrVec<Argument> empty;

	assert(name != NULL);

	// TODO: type
	return NULL;
	//return new File(*name, args ? *args : empty, NULL);
}
#endif


FileList* Parser::Files(PtrVec<File> *files, PtrVec<Argument> *args)
{
	auto_ptr<PtrVec<File> > f(files);
	auto_ptr<PtrVec<Argument> > a(args);
	static PtrVec<Argument> emptyArgs;

	static const Type *ty = fileListType();
	SourceRange loc(SourceRange::None());

	return new FileList(*files, args ? *args : emptyArgs, *ty, loc);
}

#if 0
FileList* FileList::Take(PtrVec<File> *files, PtrVec<Argument> *args)
{
	std::auto_ptr<PtrVec<File> > f(files);

	std::auto_ptr<PtrVec<Argument> > a(args);
	static PtrVec<Argument> emptyArgs;

	// TODO: type
	return NULL;
	//return new FileList(*f, args ? *args : emptyArgs, NULL);
}
#endif

BinaryOperation* Parser::Concat(Expression *lhs, Expression *rhs)
{
	if (lhs == NULL or rhs == NULL)
		return NULL;

	const Type &lt = lhs->getType(), &rt = rhs->getType();
	if (!lt.isSubtype(rt) and !rt.isSubtype(lt))
	{
		ReportError("incompatible types", SourceRange::Over(lhs, rhs));
		return NULL;
	}

	return BinaryOperation::Create(lhs, BinaryOperation::Concatenate, rhs,
	                               Type::GetSupertype(lt, rt));
}


BinaryOperation* Parser::Prefix(Expression *lhs, Expression *rhs)
{
	if (lhs == NULL or rhs == NULL)
		return NULL;

	const Type &lt = lhs->getType(), &rt = rhs->getType();
	if (!rt.isListOf(lt))
	{
		ReportError("incompatible types", SourceRange::Over(lhs, rhs));
		return NULL;
	}

	return BinaryOperation::Create(lhs, BinaryOperation::Prefix, rhs, rt);
}


BinaryOperation* Parser::ScalarAdd(Expression *lhs, Expression *rhs)
{
	if (lhs == NULL or rhs == NULL)
	return NULL;

	const Expression *lst, *element;

	const Type &lt = lhs->getType(), &rt = rhs->getType();
	if (lt.isListOf(rt))
	{
		lst = lhs;
		element = rhs;
	}
	else
	{
		lst = rhs;
		element = lhs;
	}

	// Are we trying to do .+ on unrelated (or non-list) types?
	if (!lst->getType().isListOf(element->getType()))
	{
		ReportError("incompatible types", SourceRange::Over(lhs, rhs));
		return NULL;
	}


	return BinaryOperation::Create(lhs, BinaryOperation::ScalarAdd, rhs,
	                               lst->getType());
}


Argument* Parser::Arg(Expression *e, Identifier *name)
{
	if (e == NULL)
		return NULL;

	return new Argument(name, e);
}


Parameter* Parser::Param(Identifier *name, Expression *defaultValue)
{
	if (name == NULL)
		return NULL;

	auto *p = new Parameter(name, defaultValue);
	CurrentScope().Register(p);

	return p;
}


Identifier* Parser::Id(const CStringRef& name)
{
	return new Identifier(name, NULL, lex.CurrentTokenRange());
}

Identifier* Parser::Id(Identifier *untyped, const Type *ty)
{
	auto_ptr<Identifier> u(untyped);
	assert(!untyped->isTyped());

	SourceRange loc(untyped->getSource().begin, savedLoc.end);

	return new Identifier(u->name(), ty, loc);
}


BoolLiteral* Parser::True()
{
	return new BoolLiteral(true, *getType("bool"),
	                       lex.CurrentTokenRange());
}

BoolLiteral* Parser::False()
{
	return new BoolLiteral(false, *getType("bool"),
	                       lex.CurrentTokenRange());
}

IntLiteral* Parser::ParseInt(int value)
{
	return new IntLiteral(value, *getType("int"),
	                      lex.CurrentTokenRange());
}

StringLiteral* Parser::ParseString(const string& value)
{
	// The lexer's current token range points to the quotation mark that
	// was used to close the string.
	//
	// The real beginning of the source range is value.length() columns
	// before the place the lexer reports. Simple subtraction is safe here
	// because Fabrique strings can't straddle newlines.
	SourceRange current(lex.CurrentTokenRange());
	Location begin(current.begin.filename, current.begin.line,
	               current.begin.column - value.length() - 1);

	assert(begin.column > 0);

	SourceRange loc(begin, current.end);

	return new StringLiteral(value, *getType("string"), loc);
}


const Type* Parser::fileType()
{
	static const Type *t = getType("file");
	return t;
}


const Type* Parser::fileListType()
{
	static const Type *t = getType("list", PtrVec<Type>(1, fileType()));
	return t;
}


const ErrorReport& Parser::ReportError(const string& msg, const HasSource& s)
{
	return ReportError(msg, s.getSource());
}

const ErrorReport& Parser::ReportError(const string& message,
                                       const SourceRange& location)
{
	auto *err(ErrorReport::Create(message, location));
	errs.push_back(err);

	return *err;
}


void Parser::AddToScope(const PtrVec<Argument>& args)
{
	auto& scope(CurrentScope());

	for (auto *arg : args)
		if (arg->hasName())
			scope.Register(arg);
}
