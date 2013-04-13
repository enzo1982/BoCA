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
#include "frame_def.h"

ID3_FrameDef	*ID3_FindFrameDef(ID3_FrameID id);

// compares case insensitive null terminated or n sized chars
int ID3_strncasecmp(const char *s1, const char *s2, int n)
{
	// this routine is borrowed from half-life
	int c1, c2;

	while (true)
	{
		c1 = *s1++;
		c2 = *s2++;

		if (!n--) return 0; // strings are equal until end point n

		if (c1 != c2)
		{
			if (c1 == 0) return -2;   // s1 is earlier null terminated
			if (c2 == 0) return -3;   // s2 is earlier null terminated

			if (c1 >= 'a' && c1 <= 'z') c1 -= ('a' - 'A');  //capitalize
			if (c2 >= 'a' && c2 <= 'z') c2 -= ('a' - 'A');

			if (c1 != c2) return -1; // strings not equal
		}
		else if (c1 == 0 && c2 == 0)
		{
			return 0; // strings are equal
		}
	}

	return -1; //should not reach here
}

using namespace dami;

bool IsUrl(String tmpUrl)//char* url)
{
	// a url should start with http:// and be at least 11 chars (http://a.bb) OR
	// should start with ftp:// and be at least 10 chars (ftp://a.bb) OR
	// should start with mailto: and be at least 13 chars (mailto:a@b.cc)
	// these sizes do not take into consideration those that have emails or links directly at TLD's (e.g. a@b or http://a/ )
	if ((tmpUrl.size() > 11 && ID3_strncasecmp(tmpUrl.c_str(), "http://", 7) == 0) ||
	    (tmpUrl.size() > 10 && ID3_strncasecmp(tmpUrl.c_str(), "ftp://", 6)  == 0) ||
	    (tmpUrl.size() > 13 && ID3_strncasecmp(tmpUrl.c_str(), "mailto:", 7) == 0)) return true;

	return false;
}

bool ValidFrameOwner(String owner)
{
	if (IsUrl(owner)) return true;
	else		  return false;
}

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
	else if (id != ID3_TagHeader::ID) ; // clog << "*** IsV2Tag: Not an id3v2 tag header" << endl;
	else if ((uchar) ver[0]  >= 0xFF) ; // clog << "*** IsV2Tag: Major offset" << endl;
	else if ((uchar) ver[1]  >= 0xFF) ; // clog << "*** ISV2Tag: Minor offset" << endl;
	else if ((uchar) size[0] >= 0x80) ; // clog << "*** ISV2Tag: 1st size offset" << endl;
	else if ((uchar) size[1] >= 0x80) ; // clog << "*** ISV2Tag: 2nd size offset" << endl;
	else if ((uchar) size[2] >= 0x80) ; // clog << "*** ISV2Tag: 3rd size offset" << endl;
	else if ((uchar) size[3] >= 0x80) ; // clog << "*** ISV2Tag: 4th size offset" << endl;
	else				  ; // clog << "*** shouldn't get here!" << endl;

	return tagSize;
}

ID3_TagImpl::ID3_TagImpl(const char *name, flags_t flags)
 :	_frames(),
	_cursor(_frames.begin()),
	_file_name(),
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

ID3_TagImpl::ID3_TagImpl(const ID3_Tag &tag)
 :	_frames(),
	_cursor(_frames.begin()),
	_file_name(),
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
	for (iterator cur = _frames.begin(); cur != _frames.end(); ++cur)
	{
		if (*cur)
		{
			delete *cur;
			*cur = NULL;
		}
	}

	UserUpdatedSpec = false;

	_frames.clear();
	_cursor = _frames.begin();
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

void ID3_TagImpl::AddFrame(const ID3_Frame &frame)
{
	this->AddFrame(&frame);
}

void ID3_TagImpl::AddFrame(const ID3_Frame *frame)
{
	if (frame)
	{
		ID3_Frame	*frm = new ID3_Frame(*frame);

		this->AttachFrame(frm);
	}
}

bool ID3_TagImpl::IsValidFrame(ID3_Frame& frame, bool testlinkedFrames)
{
	ID3_Frame	*testframe = &frame;
	ID3_Field	*tmpField;
	ID3_Frame	*tmpFrame;

	if (NULL == testframe) return false;

	// check if the frame is outdated
	ID3_FrameDef	*myFrameDef = ID3_FindFrameDef(testframe->GetID());

	if (myFrameDef != NULL && (this->GetSpec() > myFrameDef->eLastAppearance || this->GetSpec() < myFrameDef->eFirstAppearance))
	{
		if (myFrameDef->convert != NULL)
		{
			tmpFrame = myFrameDef->convert(testframe, this->GetSpec());

			if (tmpFrame)
			{
				testframe = tmpFrame;
				frame = *tmpFrame;
			}

			//it's too old, and i couldn't convert
			return false; //disregard frame
		}

		//it's too old and doesn't have a conversion routine
		return false; //disregard frame
	}
	else if (myFrameDef->convert != NULL) //fields have stayed the same, but inside the field was a structure change
	{
		// TODO: add here code when conversion routine of tcon is ready v2.3 <> v2.4
	}

	// check the frames on their restrictions
	switch (testframe->GetID())
	{
		case ID3FID_UNIQUEFILEID:
		{
			//check for same owner
			tmpField = testframe->GetField(ID3FN_OWNER);

			if (ValidFrameOwner(tmpField->GetRawText()))
			{
				tmpFrame = this->Find(ID3FID_UNIQUEFILEID, ID3FN_OWNER, tmpField->GetRawText());

				if (tmpFrame && tmpFrame != testframe) this->RemoveFrame(tmpFrame); //remove old one, there can be only one

				return true;
			}

			return false;
		} //
		case ID3FID_CRYPTOREG:
		{
			//check for same owner
			tmpField = testframe->GetField(ID3FN_OWNER);

			if (ValidFrameOwner((char *)tmpField->GetRawText()))
			{
				tmpFrame = this->Find(ID3FID_CRYPTOREG, ID3FN_OWNER, tmpField->GetRawText());

				if (tmpFrame && tmpFrame != testframe) this->RemoveFrame(tmpFrame); //remove old one, there can be only one

				tmpField = testframe->GetField(ID3FN_ID);
				tmpFrame = this->Find(ID3FID_CRYPTOREG, ID3FN_ID, tmpField->Get());

				if (tmpFrame && tmpFrame != testframe) this->RemoveFrame(tmpFrame); //remove old one, there can be only one

				return true;
			}

			return false;
		}
		case ID3FID_GROUPINGREG:
		{
			//check for same owner
			tmpField = testframe->GetField(ID3FN_OWNER);
			if (ValidFrameOwner((char *)tmpField->GetRawText()))
			{
				tmpFrame = this->Find(ID3FID_GROUPINGREG, ID3FN_OWNER, tmpField->GetRawText());

				if (tmpFrame && tmpFrame != testframe) this->RemoveFrame(tmpFrame); //remove old one, there can be only one

				tmpField = testframe->GetField(ID3FN_ID);
				tmpFrame = this->Find(ID3FID_CRYPTOREG, ID3FN_ID, tmpField->Get());

				if (tmpFrame && tmpFrame != testframe) this->RemoveFrame(tmpFrame); //remove old one, there can be only one

				return true;
			}

			return false;
		}
		case ID3FID_PRIVATE:
		{
			//check for same owner
			tmpField = testframe->GetField(ID3FN_OWNER);

			if (ValidFrameOwner((char *)tmpField->GetRawText()))
			{
				// here the id3v2.4 specification is not clear, it states the content of the frame cannot be the same
				// do they mean that the there can be only one frame with the same owner?
				// or that the data field cannot be the same, or both..
				return true;
			}

			return false;
		} //
		case ID3FID_PRODUCEDNOTICE:
		{
			//should have at least a year and a space
			tmpField = testframe->GetField(ID3FN_TEXT);

			String	 tmpText = tmpField->GetText();

			if (tmpText.size() > 4) return true;

			return false;
		} //
		case ID3FID_COPYRIGHT:
		{
			//should have at least a year and a space
			tmpField = testframe->GetField(ID3FN_TEXT);

			String	 tmpText = tmpField->GetText();

			if (tmpText.size() > 4) return true;

			return false;
		} //
		case ID3FID_ENCODINGTIME:
		{
			//should have at least a year, contains a timestamp yyyy[-MM[-dd[THH[:mm[:ss]]]]] (between brackets [] is optional)
			tmpField = testframe->GetField(ID3FN_TEXT);

			String	 tmpText = tmpField->GetText();

			if (tmpText.size() > 3) return true;

			return false;
		}
		case ID3FID_ORIGRELEASETIME:
		{
			//should have at least a year, contains a timestamp yyyy[-MM[-dd[THH[:mm[:ss]]]]] (between brackets [] is optional)
			tmpField = testframe->GetField(ID3FN_TEXT);

			String	 tmpText = tmpField->GetText();

			if (tmpText.size() > 3) return true;

			return false;
		}
		case ID3FID_RECORDINGTIME:
		{
			//should have at least a year, contains a timestamp yyyy[-MM[-dd[THH[:mm[:ss]]]]] (between brackets [] is optional)
			tmpField = testframe->GetField(ID3FN_TEXT);

			String	 tmpText = tmpField->GetText();

			if (tmpText.size() > 3) return true;

			return false;
		}
		case ID3FID_RELEASETIME:
		{
			//should have at least a year, contains a timestamp yyyy[-MM[-dd[THH[:mm[:ss]]]]] (between brackets [] is optional)
			tmpField = testframe->GetField(ID3FN_TEXT);

			String	 tmpText = tmpField->GetText();

			if (tmpText.size() > 3) return true;

			return false;
		}
		case ID3FID_TAGGINGTIME:
		{
			//should have at least a year, contains a timestamp yyyy[-MM[-dd[THH[:mm[:ss]]]]] (between brackets [] is optional)
			tmpField = testframe->GetField(ID3FN_TEXT);

			String	 tmpText = tmpField->GetText();

			if (tmpText.size() > 3) return true;

			return false;
		}
		case ID3FID_CDID:
		{
			//should have at least a 4 + 8*x + 8 bytes (x is nr of tracks), with a minimum of 1 track it's 20 bytes, maximum is 804 bytes (99 tracks)
			tmpField = testframe->GetField(ID3FN_DATA);

			BString	 tmpText = tmpField->GetBinary();

			if (tmpText.size() >= 20 && tmpText.size() <= 804)
			{
				if (testlinkedFrames)
				{ //check for existence of TRCK frame
					tmpFrame = this->Find(ID3FID_TRACKNUM);

					if (tmpFrame) return true; //todo...
					else	      return false;
				}

				return true;
			}

			return false;
		}//
		default:
		{
			return true;
		}
	}
}

void ID3_TagImpl::checkFrames()
{
	bool	 restart = false;

	for (iterator iter = this->begin(); iter != this->end(); ++iter)
	{
		ID3_Frame	*frame = *iter;
		ID3_Frame	&testframe = *frame;

		if (this->IsValidFrame(testframe, true) == false)
		{
			_frames.erase(iter);

			delete frame;

			restart = true;

			break;
		}
	}

	if (restart) this->checkFrames();
}

bool ID3_TagImpl::AttachFrame(ID3_Frame *frame)
{
	if (NULL == frame)
	{
		/* TODO: log this
		 */
		return false;
	}

	_frames.push_back(frame);
	_cursor = _frames.begin();

	_changed = true;

	return true;
}


ID3_Frame* ID3_TagImpl::RemoveFrame(const ID3_Frame *frame)
{
	ID3_Frame	*frm = NULL;
	iterator	 fi = Find(frame);

	if (fi != _frames.end())
	{
		frm = *fi;
		_frames.erase(fi);
		_cursor = _frames.begin();
		_changed = true;
	}

	return frm;
}


bool ID3_TagImpl::HasChanged() const
{
	bool	 changed = _changed;

	if (!changed)
	{
		for (const_iterator fi = _frames.begin(); fi != _frames.end(); ++fi)
		{
			if (*fi) changed = (*fi)->HasChanged();

			if (changed) break;
		}
	}

	return changed;
}

bool ID3_TagImpl::SetSpec(ID3_V2Spec spec)
{
	bool	 changed = _hdr.SetSpec(spec);

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
	if (this->GetExtended())
	{
		if	(this->GetSpec() == ID3V2_4_0) return  6; // minimal ID3v2.4 ext header size
		else if (this->GetSpec() == ID3V2_3_0) return 10; // minimal ID3v2.3 ext header size
	}

	return 0; // not implemented
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
