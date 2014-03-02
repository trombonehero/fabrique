/**
 * @file Support/SourceLocation.h
 * Declaration of @ref fabrique::HasSource, @ref SourceLocation and @ref SourceRange.
 */
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

#ifndef LOCATION_H
#define LOCATION_H

#include "ADT/UniqPtr.h"
#include "Support/Printable.h"

#include <string>

namespace fabrique {

class Lexer;


//! A location in the original source code.
class SourceLocation : public Printable
{
public:
	//static const SourceLocation& Nowhere();

	SourceLocation(const std::string& filename = "",
	               size_t line = 0, size_t column = 0);

	operator bool() const;
	bool operator == (const SourceLocation&) const;
	bool operator != (const SourceLocation&) const;

	virtual void PrettyPrint(Bytestream&, size_t indent = 0) const override;

	std::string filename;
	size_t line;
	size_t column;
};


class HasSource;

class SourceRange : public Printable
{
public:
	static const SourceRange& None();

	//! Construct a short (within a single line) range.
	static SourceRange Span(
		const std::string& filename, size_t line,
		size_t begincol, size_t endcol);

	//! Create a range that spans two @ref fabrique::HasSource objects.
	template<class T1, class T2>
	static SourceRange Over(const T1& b, const T2& e)
	{
		const static SourceRange& nowhere = SourceRange::None();

		const SourceRange& begin =
			b ? b->source() : (e ? e->source() : nowhere);

		const SourceRange& end =
			e ? e->source() : (b ? b->source() : nowhere);

		return SourceRange(begin.begin, end.end);
	}

	SourceRange(const SourceLocation& begin, const SourceLocation& end);
	SourceRange(const SourceRange& begin, const SourceRange& end);
	SourceRange(const HasSource& begin, const HasSource& end);

	operator bool() const;
	bool operator == (const SourceRange&) const;
	bool operator != (const SourceRange&) const;

	virtual void PrettyPrint(Bytestream&, size_t indent = 0) const override;

	SourceLocation begin;
	SourceLocation end;
};


//! A mixin type for something that has a location in source code.
class HasSource
{
public:
	HasSource(const SourceRange& src) : src_(src) {}
	const SourceRange& source() const { return src_; }

private:
	SourceRange src_;
};

} // class fabrique

#endif
