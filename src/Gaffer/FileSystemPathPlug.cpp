//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2011-2012, John Haddon. All rights reserved.
//  Copyright (c) 2011-2015, Image Engine Design Inc. All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without
//  modification, are permitted provided that the following conditions are
//  met:
//
//      * Redistributions of source code must retain the above
//        copyright notice, this list of conditions and the following
//        disclaimer.
//
//      * Redistributions in binary form must reproduce the above
//        copyright notice, this list of conditions and the following
//        disclaimer in the documentation and/or other materials provided with
//        the distribution.
//
//      * Neither the name of John Haddon nor the names of
//        any other contributors to this software may be used to endorse or
//        promote products derived from this software without specific prior
//        written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
//  IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
//  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
//  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
//  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
//  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
//  PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
//  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
//  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//////////////////////////////////////////////////////////////////////////

#include "Gaffer/FileSystemPathPlug.h"
#include "Gaffer/FileSystemPath.h"
#include "Gaffer/PathFilter.h"

#include "Gaffer/Context.h"
#include "Gaffer/Process.h"

using namespace IECore;
using namespace Gaffer;

IE_CORE_DEFINERUNTIMETYPED( FileSystemPathPlug );

namespace
{

	// Before using the HashProcess/ComputeProcess classes to get a hash or
	// a value, we first traverse back down the chain of input plugs to the
	// start, or till we find a plug of a different type. This traversal is
	/// much quicker than using the Process classes for every step in the chain
	/// and avoids the creation of lots of unnecessary cache entries.
	inline const ValuePlug *sourcePlug(const ValuePlug *p)
	{
		const IECore::TypeId typeId = p->typeId();

		const ValuePlug *in = p->getInput<ValuePlug>();
		while (in && in->typeId() == typeId)
		{
			p = in;
			in = p->getInput<ValuePlug>();
		}

		return p;
	}

} // namespace

FileSystemPathPlug::FileSystemPathPlug(
	const std::string &name,
	Direction direction,
	const std::string &defaultValue,
	unsigned flags,
	unsigned substitutions
)
	:	StringPlug( name, direction, defaultValue, flags, substitutions )
{
}

FileSystemPathPlug::~FileSystemPathPlug()
{
}

bool FileSystemPathPlug::acceptsInput( const Plug *input ) const
{
	if( !ValuePlug::acceptsInput( input ) )
	{
		return false;
	}
	if( input )
	{
		return input->isInstanceOf( staticTypeId() ) || input->isInstanceOf( StringPlug::staticTypeId() );
	}
	return true;
}

PlugPtr FileSystemPathPlug::createCounterpart( const std::string &name, Direction direction ) const
{
	return new FileSystemPathPlug( name, direction, defaultValue(), getFlags(), substitutions() );
}

void FileSystemPathPlug::setValue(const std::string &value)
{
	// Set the value to the generic string representation
	// This helps when calling getValue since it won't remove
	// back slashes when calling substitute
	setObjectValue(new StringData(Gaffer::FileSystemPath(value).string()));
}

std::string FileSystemPathPlug::getValue( const IECore::MurmurHash *precomputedHash, const Context *context, const bool forceSubstitutions ) const
{
	if( !context )
	{
		context = Context::current();
	}
	IECore::ConstObjectPtr o = getObjectValue(precomputedHash);
	const IECore::StringData *s = IECore::runTimeCast<const IECore::StringData>(o.get());
	if (!s)
	{
		throw IECore::Exception("FileSystemPathPlug::getObjectValue() didn't return StringData - is the hash being computed correctly?");
	}

	const bool performSubstitutions =
		forceSubstitutions ||
		(substitutions() &&
			direction() == In &&
			Process::current() &&
			Context::hasSubstitutions(s->readable()));
	const std::string substituted_path = performSubstitutions ? context->substitute(s->readable(), substitutions()) : s->readable();
	return Gaffer::FileSystemPath(substituted_path).nativeString();
}

void FileSystemPathPlug::setFrom(const ValuePlug *other)
{
	if (const FileSystemPathPlug *tOther = runTimeCast<const FileSystemPathPlug >(other))
	{
		setValue(tOther->getValue());
	}
	else if (const StringPlug *tOther = runTimeCast<const StringPlug>(other))
	{
		setValue(tOther->getValue());
	}
	else
	{
		throw IECore::Exception("Unsupported plug type");
	}
}

IECore::MurmurHash FileSystemPathPlug::hash() const
{
	const bool performSubstitutions =
		substitutions() &&
		direction() == In
		;

	// We need to allow backwards compatibility with StringPlug.
	// Check to see if our root plug is a StringPlug and return
	// its hash.
	const ValuePlug *r = sourcePlug(this);
	if (const StringPlug *p = runTimeCast<const StringPlug>(r->getInput()))
	{
		return p->hash();
	}
	if (performSubstitutions)
	{
		IECore::ConstObjectPtr o = getObjectValue();
		const IECore::StringData *s = IECore::runTimeCast<const IECore::StringData>(o.get());
		if (!s)
		{
			throw IECore::Exception("StringPlug::getObjectValue() didn't return StringData - is the hash being computed correctly?");
		}

		if (Context::hasSubstitutions(s->readable()))
		{
			IECore::MurmurHash result;
			result.append(Context::current()->substitute(s->readable(), substitutions()));
			return result;
		}
	}

	// no substitutions
	return ValuePlug::hash();
}
