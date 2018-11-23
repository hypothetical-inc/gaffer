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

#ifndef GAFFER_FILESYSTEMPATHPLUG_H
#define GAFFER_FILESYSTEMPATHPLUG_H

#include "Gaffer/Context.h"
#include "Gaffer/StringPlug.h"


namespace Gaffer
{

/// Plug for providing file system path values.
///
/// Inherit from StringPlug so all substitutions work and it 
/// is backwards compatible with Gaffer scripts from previous versions.
///
/// Gaffer standardizes on using forward slashes for separating directories
/// in a path in the UI. Pulling on the FileSystemPathPlug returns an OS
/// native format string. Directories are separated by forward slashes on 
/// POSIX systems and back slashes on Windows.
/// On Windows, interpret a path starting with a forward slash as a UNC
/// path, converting it to a double back slash.

class GAFFER_API FileSystemPathPlug : public StringPlug
{

	public :

		typedef std::string ValueType;

		IE_CORE_DECLARERUNTIMETYPEDEXTENSION( Gaffer::FileSystemPathPlug, FileSystemPathPlugTypeId, StringPlug );


		FileSystemPathPlug(
			const std::string &name = defaultName<FileSystemPathPlug>(),
			Direction direction=In,
			const std::string &defaultValue = "",
			unsigned flags = Default,
			unsigned substitutions = Context::AllSubstitutions
		);
		~FileSystemPathPlug() override;

		/// Accepts instances StringPlug or derived classes,
		/// which includes FileSystemPath Plug.
		bool acceptsInput( const Plug *input ) const override;
		PlugPtr createCounterpart( const std::string &name, Direction direction ) const override;

		/// \undoable
		void setValue(const std::string &value);
		/// Returns the value. See comments in TypedObjectPlug::getValue()
		/// for details of the optional precomputedHash argument - and use
		/// with care!
		std::string getValue( const IECore::MurmurHash *precomputedHash = nullptr, const Context *context = nullptr, const bool forceSubstitutions = false ) const;

		void setFrom(const ValuePlug *other) override;

		IECore::MurmurHash hash() const override;
		/// Ensures the method above doesn't mask
		/// ValuePlug::hash( h )
		using ValuePlug::hash;
};

IE_CORE_DECLAREPTR( FileSystemPathPlug );

typedef FilteredChildIterator<PlugPredicate<Plug::Invalid, FileSystemPathPlug> > FileSystemPathPlugIterator;
typedef FilteredChildIterator<PlugPredicate<Plug::In, FileSystemPathPlug> > InputFileSystemPathPlugIterator;
typedef FilteredChildIterator<PlugPredicate<Plug::Out, FileSystemPathPlug> > OutputFileSystemPathPlugIterator;

typedef FilteredRecursiveChildIterator<PlugPredicate<Plug::Invalid, FileSystemPathPlug>, PlugPredicate<> > RecursiveFileSystemPathPlugIterator;
typedef FilteredRecursiveChildIterator<PlugPredicate<Plug::In, FileSystemPathPlug>, PlugPredicate<> > RecursiveInputFileSystemPathPlugIterator;
typedef FilteredRecursiveChildIterator<PlugPredicate<Plug::Out, FileSystemPathPlug>, PlugPredicate<> > RecursiveOutputFileSystemPathPlugIterator;

} // namespace Gaffer

#endif // GAFFER_FILESYSTEMPATHPLUGPLUG_H
