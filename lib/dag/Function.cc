//! @file dag/Function.cc    Definition of @ref fabrique::dag::Function
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

#include <fabrique/Bytestream.hh>
#include <fabrique/dag/Function.hh>
#include <fabrique/dag/Parameter.hh>
#include <fabrique/dag/Visitor.hh>
#include <fabrique/types/FunctionType.hh>
#include <fabrique/types/TypeContext.hh>

using namespace fabrique::dag;
using namespace std::placeholders;


Function* Function::Create(Evaluator fnEval, const Type &resultType,
                           SharedPtrVec<Parameter> parameters, SourceRange source,
                           bool acceptExtraArguments)
{
	PtrVec<Type> paramTypes;
	for (auto &p : parameters)
	{
		paramTypes.push_back(&p->type());
	}

	auto &type = resultType.context().functionType(paramTypes, resultType);

	return new Function(fnEval, parameters, acceptExtraArguments, type, source);
}

Function::Function(Callable::Evaluator evaluator,
                   const SharedPtrVec<Parameter>& parameters, bool acceptExtraArguments,
                   const FunctionType& type, SourceRange source)
	: Callable(parameters, acceptExtraArguments, evaluator), Value(type, source)
{
}

Function::~Function() {}


void Function::PrettyPrint(Bytestream& out, unsigned int indent) const
{
	out << Bytestream::Action << "function " << Bytestream::Reset;
	type().PrettyPrint(out, indent);
	out << "\n";
}

void Function::Accept(Visitor& v) const
{
	v.Visit(*this);
}
