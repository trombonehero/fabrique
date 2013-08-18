/** @file BinaryOperator.cc    Definition of @ref BinaryOperator. */
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

#include "BinaryOperator.h"
#include "ostream.h"
using std::ostream;


BinaryOperator::Operation BinaryOperator::Op(CStringRef o)
{
	Operation op;

	if (o == "++") op = Concatenate;
	else if (o == "::") op = Prefix;
	else if (o == ".+") op = ScalarAdd;
	else op = Invalid;

	assert(o == OpStr(op));

	return op;
}

std::string BinaryOperator::OpStr(Operation op)
{
	switch (op)
	{
		case Concatenate:       return "++";
		case Prefix:            return "::";
		case ScalarAdd:         return ".+";
		case Invalid:           assert(false && "op == Invalid");
	}

	assert(false && "unhandled Operation type");
}

BinaryOperator* BinaryOperator::Take(
	Expression *LHS, Operation op, Expression *RHS)
{
	assert(op < Invalid);
	return new BinaryOperator(LHS, op, RHS);
}

bool BinaryOperator::isStatic() const
{
	return (LHS->isStatic() and RHS->isStatic());
}


void BinaryOperator::PrettyPrint(ostream& out, int indent) const
{
	LHS->PrettyPrint(out, indent);
	out << " " << Yellow << OpStr(op) << ResetAll << " ";
	RHS->PrettyPrint(out, indent);
}
