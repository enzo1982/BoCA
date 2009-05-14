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
#include <boca/common/tagging/tagid3v1.h>
#include <boca/common/tagging/tagid3v2.h>
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

Bool BoCA::Track::operator ==(const int nil) const
{
	if (trackID == -1) return True;
	else		   return False;
}

Bool BoCA::Track::operator !=(const int nil) const
{
	if (trackID == -1) return False;
	else		   return True;
}

Bool BoCA::Track::LoadCoverArtFiles()
{
	if (isCDTrack) return False;

	Directory		 directory = File(origFilename).GetFilePath();
	const Array<File>	&jpgFiles = directory.GetFilesByPattern("*.jpg");

	foreach (File file, jpgFiles) LoadCoverArtFile(file);

	const Array<File>	&jpegFiles = directory.GetFilesByPattern("*.jpeg");

	foreach (File file, jpegFiles) LoadCoverArtFile(file);

	const Array<File>	&pngFiles = directory.GetFilesByPattern("*.png");

	foreach (File file, pngFiles) LoadCoverArtFile(file);

	return True;
}

Bool BoCA::Track::LoadCoverArtFile(const String &file)
{
	Picture	 picture;

	picture.LoadFromFile(file);

	/* Check if the cover art is already in our list.
	 */
	for (Int i = 0; i < pictures.Length(); i++)
	{
		if (pictures.GetNthReference(i).data.Size() != picture.data.Size()) continue;

		if (memcmp(pictures.GetNthReference(i).data, picture.data, picture.data.Size()) == 0) return True;
	}

	if	(file.Find("front") >= 0) picture.type = 0x03; // Cover (front)
	else if (file.Find("back")  >= 0) picture.type = 0x04; // Cover (back)
	else if (file.Find("disc")  >= 0) picture.type = 0x06; // Media

	pictures.Add(picture);

	return True;
}

Int BoCA::Track::RenderID3v1Tag(Buffer<UnsignedByte> &buffer)
{
	return TagID3v1().Render(*this, buffer);
}

Bool BoCA::Track::ParseID3v1Tag(const Buffer<UnsignedByte> &buffer)
{
	return TagID3v1().Parse(buffer, this) == Success();
}

Bool BoCA::Track::ParseID3v1Tag(const String &fileName)
{
	return TagID3v1().Parse(fileName, this) == Success();
}

Int BoCA::Track::RenderID3v2Tag(Buffer<UnsignedByte> &buffer)
{
	return TagID3v2().Render(*this, buffer);
}

Bool BoCA::Track::ParseID3v2Tag(const Buffer<UnsignedByte> &buffer)
{
	return TagID3v2().Parse(buffer, this) == Success();
}

Bool BoCA::Track::ParseID3v2Tag(const String &fileName)
{
	return TagID3v2().Parse(fileName, this) == Success();
}

Int BoCA::Track::RenderAPETag(Buffer<UnsignedByte> &buffer)
{
	return TagAPE().Render(*this, buffer);
}

Bool BoCA::Track::ParseAPETag(const Buffer<UnsignedByte> &buffer)
{
	return TagAPE().Parse(buffer, this) == Success();
}

Bool BoCA::Track::ParseAPETag(const String &fileName)
{
	return TagAPE().Parse(fileName, this) == Success();
}

Bool BoCA::Track::RenderMP4Meta(const String &fileName)
{
	return TagMP4().Render(*this, fileName) == Success();
}

Bool BoCA::Track::ParseMP4Meta(const String &fileName)
{
	return TagMP4().Parse(fileName, this) == Success();
}

Int BoCA::Track::RenderVorbisComment(Buffer<UnsignedByte> &buffer, const String &vendorString)
{
	return TagVorbis().Render(*this, buffer, vendorString);
}

Bool BoCA::Track::ParseVorbisComment(const Buffer<UnsignedByte> &buffer)
{
	return TagVorbis().Parse(buffer, this) == Success();
}
