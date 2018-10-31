/** @file DAG/EvalContext.cc    Definition of @ref fabrique::ast::EvalContext. */
/*
 * Copyright (c) 2014, 2018 Jonathan Anderson
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme and at Memorial University
 * of Newfoundland under the NSERC Discovery program (RGPIN-2015-06048).
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

#include <fabrique/ast/Builtins.hh>
#include <fabrique/ast/EvalContext.hh>
#include <fabrique/ast/Parameter.hh>
#include <fabrique/ast/Value.hh>

#include <fabrique/dag/DAGBuilder.hh>

#include "Support/Bytestream.h"
#include "Support/Join.h"
#include "Support/exceptions.h"
#include "Support/os.h"

#include <fabrique/types/FileType.hh>
#include <fabrique/types/FunctionType.hh>
#include <fabrique/types/TypeContext.hh>

#include <cassert>

using namespace fabrique;
using namespace fabrique::ast;
using fabrique::dag::DAG;
using fabrique::dag::ValueMap;
using fabrique::dag::ValuePtr;

using std::shared_ptr;
using std::string;
using std::vector;


EvalContext::EvalContext(TypeContext& ctx, ValueMap builtins)
	: ctx_(ctx), builder_(*this), builtins_(std::move(builtins))
{
	// Create top-level scope
	scopes_.emplace_back();
}

std::vector<DAG::BuildTarget> EvalContext::Evaluate(const UniqPtrVec<ast::Value>& values)
{
	vector<DAG::BuildTarget> topLevelTargets;

	for (const UniqPtr<ast::Value>& v : values)
	{
		if (auto &name = v->name())
		{
			topLevelTargets.emplace_back(name->name(), v->evaluate(*this));
		}
	}

	return topLevelTargets;
}


EvalContext::Scope::Scope(EvalContext& stack, string name, ValueMap& symbols)
	: stack_(stack), name_(name), symbols_(symbols)
{
}

EvalContext::Scope::Scope(Scope&& other)
	: stack_(other.stack_),
	  name_(std::move(other.name_)),
	  symbols_(other.symbols_)
{
}

EvalContext::Scope::~Scope()
{
	if (name_.empty())
		return;

	stack_.PopScope();
}

void EvalContext::Scope::set(string name, ValuePtr v)
{
	stack_.CurrentScope()[name] = v;
}

bool EvalContext::Scope::contains(const string& name)
{
	return (symbols_.find(name) != symbols_.end());
}

ValueMap EvalContext::Scope::leave()
{
	name_ = "";
	return stack_.PopScope();
}


EvalContext::Scope EvalContext::EnterScope(const string& name)
{
	Bytestream::Debug("eval.scope")
		<< string(scopes_.size(), ' ')
		<< Bytestream::Operator << " >> "
		<< Bytestream::Type << "scope"
		<< Bytestream::Literal << " '" << name << "'"
		<< Bytestream::Reset << "\n"
		;

	scopes_.push_back(ValueMap());
	return Scope(*this, name, CurrentScope());
}


EvalContext::AlternateScoping::AlternateScoping(EvalContext& stack,
                                              std::deque<ValueMap>&& scopes)
	: stack_(stack), originalScopes_(scopes)
{
}

EvalContext::AlternateScoping::AlternateScoping(AlternateScoping&& other)
	: stack_(other.stack_), originalScopes_(std::move(other.originalScopes_))
{
}

EvalContext::AlternateScoping::~AlternateScoping()
{
	if (not originalScopes_.empty())
		stack_.scopes_ = std::move(originalScopes_);
}


EvalContext::AlternateScoping EvalContext::ChangeScopeStack(const ValueMap& altScope)
{
	std::deque<ValueMap> originalScopes = std::move(scopes_);
	scopes_.push_back(altScope);

	return AlternateScoping(*this, std::move(originalScopes));
}


EvalContext::ScopedValueName::ScopedValueName(EvalContext& stack, string name)
	: stack_(stack), name_(name)
{
	stack_.PushValueName(name_);
}

EvalContext::ScopedValueName::ScopedValueName(ScopedValueName&& other)
	: stack_(other.stack_), name_(std::move(other.name_))
{
}

EvalContext::ScopedValueName::~ScopedValueName()
{
	string poppedName = stack_.PopValueName();
	assert(poppedName == name_);
}


EvalContext::ScopedValueName EvalContext::evaluating(const string& name)
{
	return ScopedValueName(*this, name);
}


ValueMap EvalContext::PopScope()
{
	assert(not scopes_.empty());

	ValueMap values = std::move(CurrentScope());
	scopes_.pop_back();

	Bytestream& dbg = Bytestream::Debug("parser.scope");
	dbg
		<< string(scopes_.size(), ' ')
		<< Bytestream::Operator << " << "
		<< Bytestream::Type << "scope"
		<< Bytestream::Operator << ":"
		;

	for (auto& i : values)
		dbg << " " << i.first;

	dbg << Bytestream::Reset << "\n";

	return values;
}

ValueMap& EvalContext::CurrentScope()
{
	assert(not scopes_.empty());
	return scopes_.back();
}

void EvalContext::DumpScope()
{
	Bytestream& out = Bytestream::Debug("dag.scope");
	size_t depth = 0;

	out
		<< Bytestream::Operator << "---------------------------\n"
		<< Bytestream::Definition << "Scopes (parent -> current):\n"
		<< Bytestream::Operator << "---------------------------\n"
		;

	for (auto scope = scopes_.begin(); scope != scopes_.end(); scope++)
	{
		const string indent("  ", depth);
		for (auto i : *scope)
		{
			const string name(i.first);
			const ValuePtr value(i.second);

			out << indent
				<< Bytestream::Operator << "- "
				<< Bytestream::Definition << name
				<< Bytestream::Operator << ": "
				<< *value
				<< Bytestream::Reset << "\n"
				;
		}

		depth++;
	}

	out
		<< Bytestream::Operator << "---------------------------\n"
		<< Bytestream::Reset
		;
}

ValueMap EvalContext::CopyCurrentScope()
{
	ValueMap copy;

	for (auto i = scopes_.rbegin(); i != scopes_.rend(); i++)
	{
		// Unfortunately, std::copy() doesn't work here because
		// it wants to copy *into* the string part of a const
		// pair<string,SharedPtr<Value>>.
		for (auto j : *i)
		{
			string name = j.first;
			ValuePtr value = j.second;

			copy.emplace(name, value);
		}
	}

	return copy;
}


void EvalContext::Define(ScopedValueName& name, ValuePtr v, SourceRange src)
{
	assert(&name.stack_ == this);

	ValueMap& currentScope = CurrentScope();
	SemaCheck(currentScope.find(name.name_) == currentScope.end(), src,
		"redefining '" + name.name_ + "'");

	currentScope.emplace(name.name_, v);
	builder_.Define(fullyQualifiedName(), v);
}


ValuePtr EvalContext::Lookup(const string& name, SourceRange src)
{
	Bytestream& dbg = Bytestream::Debug("dag.lookup");
	dbg
		<< Bytestream::Action << "lookup "
		<< Bytestream::Literal << "'" << name << "'"
		<< Bytestream::Reset << "\n"
		;

	// Try builtins first:
	auto b = builtins_.find(name);
	if (b != builtins_.end())
	{
		return b->second;
	}

	// Next, look for lexically-defined names:
	for (auto i = scopes_.rbegin(); i != scopes_.rend(); i++)
	{
		const ValueMap& scope = *i;

		auto value = scope.find(name);
		if (value != scope.end())
		{
			FAB_ASSERT(value->second, "value-less value found in scope");

			dbg
				<< Bytestream::Action << "  found "
				<< Bytestream::Literal << "'" << name << "'"
				<< Bytestream::Operator << ": "
				<< *value->second
				<< Bytestream::Reset << "\n"
				;
			return value->second;
		}

		dbg
			<< "  no "
			<< Bytestream::Literal << "'" << name << "'"
			<< Bytestream::Operator << ":"
			;

		for (auto& j : scope)
			dbg << " " << Bytestream::Definition << j.first;

		dbg << Bytestream::Reset << "\n";
	}

	// If we are looking for 'builddir' or 'subdir' and haven't found it
	// defined anywhere, provide the top-level build/source subdirectory ('').
	if (name == ast::builtins::BuildDirectory)
		return builder_.File("", ValueMap(), SourceRange::None(), true);

	if (name == ast::builtins::Subdirectory)
		return builder_.File("");

	throw SemanticException("reference to undefined name", src);
}


string EvalContext::currentValueName() const
{
	return fullyQualifiedName();
}

string EvalContext::fullyQualifiedName() const
{
	return join(currentValueName_, ".");
}

string EvalContext::qualifyName(string name) const
{
	std::deque<string> copy(currentValueName_);
	copy.push_back(name);

	return join(copy, ".");
}

void EvalContext::PushValueName(const string& name)
{
	currentValueName_.push_back(name);
}

string EvalContext::PopValueName()
{
	const string name = currentValueName_.back();
	currentValueName_.pop_back();
	return name;
}


ValuePtr EvalContext::Function(const dag::Function::Evaluator fn,
                               const SharedPtrVec<dag::Parameter>& params,
                               const Type& resultType, SourceRange source)
{
	return builder_.Function(fn, resultType, params, source);
}
