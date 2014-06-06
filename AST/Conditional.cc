/** @file AST/Conditional.cc    Definition of @ref fabrique::ast::Conditional. */
/*
 * Copyright (c) 2013-2014 Jonathan Anderson
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

#include "AST/CompoundExpr.h"
#include "AST/Conditional.h"
#include "AST/Value.h"
#include "AST/Visitor.h"
#include "Support/Bytestream.h"

using namespace fabrique::ast;


Conditional::Conditional(const SourceRange& ifLoc,
                         UniqPtr<Expression>& condition,
                         UniqPtr<Expression>& thenResult,
                         UniqPtr<Expression>& elseResult,
                         const Type& ty)
	: Expression(ty, SourceRange(ifLoc.begin, elseResult->source().end)),
	  condition_(std::move(condition)),
	  thenClause_(std::move(thenResult)),
	  elseClause_(std::move(elseResult))
{
}

void Conditional::PrettyPrint(Bytestream& out, size_t /*indent*/) const
{
	out
		<< Bytestream::Operator << "if ("
		<< *condition_
		<< Bytestream::Operator << ")\n"
		<< *thenClause_
		<< Bytestream::Operator << "\nelse\n"
		<< *elseClause_
		<< Bytestream::Reset
		;
}


void Conditional::Accept(Visitor& v) const
{
	if (v.Enter(*this))
	{
		condition_->Accept(v);
		thenClause_->Accept(v);
		elseClause_->Accept(v);
	}

	v.Leave(*this);
}
