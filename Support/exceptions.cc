/** @file Support/exceptions.cc    Definition of basic Fabrique exceptions. */
/*
 * Copyright (c) 2013, 2018 Jonathan Anderson
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

#include <fabrique/Bytestream.hh>
#include "Support/ErrorReport.h"
#include "Support/exceptions.h"

using namespace fabrique;
using std::string;


AssertionFailure::AssertionFailure(const string& condition, const string& msg)
	: condition_(condition),
	  message_(msg.empty() ? ("Assertion failed: " + condition) : msg)
{
}

AssertionFailure::AssertionFailure(const AssertionFailure& orig)
	: condition_(orig.condition_), message_(orig.message_)
{
}

AssertionFailure::~AssertionFailure() {}

const char* AssertionFailure::what() const noexcept
{
	return message_.c_str();
}


OSError::OSError(const string& message, const string& description)
	: message_(message), description_(description),
	  completeMessage_(message + ": " + description)
{
}

OSError::OSError(const OSError& orig)
	: message_(orig.message_), description_(orig.description_),
	  completeMessage_(orig.completeMessage_)
{
}

OSError::~OSError() {}

void OSError::PrettyPrint(Bytestream& out, unsigned int /*indent*/) const
{
	out
		<< Bytestream::Error << "OS error"
		<< Bytestream::Reset << ": " << message_ << ": "
		<< Bytestream::ErrorMessage << description_
		<< Bytestream::Reset
		;
}


UserError::UserError(const string& message)
	: message_(message)
{
}

UserError::UserError(const UserError& orig)
	: message_(orig.message_)
{
}

UserError::~UserError() {}

void UserError::PrettyPrint(Bytestream& out, unsigned int /*indent*/) const
{
	out << Bytestream::ErrorMessage << message_ << Bytestream::Reset;
}


SourceCodeException::SourceCodeException(string m, SourceRange l, string detail)
	: HasSource(l), err_(m, l, ErrorReport::Severity::Error, detail)
{
}

const string& SourceCodeException::message() const
{
	return err_.getMessage();
}

const string& SourceCodeException::detail() const
{
	return err_.getDetails();
}

const char* SourceCodeException::what() const noexcept
{
	return message().c_str();
}

void SourceCodeException::PrettyPrint(Bytestream& out, unsigned int indent) const
{
	err_.PrettyPrint(out, indent);
}


ParserError::ParserError(string message, SourceRange loc, string detail)
	: SourceCodeException(message, loc, detail)
{
}

ParserError::ParserError(const ParserError& orig)
	: SourceCodeException(orig.message(), orig.source(), orig.detail())
{
}

ParserError::~ParserError() {}


SyntaxError::SyntaxError(string message, SourceRange loc, string detail)
	: SourceCodeException(message, loc, detail)
{
}

SyntaxError::SyntaxError(const SyntaxError& orig)
	: SourceCodeException(orig.message(), orig.source(), orig.detail())
{
}

SyntaxError::~SyntaxError() {}

SemanticException::SemanticException(string m, SourceRange loc, string detail)
	: SourceCodeException(m, loc, detail)
{
}

SemanticException::~SemanticException() {}
