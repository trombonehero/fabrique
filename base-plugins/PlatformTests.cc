/**
 * @file plugins/PlatformTests.cc
 * Definition of @ref fabrique::plugins::PlatformTests.
 */
/*
 * Copyright (c) 2014 Jonathan Anderson
 * All rights reserved.
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

#include "DAG/DAGBuilder.h"
#include "DAG/File.h"
#include "DAG/List.h"
#include "DAG/Parameter.h"
#include "Plugin/Registry.h"
#include "Types/FileType.h"
#include "Types/FunctionType.h"
#include "Types/RecordType.h"
#include "Types/TypeContext.h"
#include "Support/Platform.h"

#include <cassert>
#include <sstream>

#include <errno.h>
#include <stdlib.h>

using namespace fabrique;
using namespace fabrique::dag;
using fabrique::plugin::Plugin;
using std::placeholders::_1;
using std::placeholders::_2;
using std::placeholders::_3;
using std::placeholders::_4;
using std::shared_ptr;
using std::string;
using std::vector;


namespace fabrique {
namespace plugins {

/**
 * Platform detection: defines the bare minimum of constants required to
 * implement platform-specific functionality.
 *
 */
class PlatformTests : public plugin::Plugin
{
	public:
	virtual shared_ptr<dag::Record> Create(dag::DAGBuilder&) const override;

	class Factory : public Plugin::Descriptor
	{
		public:
		virtual string name() const override { return "platform-tests"; }
		virtual UniqPtr<Plugin> Instantiate(TypeContext&) const override;

	};

	private:
	PlatformTests(const Factory& factory, const RecordType& type)
		: Plugin(type, factory)
	{
	}
};

static const char* Platforms[] = {
	// Apple:
	platform::iOS,
	platform::MacOSX,

	// BSD:
	platform::FreeBSD,
	platform::NetBSD,
	platform::OpenBSD,

	// Linux:
	platform::Linux,

	// Windows:
	platform::Win32,
	platform::Win64,
};


UniqPtr<Plugin> PlatformTests::Factory::Instantiate(TypeContext& ctx) const
{
	const SourceRange nowhere = SourceRange::None();

	const Type& boolean = ctx.booleanType();

	Type::NamedTypeVec fields;
	for (const char *platform : Platforms)
		fields.emplace_back(platform, boolean);

	const RecordType& type = ctx.recordType(fields);

	return UniqPtr<Plugin>(new PlatformTests(*this, type));
}


shared_ptr<Record> PlatformTests::Create(DAGBuilder& builder) const
{
	const ValueMap scope;
	static const SourceRange src = SourceRange::None();

	vector<Record::Field> fields;

	for (const char *platform : Platforms)
	{
		const bool isThisPlatform = (platform::Name == platform);
		fields.emplace_back(platform, builder.Bool(isThisPlatform, src));
	}

	auto result = std::dynamic_pointer_cast<Record>(
		builder.Record(fields, type(), SourceRange::None()));

	assert(result);
	return result;
}

static plugin::Registry::Initializer init(new PlatformTests::Factory());

} // plugins namespace
} // fabrique namespace
