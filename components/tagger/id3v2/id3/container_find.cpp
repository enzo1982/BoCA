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

#include "container_impl.h"

using namespace dami;

ID3_ContainerImpl::const_iterator ID3_ContainerImpl::Find(const ID3_Frame *frame) const
{
	const_iterator	 cur = _frames.begin();

	for (; cur != _frames.end(); ++cur)
	{
		if (*cur == frame) break;
	}

	return cur;
}

ID3_ContainerImpl::iterator ID3_ContainerImpl::Find(const ID3_Frame *frame)
{
	iterator	 cur = _frames.begin();

	for (; cur != _frames.end(); ++cur)
	{
		if (*cur == frame) break;
	}

	return cur;
}

ID3_Frame *ID3_ContainerImpl::Find(ID3_FrameID id) const
{
	ID3_Frame	*frame = NULL;

	/* Reset the cursor if it isn't set.
	 */
	if (_frames.end() == _cursor)
	{
		_cursor = _frames.begin();
	}

	for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
	{
		/* We want to cycle through the list to find the matching frame. We
		 * should begin from the cursor, search each successive frame, wrapping
		 * if necessary. The enclosing loop and the assignment statments below
		 * ensure that we first begin at the cursor and search to the end of the
		 * list and, if unsuccessful, start from the beginning of the list and
		 * search to the cursor.
		 */
		const_iterator	 begin  = (0 == iCount ? _cursor       : _frames.begin()),
				 end    = (0 == iCount ? _frames.end() : _cursor);

		/* Search from the cursor to the end.
		 */
		for (const_iterator cur = begin; cur != end; ++cur)
		{
			if ((*cur != NULL) && ((*cur)->GetID() == id))
			{
				/* We've found a valid frame. Set the cursor to be the next element.
				 */
				frame = *cur;
				_cursor = ++cur;

				break;
			}
		}
	}

	return frame;
}

ID3_Frame *ID3_ContainerImpl::Find(ID3_FrameID id, ID3_FieldID fldID, String data) const
{
	ID3_Frame	*frame = NULL;

	ID3D_NOTICE("Find: looking for comment with data = " << data.c_str());

	/* Reset the cursor if it isn't set.
	 */
	if (_frames.end() == _cursor)
	{
		_cursor = _frames.begin();

		ID3D_NOTICE("Find: resetting cursor");
	}

	for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
	{
		ID3D_NOTICE("Find: iCount = " << iCount);

		/* We want to cycle through the list to find the matching frame. We
		 * should begin from the cursor, search each successive frame, wrapping
		 * if necessary. The enclosing loop and the assignment statments below
		 * ensure that we first begin at the cursor and search to the end of the
		 * list and, if unsuccessful, start from the beginning of the list and
		 * search to the cursor.
		 */
		const_iterator	 begin  = (0 == iCount ? _cursor       : _frames.begin()),
				 end    = (0 == iCount ? _frames.end() : _cursor);

		/* Search from the cursor to the end.
		 */
		for (const_iterator cur = begin; cur != end; ++cur)
		{
			ID3D_NOTICE("Find: frame = 0x" << hex << (uint32) *cur << dec);

			if ((*cur != NULL) && ((*cur)->GetID() == id) &&
			    (*cur)->Contains(fldID))
			{
				ID3_Field	*fld = (*cur)->GetField(fldID);

				if (NULL == fld)
				{
					continue;

					ID3D_NOTICE("Find: didn't have the right field");
				}

				String	 text(fld->GetRawText(), fld->Size());

				ID3D_NOTICE("Find: text = " << text.c_str());

				if (text == data)
				{
					/* We've found a valid frame. Set cursor to be the next element.
					 */
					frame = *cur;
					_cursor = ++cur;

					break;
				}
			}
		}
	}

	return frame;
}

ID3_Frame *ID3_ContainerImpl::Find(ID3_FrameID id, ID3_FieldID fldID, WString data) const
{
	ID3_Frame	*frame = NULL;

	/* Reset the cursor if it isn't set.
	 */
	if (_frames.end() == _cursor)
	{
		_cursor = _frames.begin();
	}

	for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
	{
		/* We want to cycle through the list to find the matching frame. We
		 * should begin from the cursor, search each successive frame, wrapping
		 * if necessary. The enclosing loop and the assignment statments below
		 * ensure that we first begin at the cursor and search to the end of the
		 * list and, if unsuccessful, start from the beginning of the list and
		 * search to the cursor.
		 */
		const_iterator	 begin  = (0 == iCount ? _cursor       : _frames.begin()),
				 end    = (0 == iCount ? _frames.end() : _cursor);

		/* Search from the cursor to the end.
		 */
		for (const_iterator cur = begin; cur != end; ++cur)
		{
			if ((*cur != NULL) && ((*cur)->GetID() == id) &&
			    (*cur)->Contains(fldID))
			{
				ID3_Field	*fld = (*cur)->GetField(fldID);

				if (NULL == fld) continue;

				WString	 text = toWString(fld->GetRawUnicodeText(), fld->Size());

				if (text == data)
				{
					/* We've found a valid frame. Set cursor to be the next element.
					 */
					frame = *cur;
					_cursor = ++cur;

					break;
				}
			}
		}
	}

	return frame;
}

ID3_Frame *ID3_ContainerImpl::Find(ID3_FrameID id, ID3_FieldID fldID, uint32 data) const
{
	ID3_Frame	*frame = NULL;

	/* Reset the cursor if it isn't set.
	 */
	if (_frames.end() == _cursor)
	{
		_cursor = _frames.begin();
	}

	for (int iCount = 0; iCount < 2 && frame == NULL; iCount++)
	{
		/* We want to cycle through the list to find the matching frame. We
		 * should begin from the cursor, search each successive frame, wrapping
		 * if necessary. The enclosing loop and the assignment statments below
		 * ensure that we first begin at the cursor and search to the end of the
		 * list and, if unsuccessful, start from the beginning of the list and
		 * search to the cursor.
		 */
		const_iterator	 begin  = (0 == iCount ? _cursor       : _frames.begin()),
				 end    = (0 == iCount ? _frames.end() : _cursor);

		/* Search from the cursor to the end.
		 */
		for (const_iterator cur = begin; cur != end; ++cur)
		{
			if ((*cur != NULL) && ((*cur)->GetID() == id) &&
			   ((*cur)->GetField(fldID)->Get() == data))
			{
				/* We've found a valid frame. Set the cursor to be the next element.
				 */
				frame = *cur;
				_cursor = ++cur;

				break;
			}
		}
	}

	return frame;
}
