//////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2011, John Haddon. All rights reserved.
//  Copyright (c) 2012, Image Engine Design Inc. All rights reserved.
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

#ifndef GAFFERBINDINGS_VALUEPLUGBINDING_H
#define GAFFERBINDINGS_VALUEPLUGBINDING_H

#include "GafferBindings/PlugBinding.h"

#include "Gaffer/ValuePlug.h"

namespace GafferBindings
{

/// Supports the following Context variables :
///
/// "valuePlugSerialiser:omitParentNodePlugValues"
///
/// :	Omits the `setValue()` calls for plugs of the parent
///     node. This is used when exporting the contents of a
///     Box node for referencing.
class GAFFERBINDINGS_API ValuePlugSerialiser : public PlugSerialiser
{

	public :

		std::string constructor( const Gaffer::GraphComponent *graphComponent, Serialisation &serialisation ) const override;
		std::string postHierarchy( const Gaffer::GraphComponent *graphComponent, const std::string &identifier, Serialisation &serialisation ) const override;

		static std::string repr( const Gaffer::ValuePlug *plug, const std::string &extraArguments = "", Serialisation *serialisation = nullptr );
		/// Returns a serialisation suitable for use in a `setValue()` or `setDefaultValue()` call.
		/// If `serialisation` is provided then `serialisation->addModule()` will be called appropriately.
		static std::string valueRepr( const boost::python::object &value, Serialisation *serialisation = nullptr );

};

} // namespace GafferBindings

#endif // GAFFERBINDINGS_VALUEPLUGBINDING_H
