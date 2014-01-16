/** @file Build.h    Declaration of @ref Build. */
/*
 * Copyright (c) 2014 Jonathan Anderson
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

#ifndef DAG_BUILD_H
#define DAG_BUILD_H

#include "DAG/DAG.h"
#include "DAG/Value.h"

#include <memory>
#include <vector>


namespace fabrique {
namespace dag {

class File;
class Rule;


/**
 * An application of a @ref Rule to transform @ref File objects.
 */
class Build : public Value
{
public:
	typedef std::vector<std::shared_ptr<File>> FileVec;

	static Build* Create(std::shared_ptr<Rule>&,
	                     std::shared_ptr<Value> in,
	                     std::shared_ptr<Value> out,
	                     const ValueMap& arguments,
	                     const SourceRange src);

	virtual ~Build() {}

	const Rule& buildRule() const { return *rule; }
	const FileVec& inputs() const { return in; }
	const FileVec& outputs() const { return out; }
	const ValueMap& arguments() const { return args; }

	std::string type() const { return "build"; }
	std::string str() const;

	void PrettyPrint(Bytestream&, int indent = 0) const;

private:
	Build(std::shared_ptr<Rule>&,
	      SharedPtrVec<File>& inputs,
	      SharedPtrVec<File>& outputs,
	      const ValueMap& arguments,
	      SourceRange src);

	static void appendFiles(std::shared_ptr<Value>& in,
	                        SharedPtrVec<File>& out);

	std::shared_ptr<Rule> rule;
	SharedPtrVec<File> in;
	SharedPtrVec<File> out;
	ValueMap args;
};

} // namespace dag
} // namespace fabrique

#endif