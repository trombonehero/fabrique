//! @file ast/Record.cc    Definition of @ref fabrique::ast::Record
/*
 * Copyright (c) 2014, 2018 Jonathan Anderson
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

#include <fabrique/names.hh>
#include <fabrique/Bytestream.hh>
#include <fabrique/ast/EvalContext.hh>
#include <fabrique/ast/Record.hh>
#include <fabrique/ast/Visitor.hh>

using namespace fabrique;
using namespace fabrique::ast;


Record::Record(UniqPtrVec<Value> fields, SourceRange loc)
	: Expression(loc), fields_(std::move(fields))
{
}

void Record::PrettyPrint(Bytestream& out, unsigned int indent) const
{
	const std::string outerTabs(indent, '\t');

	out
		<< Bytestream::Definition << "record\n"
		<< Bytestream::Operator << outerTabs << "{\n"
		;

	for (auto& f : fields_)
	{
		f->PrettyPrint(out, indent + 1);
		out << "\n";
	}

	out
		<< Bytestream::Operator << outerTabs << "}"
		<< Bytestream::Reset
		;
}

void Record::Accept(Visitor& v) const
{
	if (v.Enter(*this))
	{
		for (auto& f : fields_)
		{
			f->Accept(v);
		}
	}

	v.Leave(*this);
}

dag::ValuePtr Record::evaluate(EvalContext& ctx) const
{
	auto instantiationScope(ctx.EnterScope(names::Record));

	dag::ValueMap fields;
	for (auto& field : fields_)
	{
		auto &name = field->name();
		SemaCheck(name, field->source(), "record fields must have names");

		fields[name->name()] = field->evaluate(ctx);
	}

	return ctx.builder().Record(fields, source());
}
