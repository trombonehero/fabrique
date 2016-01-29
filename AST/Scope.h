/** @file AST/Scope.h    Declaration of @ref fabrique::ast::Scope. */
/*
 * Copyright (c) 2013, 2016 Jonathan Anderson
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

#ifndef SCOPE_H
#define SCOPE_H

#include "ADT/PtrVec.h"
#include "ADT/StringMap.h"
#include "AST/Value.h"
#include "Support/Printable.h"
#include "Support/Uncopyable.h"
#include "Types/Type.h"

#include <map>
#include <memory>
#include <string>

namespace fabrique {

class TypeContext;

namespace ast {

class Argument;
class Expression;
class Identifier;
class Parameter;
class Value;
class Visitor;


/**
 * A scope is a container for name->value mappings.
 *
 * A scope can have a parent scope for recursive name lookups.
 */
class Scope : public Node
{
public:
	static const Scope& None();
	static UniqPtr<Scope> Create(UniqPtrVec<Value>, const Scope *parent = nullptr);

	virtual ~Scope();

	bool contains(const Identifier&) const;
	const UniqPtr<Value>& Lookup(const Identifier&) const;

	//! Values defined in the scope, in AST order.
	virtual const UniqPtrVec<Value>& values() const = 0;

	virtual void PrettyPrint(Bytestream&, size_t indent) const override;
	virtual void Accept(Visitor&) const override;

	class Parser : public Node::Parser
	{
	public:
		virtual ~Parser();
		Scope* Build(const Scope&, TypeContext&, Err&) const override;

	private:
		ChildNodes<Value> values_;
	};

protected:
	Scope(SourceRange src, const Scope* parent = nullptr);
	const Scope *parent_;
};

} // namespace ast
} // namespace fabrique

#endif
