 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/track.h>
#include <boca/common/tagging/tagape.h>
#include <boca/common/tagging/tagid3.h>
#include <boca/common/tagging/tagmp4.h>

Int BoCA::Track::nextTrackID = 0;

BoCA::Track::Track()
{
	trackID		= nextTrackID++;

	channels	= 0;
	rate		= 0;
	bits		= 0;
	length		= -1;
	approxLength	= -1;
	fileSize	= -1;
	order		= BYTE_INTEL;

	isCDTrack	= False;

	drive		= -1;
	discid		= 0;
	cdTrack		= -1;

	track		= -1;
	year		= -1;
}

BoCA::Track::Track(const Track &oTrack)
{
	*this = oTrack;
}

BoCA::Track::~Track()
{
}

BoCA::Track &BoCA::Track::operator =(const Track &oTrack)
{
	trackID		= oTrack.trackID;

	channels	= oTrack.channels;
	rate		= oTrack.rate;
	bits		= oTrack.bits;
	length		= oTrack.length;
	approxLength	= oTrack.approxLength;
	fileSize	= oTrack.fileSize;
	order		= oTrack.order;

	isCDTrack	= oTrack.isCDTrack;
	drive		= oTrack.drive;
	cdTrack		= oTrack.cdTrack;

	artist		= oTrack.artist;
	title		= oTrack.title;
	album		= oTrack.album;
	track		= oTrack.track;
	genre		= oTrack.genre;
	year		= oTrack.year;
	comment		= oTrack.comment;

	oArtist		= oTrack.oArtist;
	oTitle		= oTrack.oTitle;
	oAlbum		= oTrack.oAlbum;
	oGenre		= oTrack.oGenre;

	for (Int i = 0; i < oTrack.pictures.Length(); i++)
	{
		pictures.Add(oTrack.pictures.GetNth(i));
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

Int BoCA::Track::RenderID3Tag(Buffer<UnsignedByte> &buffer, Int version)
{
	TagID3	 tag;

	tag.SetID3Version(version);

	return tag.Render(*this, buffer);
}

Bool BoCA::Track::ParseID3Tag(Buffer<UnsignedByte> &buffer)
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

Bool BoCA::Track::ParseAPETag(Buffer<UnsignedByte> &buffer)
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
