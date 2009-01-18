 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/track/track.h>
#include <boca/common/tagging/tagape.h>
#include <boca/common/tagging/tagid3.h>
#include <boca/common/tagging/tagmp4.h>
#include <boca/common/tagging/tagvorbis.h>

Int BoCA::Track::nextTrackID = 0;

BoCA::Track::Track()
{
	trackID		= nextTrackID++;

	length		= -1;
	approxLength	= -1;
	fileSize	= -1;

	isCDTrack	= False;

	drive		= -1;
	discid		= 0;
	cdTrack		= -1;
}

BoCA::Track::Track(int nil)
{
	trackID		= -1;
}

BoCA::Track::Track(const Track &oTrack)
{
	*this = oTrack;
}

BoCA::Track::~Track()
{
}

BoCA::Track &BoCA::Track::operator =(const int nil)
{
	trackID		= -1;

	return *this;
}

BoCA::Track &BoCA::Track::operator =(const Track &oTrack)
{
	if (&oTrack == this) return *this;

	trackID		= oTrack.trackID;

	format		= oTrack.format;

	info		= oTrack.info;
	originalInfo	= oTrack.originalInfo;

	length		= oTrack.length;
	approxLength	= oTrack.approxLength;
	fileSize	= oTrack.fileSize;

	isCDTrack	= oTrack.isCDTrack;
	drive		= oTrack.drive;
	cdTrack		= oTrack.cdTrack;

	pictures.RemoveAll();

	foreach (const Picture &picture, oTrack.pictures)
	{
		pictures.Add(picture);
	}

	offset		= oTrack.offset;
	discid		= oTrack.discid;
	category	= oTrack.category;
	revision	= oTrack.revision;
	disclength	= oTrack.disclength;
	discComment	= oTrack.discComment;
	playorder	= oTrack.playorder;

	fileSizeString	= oTrack.fileSizeString;
	lengthString	= oTrack.lengthString;

	outfile		= oTrack.outfile;
	origFilename	= oTrack.origFilename;

	return *this;
}

S::Bool BoCA::Track::operator ==(const int nil) const
{
	if (trackID == -1) return True;
	else		   return False;
}

S::Bool BoCA::Track::operator !=(const int nil) const
{
	if (trackID == -1) return False;
	else		   return True;
}

Int BoCA::Track::RenderID3Tag(Buffer<UnsignedByte> &buffer, Int version)
{
	TagID3	 tag;

	tag.SetID3Version(version);

	return tag.Render(*this, buffer);
}

Bool BoCA::Track::ParseID3Tag(const Buffer<UnsignedByte> &buffer)
{
	TagID3	 tag;

	return tag.Parse(buffer, this);
}

Bool BoCA::Track::ParseID3Tag(const String &fileName)
{
	TagID3	 tag;

	return tag.Parse(fileName, this);
}

Int BoCA::Track::RenderAPETag(Buffer<UnsignedByte> &buffer)
{
	TagAPE	 tag;

	return tag.Render(*this, buffer);
}

Bool BoCA::Track::ParseAPETag(const Buffer<UnsignedByte> &buffer)
{
	TagAPE	 tag;

	return tag.Parse(buffer, this);
}

Bool BoCA::Track::ParseAPETag(const String &fileName)
{
	TagAPE	 tag;

	return tag.Parse(fileName, this);
}

Bool BoCA::Track::RenderMP4Meta(const String &fileName)
{
	TagMP4	 tag;

	return tag.Render(*this, fileName);
}

Bool BoCA::Track::ParseMP4Meta(const String &fileName)
{
	TagMP4	 tag;

	return tag.Parse(fileName, this);
}

Int BoCA::Track::RenderVorbisComment(Buffer<UnsignedByte> &buffer, const String &vendorString)
{
	TagVorbis	 tag;

	return tag.Render(*this, buffer, vendorString);
}

Bool BoCA::Track::ParseVorbisComment(const Buffer<UnsignedByte> &buffer)
{
	TagVorbis	 tag;

	return tag.Parse(buffer, this);
}
