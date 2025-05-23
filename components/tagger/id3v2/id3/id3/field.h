// -*- C++ -*-
// $Id$

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

#ifndef _ID3LIB_FIELD_H_
#define _ID3LIB_FIELD_H_

#include "strings.h"
#include "container.h"

class ID3_Reader;
class ID3_Writer;

class ID3_Field : public ID3_Container
{
	public:
		virtual void		 Clear() = 0;

		virtual size_t		 Size() const = 0;
		virtual size_t		 BinSize() const = 0;
		virtual size_t		 GetNumTextItems() const = 0;

		/* Integer field functions
		 */
		virtual ID3_Field	&operator= (uint32 val) = 0;
		virtual void		 Set(uint32) = 0;
		virtual uint32		 Get() const = 0;

		/* ASCII string field functions
		 */
		virtual ID3_Field	&operator= (const char* s) = 0;
		virtual size_t		 Set(const char*) = 0;
		virtual size_t		 Get(char*, size_t) const = 0;
		virtual size_t		 Get(char*, size_t, size_t) const = 0;
		virtual const char	*GetRawText() const = 0;
		virtual size_t		 Add(const char*) = 0;

		virtual dami::String	 GetText() const = 0;
		virtual size_t		 SetText(dami::String) = 0;

		/* Unicode string field functions
		 */
		virtual ID3_Field	&operator= (const unicode_t* s) = 0;
		virtual size_t		 Set(const unicode_t*) = 0;
		virtual size_t		 Get(unicode_t *buffer, size_t) const = 0;
		virtual size_t		 Get(unicode_t *buffer, size_t, size_t) const = 0;
		virtual const unicode_t	*GetRawUnicodeText() const = 0;
		virtual size_t		 Add(const unicode_t*) = 0;

		/* Binary field functions
		 */
		virtual size_t		 Set(const uchar *, size_t) = 0;
		virtual size_t		 Get(uchar *, size_t) const = 0;
		virtual const uchar	*GetRawBinary() const = 0;
		virtual void		 FromFile(const char *) = 0;
		virtual void		 ToFile(const char *sInfo) const = 0;
		virtual dami::BString	 GetBinary() const = 0;

		/* Miscelaneous functions
		 */
		virtual ID3_Field	&operator =(const ID3_Field &) = 0;
		virtual bool		 InScope(ID3_V2Spec spec) const = 0;
		virtual ID3_V2Spec	 MinSpec() const = 0;
		virtual ID3_V2Spec	 MaxSpec() const = 0;

		virtual ID3_FieldID	 GetID() const = 0;
		virtual ID3_FieldID	 GetLinkedField() const = 0;
		virtual ID3_FieldType	 GetType() const = 0;
		virtual bool		 SetEncoding(ID3_TextEnc enc) = 0;
		virtual bool		 SetLinkedSize(size_t newfixedsize) = 0;
		virtual bool		 HasFixedSize() = 0;
		virtual ID3_TextEnc	 GetEncoding() const = 0;
		virtual bool		 HasFlag(const flags_t flag) const = 0;
		virtual bool		 IsEncodable() const = 0;

		virtual ID3_Err		 Render(ID3_Writer &) const = 0;
		virtual bool		 Parse(ID3_Reader &) = 0;
		virtual bool		 HasChanged() const = 0;
	protected:
		/* To prevent public instantiation, the constructor is made protected
		 */
					 ID3_Field() { };
		virtual			~ID3_Field() { };
};

class ID3_FrameInfo
{
	public:
				 ID3_FrameInfo() {};
				~ID3_FrameInfo() {};

		char		*ShortName(ID3_FrameID frameid);
		char		*LongName(ID3_FrameID frameid);
		const char	*Description(ID3_FrameID frameid);

		int		 MaxFrameID();

		int		 NumFields(ID3_FrameID frameid);

		ID3_FieldID	 FieldID(ID3_FrameID frameid, int fieldnum);
		ID3_FieldType	 FieldType(ID3_FrameID frameid, int fieldnum);
		size_t		 FieldSize(ID3_FrameID frameid, int fieldnum);
		flags_t		 FieldFlags(ID3_FrameID frameid, int fieldnum);
};

#endif /* _ID3LIB_FIELD_H_ */
