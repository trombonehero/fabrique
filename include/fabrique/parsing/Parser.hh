//! @file parsing/Parser.hh    Declaration of @ref fabrique::parsing::Parser
/*
 * Copyright (c) 2013-2014, 2018-2019 Jonathan Anderson
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

#ifndef PARSER_H
#define PARSER_H

#include <fabrique/AssertionFailure.hh>
#include <fabrique/ErrorReport.hh>
#include <fabrique/UniqPtr.hh>
#include <fabrique/ast/ast.hh>

#include <unordered_map>

namespace fabrique {
namespace parsing {


/**
 * Parser for Fabrique files and/or values defined at the command line.
 */
class Parser
{
public:
	/**
	 * Constructor.
	 *
	 * @param    prettyPrint      pretty-print values or files as they are parsed
	 * @param    dump             dump values as they are parsed
	 */
	Parser(bool prettyPrint, bool dump);

	template<typename T>
	class Result
	{
	public:
		operator bool() const { return result; }

		const T& ok() const
		{
			FAB_ASSERT(result, "calling ok() on invalid parse result");
			return *result;
		}

		const std::vector<ErrorReport>& errors()
		{
			return errs;
		}

		static Result Ok(const T &value) { return Result(&value, {}); }
		static Result Err(std::vector<ErrorReport> errs)
		{
			FAB_ASSERT(not errs.empty(),
			           "Parser::Result::Err() has no errors");

			return Result(nullptr, errs);
		}

	private:
		Result(const T *value, std::vector<ErrorReport> errors)
			: result(value), errs(errors)
		{
		}

		const T *result;
		std::vector<ErrorReport> errs;
	};

	using ValueResult = Result<ast::Value>;
	using FileResult = Result<UniqPtrVec<ast::Value>>;

	//! Parse a single value (e.g., a command-line definition)
	ValueResult Parse(std::string, SourceRange = SourceRange::None());

	//! Parse Fabrique input (usually a file) into @ref Value objects.
	FileResult ParseFile(std::istream&, std::string name = "");

	//! What inputs have we parsed?
	std::vector<std::string> inputs() const;

	//! Retrive a previously-parsed tree.
	const UniqPtrVec<ast::Value>& parseTree(const std::string &name);

private:
	const bool prettyPrint_;
	const bool dump_;

	//! Input sources of trees we've parsed.
	std::vector<std::string> inputs_;

	/**
	 * ASTs that have been generated by this parser.
	 *
	 * Parse trees need to be stored somewhere, and this provides the ability to
	 * recall the contents of named files later.
	 */
	std::unordered_map<std::string, UniqPtrVec<ast::Value>> parseTrees_;
};

} // namespace parsing
} // namespace fabrique

#endif
