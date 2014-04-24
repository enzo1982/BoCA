// $Id$

// id3lib: a C++ library for creating and manipulating id3v1/v2 tags
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

#if defined HAVE_SYS_PARAM_H
#	include <sys/param.h>
#endif

#include "tag_impl.h"
#include <id3/io_strings.h>

using namespace dami;

size_t ID3_TagImpl::IsV2Tag(ID3_Reader &reader)
{
	io::ExitTrigger	 et(reader);
	size_t		 tagSize = 0;
	String		 id	 = io::readText(reader, ID3_TagHeader::ID_SIZE);
	String		 ver	 = io::readText(reader, 2);

	/* Read flags byte.
	 */
	reader.readChar();

	String		 size	 = io::readText(reader, 4);

	if	(id == ID3_TagHeader::ID &&
		 (uchar) ver [0] < 0xFF  && (uchar) ver [1] < 0xFF &&
		 (uchar) size[0] < 0x80  && (uchar) size[1] < 0x80 &&
		 (uchar) size[2] < 0x80  && (uchar) size[3] < 0x80)
	{
		io::StringReader sr(size);
		tagSize = io::readUInt28(sr) + ID3_TagHeader::SIZE;
	}
	else if (id != ID3_TagHeader::ID) ID3D_NOTICE( "*** IsV2Tag: Not an id3v2 tag header" );
	else if ((uchar) ver[0]  >= 0xFF) ID3D_NOTICE( "*** IsV2Tag: Major offset" );
	else if ((uchar) ver[1]  >= 0xFF) ID3D_NOTICE( "*** ISV2Tag: Minor offset" );
	else if ((uchar) size[0] >= 0x80) ID3D_NOTICE( "*** ISV2Tag: 1st size offset" );
	else if ((uchar) size[1] >= 0x80) ID3D_NOTICE( "*** ISV2Tag: 2nd size offset" );
	else if ((uchar) size[2] >= 0x80) ID3D_NOTICE( "*** ISV2Tag: 3rd size offset" );
	else if ((uchar) size[3] >= 0x80) ID3D_NOTICE( "*** ISV2Tag: 4th size offset" );
	else				  ID3D_NOTICE( "*** shouldn't get here!" );

	return tagSize;
}

ID3_TagImpl::ID3_TagImpl(const char *name, flags_t flags) : _file_name(),
							    _file_size(0),
							    _prepended_bytes(0),
							    _appended_bytes(0),
							    _is_file_writable(false),
							    _mp3_info(NULL) // need to do this before this->Clear()
{
	this->Clear();

	if (name)
	{
		this->Link(name, flags);
	}
}

ID3_TagImpl::ID3_TagImpl(const ID3_Tag &tag) : _file_name(),
					       _file_size(0),
					       _prepended_bytes(0),
					       _appended_bytes(0),
					       _is_file_writable(false),
					       _mp3_info(NULL) // need to do this before this->Clear()
{
	*this = tag;
}

ID3_TagImpl::~ID3_TagImpl()
{
	this->Clear();
}

void ID3_TagImpl::Clear()
{
	ID3_ContainerImpl::Clear();

	_is_padded = true;

	_hdr.Clear();
	_hdr.SetSpec(ID3V2_DEFAULT);

	_tags_to_parse.clear();

	if (_mp3_info)
	{
		delete _mp3_info; // Also deletes _mp3_header
		_mp3_info = NULL;
	}

	_file_name = "";
	_last_error = ID3E_NoError;
	_changed = true;
}

ID3_Err ID3_TagImpl::GetLastError()
{
	ID3_Err	 err = _last_error;

	_last_error = ID3E_NoError;

	return err;
}

bool ID3_TagImpl::SetSpec(ID3_V2Spec spec)
{
	bool	 changed = _hdr.SetSpec(spec);

	ID3_ContainerImpl::SetSpec(spec);

	_changed = _changed || changed;

	return changed;
}

ID3_V2Spec ID3_TagImpl::GetSpec() const
{
	return _hdr.GetSpec();
}

bool ID3_TagImpl::SetUnsync(bool b)
{
	bool	 changed = _hdr.SetUnsync(b);

	_changed = changed || _changed;

	return changed;
}

bool ID3_TagImpl::SetExtended(bool ext)
{
	bool	 changed = _hdr.SetExtended(ext);

	_changed = changed || _changed;

	return changed;
}

bool ID3_TagImpl::SetExperimental(bool exp)
{
	bool	 changed = _hdr.SetExperimental(exp);

	_changed = changed || _changed;

	return changed;
}

bool ID3_TagImpl::GetUnsync() const
{
	return _hdr.GetUnsync();
}

bool ID3_TagImpl::GetExtended() const
{
	return _hdr.GetExtended();
}

bool ID3_TagImpl::GetExperimental() const
{
	return _hdr.GetExperimental();
}

bool ID3_TagImpl::GetFooter() const
{
	return _hdr.GetFooter();
}

size_t ID3_TagImpl::GetExtendedBytes() const
{
	/* Returns the number of bytes this lib will write, is only called by id3::v2::render.
	 */
	if (this->GetExtended())
	{
		if	(this->GetSpec() == ID3V2_4_0) return  6; // minimal ID3v2.4 ext header size
		else if (this->GetSpec() == ID3V2_3_0) return 10; // minimal ID3v2.3 ext header size
		else				       return  0; // not implemented
	}

	return 0;
}

bool ID3_TagImpl::SetPadding(bool pad)
{
	bool	 changed = (_is_padded != pad);

	_changed = changed || _changed;

	if (changed) _is_padded = pad;

	return changed;
}

ID3_TagImpl &ID3_TagImpl::operator=(const ID3_Tag &rTag)
{
	this->Clear();

	this->SetUnsync(rTag.GetUnsync());
	this->SetExtended(rTag.GetExtendedHeader());
	this->SetExperimental(rTag.GetExperimental());

	ID3_Tag::ConstIterator	*iter = rTag.CreateIterator();
	const ID3_Frame		*frame = NULL;

	while (NULL != (frame = iter->GetNext()))
	{
		this->AttachFrame(new ID3_Frame(*frame));
	}

	delete iter;

	return *this;
}

size_t ID3_GetDataSize(const ID3_TagImpl &tag)
{
	return tag.GetFileSize() - tag.GetPrependedBytes() - tag.GetAppendedBytes();
}
