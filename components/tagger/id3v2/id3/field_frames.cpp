// $Id$

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug

/* This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
 * License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; if not, write to the Free Software Foundation,
 * Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/* The id3lib authors encourage improvements and optimisations to be sent to
 * the id3lib coordinator.  Please see the README file for details on where to
 * send such submissions.  See the AUTHORS file for a list of people who have
 * contributed to id3lib.  See the ChangeLog file for a list of changes to
 * id3lib.  These files are distributed with id3lib at
 * http://download.sourceforge.net/id3lib/
 */

#include "field_impl.h"
#include "container_impl.h"

#include <id3/utils.h>
#include <id3/io_helpers.h>

using namespace dami;

bool ID3_FieldImpl::ParseFrames(ID3_Reader& reader)
{
	ID3D_NOTICE( "ID3_FieldImpl::ParseFrames(): beg = " << reader.getBeg() );
	ID3D_NOTICE( "ID3_FieldImpl::ParseFrames(): cur = " << reader.getCur() );
	ID3D_NOTICE( "ID3_FieldImpl::ParseFrames(): end = " << reader.getEnd() );

	bool success = false;

	if (!reader.atEnd())
	{
		this->Clear();

		id3::v2::parseFrames(*this->_impl, reader);

		_changed = false;
		success = true;
	}

	return success;
}

void ID3_FieldImpl::RenderFrames(ID3_Writer& writer) const
{
	id3::v2::renderFrames(writer, *this->_impl);
}
