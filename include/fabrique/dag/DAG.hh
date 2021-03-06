//! @file dag/DAG.hh    Declaration of @ref fabrique::dag::DAG
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

#ifndef DAG_H
#define DAG_H

#include <fabrique/Printable.hh>
#include <fabrique/dag/Value.hh>

#include <string>


namespace fabrique {

//! Representations of nodes (files, build rules, etc.) in the build graph.
namespace dag {

class Build;
class File;
class Rule;


/**
 * A directed acyclic graph of build actions.
 */
class DAG : public Printable
{
public:
	virtual const SharedPtrVec<File>& files() const = 0;
	virtual const SharedPtrVec<Build>& builds() const = 0;
	virtual const SharedPtrMap<Rule>& rules() const = 0;
	virtual const SharedPtrMap<Value>& variables() const = 0;
	virtual const SharedPtrMap<Value>& targets() const = 0;

	typedef std::pair<std::string,ValuePtr> BuildTarget;

	//! A file's top-level targets, in order of original definition.
	virtual const std::vector<BuildTarget>& topLevelTargets() const = 0;

	virtual void PrettyPrint(Bytestream&, unsigned int indent = 0) const override;
};

} // namespace dag
} // namespace fabrique

#endif
