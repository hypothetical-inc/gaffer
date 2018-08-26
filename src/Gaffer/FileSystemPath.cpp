//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2011-2012, John Haddon. All rights reserved.
//  Copyright (c) 2012-2015, Image Engine Design Inc. All rights reserved.
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

#include "Gaffer/FileSystemPath.h"

#include "Gaffer/CompoundPathFilter.h"
#include "Gaffer/FileSequencePathFilter.h"
#include "Gaffer/MatchPatternPathFilter.h"
#include "Gaffer/PathFilter.h"

#include "IECore/DateTimeData.h"
#include "IECore/FileSequenceFunctions.h"
#include "IECore/SimpleTypedData.h"

#include "boost/algorithm/string.hpp"
#include "boost/date_time/posix_time/conversion.hpp"
#include "boost/filesystem.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/regex.hpp"

#ifndef _MSC_VER
	#include <grp.h>
	#include <pwd.h>
#else
#include <stdio.h>
#include <Windows.h>
#include <tchar.h>
#include "accctrl.h"
#include "aclapi.h"
#endif

#include <sys/stat.h>

using namespace std;
using namespace boost::filesystem;
using namespace boost::algorithm;
using namespace boost::posix_time;
using namespace IECore;
using namespace Gaffer;

IE_CORE_DEFINERUNTIMETYPED( FileSystemPath );

static InternedString g_ownerPropertyName( "fileSystem:owner" );
static InternedString g_groupPropertyName( "fileSystem:group" );
static InternedString g_modificationTimePropertyName( "fileSystem:modificationTime" );
static InternedString g_sizePropertyName( "fileSystem:size" );
static InternedString g_frameRangePropertyName( "fileSystem:frameRange" );

static InternedString g_windowsSeparator( "\\" );
static InternedString g_genericSeparator( "/" );
static InternedString g_uncPrefix( "\\\\" );

FileSystemPath::FileSystemPath( PathFilterPtr filter, bool includeSequences )
	:	Path( filter ), m_includeSequences( includeSequences )
{
}

FileSystemPath::FileSystemPath( const std::string &path, PathFilterPtr filter, bool includeSequences )
	:	Path( path, filter ), m_includeSequences( includeSequences )
{
	setFromString( path );
}

FileSystemPath::FileSystemPath( const Names &names, const IECore::InternedString &root, PathFilterPtr filter, bool includeSequences )
	:	Path( names, root, filter ), m_includeSequences( includeSequences )
{
}

FileSystemPath::~FileSystemPath()
{
}

// Create a path that is aware of differences in OS naming schemes
// Windows separates path elements with a backlash and Linux / Mac with forward slash
// Windows also starts drive letter paths with not leading \, which looks like a relative
// path but should be treated as though the drive letter is the root of the path
// If we don't treat the drive letter as the root, there will be errors when repeatedly
// popping the last path entry and getting to the drive letter path at the start of what
// FileSystemPath thinks is a relative path

void FileSystemPath::setFromString(const std::string &string)
{
	Names newNames;
	std::string sanitizedString;
	boost::regex driveLetterPattern{ "^([A-Za-z]{1}:)" };
	boost::smatch result;

	sanitizedString = string.c_str();
	if (boost::starts_with(sanitizedString, g_uncPrefix.c_str()))
	{
		boost::replace_first(sanitizedString, g_uncPrefix.c_str(), g_genericSeparator.c_str());
	}
	boost::replace_all( sanitizedString, g_windowsSeparator.c_str(), g_genericSeparator.c_str() );

	StringAlgo::tokenize<InternedString>(sanitizedString, '/', back_inserter(newNames));

	InternedString newRoot;
	if (string.size() && string[0] == '/')
	{
		newRoot = "/";
	}
	else if ( string.size() && boost::regex_search( newNames[0].string(), result, driveLetterPattern ) )
	{
		newRoot = newNames[0];
		newNames.erase( newNames.begin() );
	}

	if (newRoot == this->root() && newNames == this->names() )
	{
		return;
	}

	set( 0, this->names().size(), newNames );
	setRoot( newRoot );

	emitPathChanged();
}

bool FileSystemPath::isValid() const
{
	if( !Path::isValid() )
	{
		return false;
	}

	if( m_includeSequences && isFileSequence() )
	{
		return true;
	}

#ifndef _MSC_VER
	const file_type t = symlink_status( path( this->string() ) ).type();
#else
	const file_type t = status( path( this->string() ) ).type();
#endif
	return t != status_error && t != file_not_found;
}

bool FileSystemPath::isLeaf() const
{
	return isValid() && !is_directory( path( this->string() ) );
}

bool FileSystemPath::getIncludeSequences() const
{
	return m_includeSequences;
}

void FileSystemPath::setIncludeSequences( bool includeSequences )
{
	m_includeSequences = includeSequences;
}

bool FileSystemPath::isFileSequence() const
{
	if( !m_includeSequences || is_directory( path( this->string() ) ) )
	{
		return false;
	}

	try
	{
		return boost::regex_match( this->string(), FileSequence::fileNameValidator() );
	}
	catch( ... )
	{
	}

	return false;
}

FileSequencePtr FileSystemPath::fileSequence() const
{
	if( !m_includeSequences || is_directory( path( this->string() ) ) )
	{
		return nullptr;
	}

	FileSequencePtr sequence = nullptr;
	IECore::ls( this->nativeString(), sequence, /* minSequenceSize = */ 1 );
	return sequence;
}

void FileSystemPath::propertyNames( std::vector<IECore::InternedString> &names ) const
{
	Path::propertyNames( names );

	names.push_back( g_ownerPropertyName );
	names.push_back( g_groupPropertyName );
	names.push_back( g_modificationTimePropertyName );
	names.push_back( g_sizePropertyName );

	if( m_includeSequences )
	{
		names.push_back( g_frameRangePropertyName );
	}
}

IECore::ConstRunTimeTypedPtr FileSystemPath::property( const IECore::InternedString &name ) const
{
	if( name == g_ownerPropertyName )
	{
		if( m_includeSequences )
		{
			FileSequencePtr sequence = fileSequence();
			if( sequence )
			{
				std::vector<std::string> files;
				sequence->fileNames( files );

				size_t maxCount = 0;
				std::string mostCommon = "";
				std::map<std::string,size_t> ownerCounter;
				for( std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it )
				{
					std::string value = getOwner( it->c_str() );
					std::pair<std::map<std::string,size_t>::iterator,bool> oIt = ownerCounter.insert( std::pair<std::string,size_t>( value, 0 ) );
					oIt.first->second++;
					if( oIt.first->second > maxCount )
					{
						mostCommon = value;
					}
				}

				return new StringData( mostCommon );
			}
		}

		std::string n = this->string();
		struct stat s;
		stat( n.c_str(), &s );
		#ifndef _MSC_VER
		struct passwd *pw = getpwuid( s.st_uid );
		return new StringData( pw ? pw->pw_name : "" );
		#else
		return new StringData( "" );
		#endif
	}
	else if( name == g_groupPropertyName )
	{
		if( m_includeSequences )
		{
			FileSequencePtr sequence = fileSequence();
			if( sequence )
			{
				std::vector<std::string> files;
				sequence->fileNames( files );

				size_t maxCount = 0;
				std::string mostCommon = "";
				std::map<std::string,size_t> ownerCounter;
				for( std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it )
				{
					struct stat s;
					stat( it->c_str(), &s );
					#ifndef _MSC_VER
					struct group *gr = getgrgid( s.st_gid );
					std::string value = gr ? gr->gr_name : "";
					#else
					std::string value = "";
					#endif
					std::pair<std::map<std::string,size_t>::iterator,bool> oIt = ownerCounter.insert( std::pair<std::string,size_t>( value, 0 ) );
					oIt.first->second++;
					if( oIt.first->second > maxCount )
					{
						mostCommon = value;
					}
				}

				return new StringData( mostCommon );
			}
		}

		std::string n = this->string();
		struct stat s;
		stat( n.c_str(), &s );
		#ifndef _MSC_VER
		struct group *gr = getgrgid( s.st_gid );
		return new StringData( gr ? gr->gr_name : "" );
		#else
		return new StringData( "" );
		#endif
	}
	else if( name == g_modificationTimePropertyName )
	{
		boost::system::error_code e;

		if( m_includeSequences )
		{
			FileSequencePtr sequence = fileSequence();
			if( sequence )
			{
				std::vector<std::string> files;
				sequence->fileNames( files );

				std::time_t newest = 0;
				for( std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it )
				{
					std::time_t t = last_write_time( path( *it ), e );
					if( t > newest )
					{
						newest = t;
					}
				}

				return new DateTimeData( from_time_t( newest ) );
			}
		}

		std::time_t t = last_write_time( path( this->string() ), e );
		return new DateTimeData( from_time_t( t ) );
	}
	else if( name == g_sizePropertyName )
	{
		boost::system::error_code e;

		if( m_includeSequences )
		{
			FileSequencePtr sequence = fileSequence();
			if( sequence )
			{
				std::vector<std::string> files;
				sequence->fileNames( files );

				uintmax_t total = 0;
				for( std::vector<std::string>::iterator it = files.begin(); it != files.end(); ++it )
				{
					uintmax_t s = file_size( path( *it ), e );
					if( !e )
					{
						total += s;
					}
				}

				return new UInt64Data( total );
			}
		}

		uintmax_t s = file_size( path( this->string() ), e );
		return new UInt64Data( !e ? s : 0 );
	}
	else if( name == g_frameRangePropertyName )
	{
		FileSequencePtr sequence = fileSequence();
		if( sequence )
		{
			return new StringData( sequence->getFrameList()->asString() );
		}

		return new StringData;
	}

	return Path::property( name );
}

PathPtr FileSystemPath::copy() const
{
	return new FileSystemPath( names(), root(), const_cast<PathFilter *>( getFilter() ), m_includeSequences );
}

void FileSystemPath::doChildren( std::vector<PathPtr> &children ) const
{
	path p( this->string() );

	if( !is_directory( p ) )
	{
		return;
	}

	for( directory_iterator it( p ), eIt; it != eIt; ++it )
	{
		children.push_back( new FileSystemPath( it->path().string(), const_cast<PathFilter *>( getFilter() ), m_includeSequences ) );
	}

	if( m_includeSequences )
	{
		std::vector<FileSequencePtr> sequences;
		IECore::ls( p.string(), sequences, /* minSequenceSize */ 1 );
		for( std::vector<FileSequencePtr>::iterator it = sequences.begin(); it != sequences.end(); ++it )
		{
			std::vector<FrameList::Frame> frames;
			(*it)->getFrameList()->asList( frames );
			if ( !is_directory( path( (*it)->fileNameForFrame( frames[0] ) ) ) )
			{
				children.push_back( new FileSystemPath( path( p / (*it)->getFileName() ).string(), const_cast<PathFilter *>( getFilter() ), m_includeSequences ) );
			}
		}
	}
}

PathFilterPtr FileSystemPath::createStandardFilter( const std::vector<std::string> &extensions, const std::string &extensionsLabel, bool includeSequenceFilter )
{
	CompoundPathFilterPtr result = new CompoundPathFilter();

	// Filter for the extensions

	if( extensions.size() )
	{
		std::string defaultLabel = "Show only ";
		vector<StringAlgo::MatchPattern> patterns;
		for( std::vector<std::string>::const_iterator it = extensions.begin(), eIt = extensions.end(); it != eIt; ++it )
		{
			patterns.push_back( "*." + to_lower_copy( *it ) );
			patterns.push_back( "*." + to_upper_copy( *it ) );
			// the form below is for file sequences, where the frame
			// range will come after the extension.
			patterns.push_back( "*." + to_lower_copy( *it ) + " *" );
			patterns.push_back( "*." + to_upper_copy( *it ) + " *" );

			if( it != extensions.begin() )
			{
				defaultLabel += ", ";
			}
			defaultLabel += "." + to_lower_copy( *it );
		}
		defaultLabel += " files";

		MatchPatternPathFilterPtr fileNameFilter = new MatchPatternPathFilter( patterns, "name" );
		CompoundDataPtr uiUserData = new CompoundData;
		uiUserData->writable()["label"] = new StringData( extensionsLabel.size() ? extensionsLabel : defaultLabel );
		fileNameFilter->userData()->writable()["UI"] = uiUserData;

		result->addFilter( fileNameFilter );
	}

	// Filter for sequences
	if( includeSequenceFilter )
	{
		result->addFilter( new FileSequencePathFilter( /* mode = */ FileSequencePathFilter::Concise ) );
	}

	// Filter for hidden files

	std::vector<std::string> hiddenFilePatterns; hiddenFilePatterns.push_back( ".*" );
	MatchPatternPathFilterPtr hiddenFilesFilter = new MatchPatternPathFilter( hiddenFilePatterns, "name", /* leafOnly = */ false );
	hiddenFilesFilter->setInverted( true );

	CompoundDataPtr hiddenFilesUIUserData = new CompoundData;
	hiddenFilesUIUserData->writable()["label"] = new StringData( "Show hidden files" );
	hiddenFilesUIUserData->writable()["invertEnabled"] = new BoolData( true );
	hiddenFilesFilter->userData()->writable()["UI"] = hiddenFilesUIUserData;

	result->addFilter( hiddenFilesFilter );

	// User defined search filter

	std::vector<std::string> searchPatterns; searchPatterns.push_back( "" );
	MatchPatternPathFilterPtr searchFilter = new MatchPatternPathFilter( searchPatterns );
	searchFilter->setEnabled( false );

	CompoundDataPtr searchUIUserData = new CompoundData;
	searchUIUserData->writable()["editable"] = new BoolData( true );
	searchFilter->userData()->writable()["UI"] = searchUIUserData;

	result->addFilter( searchFilter );

	return result;
}

std::string FileSystemPath::string() const
{
	boost::regex driveLetterPattern{ "^([A-Za-z]{1}:)" };
	boost::smatch regex_result;

	std::string result = this->root();
	if (boost::regex_match(result, regex_result, driveLetterPattern))
	{
		result += "/";
	}
	for (size_t i = 0, s = this->names().size(); i < s; ++i)
	{
		if (i != 0)
		{
			result += "/";
		}
		result += this->names()[i].string();
	}
	return result;
}

std::string FileSystemPath::nativeString() const
{
	#ifdef _MSC_VER
		std::string separator = g_windowsSeparator;
	#else
		std::string separator = g_genericSeparator;
	#endif
	
	boost::regex driveLetterPattern{ "^([A-Za-z]{1}:)" };
	boost::smatch regex_result;

	std::string result = this->root();
	if( boost::regex_match( result, regex_result, driveLetterPattern ) )
	{
		result += separator;
	}
	Path::Names names = this->names();
	for( size_t i = 0, s = names.size(); i < s; ++i )
	{
		if( i != 0 )
		{
			result += separator;
		}
		result += names[i].string();
	}
	return result;
}

// Windows code to get file / directory owner from https://docs.microsoft.com/en-us/windows/desktop/SecAuthZ/finding-the-owner-of-a-file-object-in-c--
std::string FileSystemPath::getOwner( const std::string &pathString ) const
{
	std::string value;
#ifndef _MSC_VER
	struct stat s;
	stat(it->c_str(), &s);
	struct passwd *pw = getpwuid(s.st_uid);
	value = pw ? pw->pw_name : "";
#else
	DWORD dwRtnCode = 0;
	PSID pSidOwner = NULL;
	BOOL bRtnBool = TRUE;
	LPTSTR AcctName = NULL;
	LPTSTR DomainName = NULL;
	DWORD dwAcctName = 1, dwDomainName = 1;
	SID_NAME_USE eUse = SidTypeUnknown;
	HANDLE hFile;
	PSECURITY_DESCRIPTOR pSD = NULL;


	// Get the handle of the file object.
	hFile = CreateFile( pathString.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	// Check GetLastError for CreateFile error code.
	if (hFile == INVALID_HANDLE_VALUE) {
		return "";
	}

	// Get the owner SID of the file.
	dwRtnCode = GetSecurityInfo(hFile, SE_FILE_OBJECT, OWNER_SECURITY_INFORMATION, &pSidOwner, NULL, NULL, NULL, &pSD);

	CloseHandle( hFile );

	// Check GetLastError for GetSecurityInfo error condition.
	if (dwRtnCode != ERROR_SUCCESS) {
		return "";
	}

	// First call to LookupAccountSid to get the buffer sizes.
	bRtnBool = LookupAccountSid(NULL, pSidOwner, AcctName, (LPDWORD)&dwAcctName, DomainName, (LPDWORD)&dwDomainName, &eUse);

	// Reallocate memory for the buffers.
	AcctName = (LPTSTR)GlobalAlloc(GMEM_FIXED, dwAcctName);

	// Check GetLastError for GlobalAlloc error condition.
	if (AcctName == NULL) {
		return "";
	}

	DomainName = (LPTSTR)GlobalAlloc(GMEM_FIXED, dwDomainName);

	// Check GetLastError for GlobalAlloc error condition.
	if (DomainName == NULL) {
		return "";
	}

	// Second call to LookupAccountSid to get the account name.
	bRtnBool = LookupAccountSid(NULL, pSidOwner, AcctName, (LPDWORD)&dwAcctName, DomainName, (LPDWORD)&dwDomainName, &eUse);

	if (bRtnBool == FALSE) {
		return "";
	}
	else if (bRtnBool == TRUE)
	{
		value = AcctName;	//let's ignore unicode considerations for now
	}
		
#endif
	return value;
}
