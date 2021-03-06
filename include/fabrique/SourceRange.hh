/**
 * @file  fabrique/SourceRange.hh
 * @brief Declaration of @ref fabrique::HasSource and @ref fabrique::SourceRange
 */
/*
 * Copyright (c) 2013, 2016, 2018-2019 Jonathan Anderson
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

#ifndef FAB_SOURCE_RANGE_H_
#define FAB_SOURCE_RANGE_H_

#include <fabrique/Printable.hh>
#include <fabrique/SourceLocation.hh>

#include <string>

namespace fabrique {

class HasSource;


//! A range of characters in source code.
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
	static SourceRange Over(const T1& x, const T2& y)
	{
		const static SourceRange& nowhere = SourceRange::None();

		const SourceRange& xsrc = x ? x->source() : nowhere;
		const SourceRange& ysrc = y ? y->source() : nowhere;

		const SourceRange& begin = (xsrc and xsrc < ysrc) ? xsrc : ysrc;
		const SourceRange& end = (ysrc and xsrc < ysrc) ? ysrc : xsrc;

		return SourceRange(begin.begin, end.end);
	}

	//! Create a range over a map of @ref fabrique::HasSource objects.
	template<class T>
	static SourceRange Over(const T& values)
	{
		SourceLocation begin, end;

		for (auto i : values)
		{
			const SourceLocation& b = i.second->source().begin;
			const SourceLocation& e = i.second->source().end;

			if (not begin or b < begin)
				begin = b;

			if (not end or e > end)
				end = e;
		}

		return SourceRange(begin, end);
	}

	SourceRange(const SourceLocation& begin, const SourceLocation& end);
	SourceRange(const SourceRange& begin, const SourceRange& end);
	SourceRange(const HasSource& begin, const HasSource& end);

	operator bool() const;
	bool operator < (const SourceRange&) const;
	bool operator > (const SourceRange&) const;
	bool operator == (const SourceRange&) const;
	bool operator != (const SourceRange&) const;

	bool isInside(const SourceRange&) const;

	std::string filename() const;

	Bytestream& PrintSource(Bytestream&, SourceLocation caret = SourceLocation(),
	                        unsigned int contextLines = 3) const;
	virtual void PrettyPrint(Bytestream&, unsigned int indent = 0) const override;

	SourceLocation begin;
	SourceLocation end;
};

} // class fabrique

#endif  // FAB_SOURCE_RANGE_H_
