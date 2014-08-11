/** @file DAG/Value.h    Declaration of @ref fabrique::dag::Value. */
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

#ifndef DAG_VALUE_H
#define DAG_VALUE_H

#include "ADT/StringMap.h"
#include "Support/Printable.h"
#include "Support/SourceLocation.h"
#include "Support/Uncopyable.h"
#include "Types/Typed.h"

#include <string>

namespace fabrique {

class Type;

namespace dag {

class List;
class Visitor;


class Value;
typedef std::shared_ptr<Value> ValuePtr;


//! The result of evaluating an expression.
class Value : public HasSource, public Printable, public Typed,
              public Uncopyable
{
public:
	virtual bool hasFields() const { return false; }
	virtual ValuePtr field(const std::string& /*name*/) const { return ValuePtr(); }

	//! Unary 'not' operator.
	virtual ValuePtr Negate(const SourceRange& loc) const;

	/**
	 * Add this @ref fabrique::dag::Value to a following @ref Value.
	 *
	 * The implementation of addition is type-dependent: it might make
	 * sense to add, concatenate or apply a logical AND.
	 */
	virtual ValuePtr Add(ValuePtr&) const;

	//! Apply the prefix operation: prefix this value with another value.
	virtual ValuePtr PrefixWith(ValuePtr&) const;

	//! Add another @ref fabrique::dag::Value scalar-wise across this @ref Value.
	virtual ValuePtr ScalarAdd(ValuePtr&) const;

	/** Logical and. */
	virtual ValuePtr And(ValuePtr&) const;

	/** Logical or. */
	virtual ValuePtr Or(ValuePtr&) const;

	/** Logical xor. */
	virtual ValuePtr Xor(ValuePtr&) const;

	/** Equivalence. */
	virtual ValuePtr Equals(ValuePtr&) const;

	//! A list representation of this value (or nullptr).
	virtual const List* asList() const { return nullptr; }

	/**
	 * This @ref Value can add a value to itself in a scalar fashion.
	 * For instance, [ 1 2 ] can add 3 to itself but not vice versa.
	 */
	virtual bool canScalarAdd(const Value&) const { return false; }

	virtual void Accept(Visitor&) const = 0;

protected:
	Value(const Type&, const SourceRange&);
};

typedef StringMap<ValuePtr> ValueMap;

} // namespace dag
} // namespace fabrique

#endif // !DAG_VALUE_H
