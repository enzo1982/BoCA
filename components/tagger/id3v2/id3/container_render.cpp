// $Id$

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug
// Copyright 2002  Thijmen Klok (thijmen@id3lib.org)

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

#include "container_impl.h"

using namespace dami;

ID3_Err id3::v2::renderFrames(ID3_Writer &writer, const ID3_ContainerImpl &container)
{
	for (ID3_ContainerImpl::const_iterator iter = container.begin(); iter != container.end(); ++iter)
	{
		const ID3_Frame	*frame = *iter;

		if (frame)
		{
			ID3_Err	 err = frame->Render(writer);

			if (err != ID3E_NoError) return err;
		}
	}

	return ID3E_NoError;
}

ID3_V2Spec ID3_ContainerImpl::MinSpec() const
{
	ID3_V2Spec	 minSpec = ID3V2_EARLIEST;

	for (const_iterator cur = _frames.begin(); cur != _frames.end(); ++cur)
	{
		const ID3_Frame	*frame = *cur;

		if (!frame) continue;

		ID3_V2Spec	 frameSpec = frame->MinSpec();

		if (minSpec < frameSpec) minSpec = frameSpec;
	}

	return minSpec;
}

size_t ID3_ContainerImpl::Size() const
{
	if (this->NumFrames() == 0) return 0;

	ID3_V2Spec	 spec = this->MinSpec();

	if (this->GetSpec() > spec) spec = this->GetSpec();

	size_t	 frameBytes = 0;

	for (const_iterator cur = _frames.begin(); cur != _frames.end(); ++cur)
	{
		if (*cur)
		{
			(*cur)->SetSpec(spec);

			frameBytes += (*cur)->Size();
		}
	}

	return frameBytes;
}
