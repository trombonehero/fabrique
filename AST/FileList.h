/** @file FileList.h    Declaration of @ref FileList. */
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

#ifndef FILE_LIST_H
#define FILE_LIST_H

#include "AST/Argument.h"
#include "AST/Expression.h"
#include "AST/Filename.h"

namespace fabrique {
namespace ast {

/**
 * A list of files, with optional arguments that can be applied to each file.
 */
class FileList : public Expression
{
public:
	FileList(PtrVec<Filename>& files, PtrVec<Argument>& args,
	         const Type& ty, const SourceRange& loc)
		: Expression(ty, loc), files(files), args(args)
	{
	}

	PtrVec<Filename>::const_iterator begin() const { return files.begin(); }
	PtrVec<Filename>::const_iterator end() const { return files.end(); }

	virtual bool isStatic() const;
	virtual void PrettyPrint(Bytestream&, int indent = 0) const;

	virtual void Accept(Visitor&) const;

private:
	PtrVec<Filename> files;
	PtrVec<Argument> args;
};

} // namespace ast
} // namespace fabrique

#endif
