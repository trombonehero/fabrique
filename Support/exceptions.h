/** @file Support/exceptions.h    Declaration of basic Fabrique exceptions. */
/*
 * Copyright (c) 2013, 2018-2019 Jonathan Anderson
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

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <fabrique/SourceCodeException.hh>

#include <exception>
#include <memory>
#include <string>

namespace fabrique {


//! An error in user input.
class UserError : public std::exception, public Printable
{
public:
	UserError(const std::string& message);
	UserError(const UserError&);
	virtual ~UserError() override;

	virtual const std::string& message() const { return message_; }
	const char* what() const noexcept override { return message_.c_str(); }

	virtual void PrettyPrint(Bytestream&, unsigned int indent = 0) const override;

private:
	const std::string message_;
};


//! A parser assertion failed.
class ParserError : public SourceCodeException
{
public:
	ParserError(std::string message, SourceRange, std::string detail = "");
	ParserError(const ParserError&);
	virtual ~ParserError() override;
};

//! A syntactic error is present in the Fabrique description.
class SyntaxError : public SourceCodeException
{
public:
	SyntaxError(std::string message, SourceRange, std::string detail = "");
	SyntaxError(const SyntaxError&);
	virtual ~SyntaxError() override;
};

//! A semantic error is present in the Fabrique description.
class SemanticException : public SourceCodeException
{
public:
	SemanticException(std::string message, SourceRange, std::string detail = "");
	SemanticException(const SemanticException&);
	virtual ~SemanticException() override;
};

template<typename T>
void SemaCheck(const T &condition, SourceRange src, std::string message)
{
	if (not condition)
	{
		throw SemanticException(message, src);
	}
}

}

#endif
