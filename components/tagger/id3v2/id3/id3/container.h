// -*- C++ -*-
// $Id$

// id3lib: a software library for creating and manipulating id3v1/v2 tags
// Copyright 1999, 2000  Scott Thomas Haug
// Copyright 2002 Thijmen Klok (thijmen@id3lib.org)

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

#ifndef _ID3LIB_CONTAINER_H_
#define _ID3LIB_CONTAINER_H_

#include "frame.h"
#include "field.h"

class ID3_Reader;
class ID3_Writer;
class ID3_ContainerImpl;
class ID3_Container;

class ID3_Container
{
	public:
		class Iterator
		{
			public:
				virtual 		~Iterator() {};

				virtual ID3_Frame	*GetNext() = 0;
		};

		class ConstIterator
		{
			public:
				virtual			~ConstIterator() {};

				virtual const ID3_Frame	*GetNext() = 0;
		};

	protected:
		ID3_ContainerImpl	*_impl;
		bool			 _own_impl;

	public:
					 ID3_Container();
					 ID3_Container(ID3_ContainerImpl *impl);
					 ID3_Container(const ID3_Container &container);

		virtual			~ID3_Container();

		void			 Clear();
		bool			 HasChanged() const;
		size_t			 Size() const;

		void			 AddFrame(const ID3_Frame &);
		void			 AddFrame(const ID3_Frame *);
		bool			 AttachFrame(ID3_Frame *);
		ID3_Frame		*RemoveFrame(const ID3_Frame *);

		ID3_Frame		*Find(ID3_FrameID) const;
		ID3_Frame		*Find(ID3_FrameID, ID3_FieldID, uint32) const;
		ID3_Frame		*Find(ID3_FrameID, ID3_FieldID, const char *) const;
		ID3_Frame		*Find(ID3_FrameID, ID3_FieldID, const unicode_t *) const;

		size_t			 NumFrames() const;

		Iterator		*CreateIterator();
		ConstIterator		*CreateIterator() const;

		ID3_Container		&operator =(const ID3_Container &);

		ID3_V2Spec		 GetSpec() const;
		bool			 SetSpec(ID3_V2Spec);
};

#endif /* _ID3LIB_CONTAINER_H_ */
