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

#ifndef _ID3LIB_CONTAINER_IMPL_H_
#define _ID3LIB_CONTAINER_IMPL_H_

#include <id3/container.h>
#include <id3/frame.h>
#include <id3/field.h>
#include <id3/strings.h>

#include <list>

class ID3_Reader;
class ID3_Writer;

namespace dami
{
	namespace id3
	{
		namespace v2
		{
			bool	 parseFrames(ID3_ContainerImpl& tag, ID3_Reader& rdr);
			ID3_Err	 renderFrames(ID3_Writer& writer, const ID3_ContainerImpl& container);
		};
	};
};

class ID3_ContainerImpl
{
  	typedef std::list<ID3_Frame *> Frames;

	public:
		typedef Frames::iterator	 iterator;
		typedef Frames::const_iterator	 const_iterator;

	public:
					 ID3_ContainerImpl();
					 ID3_ContainerImpl(const ID3_Container &container);
		virtual			~ID3_ContainerImpl();

		virtual void		 Clear();
		virtual size_t		 Size() const;

		bool			 HasChanged() const;
		void			 SetChanged(bool b) { _changed = b; }

		void			 AddFrame(const ID3_Frame &);
		void			 AddFrame(const ID3_Frame *);
		bool			 AttachFrame(ID3_Frame *);
		bool			 IsValidFrame(ID3_Frame &, bool);
		void			 checkFrames();
		ID3_Frame		*RemoveFrame(const ID3_Frame *);

		ID3_Frame		*Find(ID3_FrameID id) const;
		ID3_Frame		*Find(ID3_FrameID id, ID3_FieldID fld, uint32 data) const;
		ID3_Frame		*Find(ID3_FrameID id, ID3_FieldID fld, dami::String) const;
		ID3_Frame		*Find(ID3_FrameID id, ID3_FieldID fld, dami::WString) const;

		size_t			 NumFrames() const { return _frames.size(); }
		ID3_ContainerImpl	&operator =(const ID3_Container &);

		virtual ID3_V2Spec	 GetSpec() const;
		virtual bool		 SetSpec(ID3_V2Spec);
		ID3_V2Spec		 MinSpec() const;

		iterator		 begin()       { return _frames.begin(); }
		iterator		 end()         { return _frames.end(); }
		const_iterator		 begin() const { return _frames.begin(); }
		const_iterator		 end()   const { return _frames.end(); }

		/* Deprecated! */
		bool			 UserUpdatedSpec;	// used to determine whether user used SetSpec();

	protected:
		const_iterator		 Find(const ID3_Frame *) const;
		iterator		 Find(const ID3_Frame *);

		void			 ParseFile();
		void			 ParseReader(ID3_Reader &reader);

	private:
		ID3_V2Spec		 _spec;			// which version of the spec
		Frames			 _frames;

		mutable const_iterator	 _cursor;		// which frame in list are we at
		mutable bool		 _changed;		// have frames changed since last parse or render?
};

#endif /* _ID3LIB_CONTAINER_IMPL_H_ */
