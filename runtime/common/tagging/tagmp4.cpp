 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/tagging/tagmp4.h>
#include <boca/common/config.h>
#include <boca/common/utilities.h>
#include <boca/core/dllinterfaces.h>

using namespace smooth::IO;

BoCA::TagMP4::TagMP4()
{
}

BoCA::TagMP4::~TagMP4()
{
}

Int BoCA::TagMP4::Render(const Track &track, const String &fileName)
{
	Config		*currentConfig = Config::Get();

	MP4FileHandle	 mp4File;

	if (String::IsUnicode(fileName))
	{
		File(fileName).Copy(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"));

		mp4File = ex_MP4Modify(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"), 0, 0);
	}
	else
	{
		mp4File = ex_MP4Modify(fileName, 0, 0);
	}

	char	*prevOutFormat = String::SetOutputFormat(currentConfig->mp4meta_encoding);

	if	(track.artist != NIL) ex_MP4SetMetadataArtist(mp4File, track.artist);
	if	(track.title  != NIL) ex_MP4SetMetadataName(mp4File, track.title);
	if	(track.album  != NIL) ex_MP4SetMetadataAlbum(mp4File, track.album);
	if	(track.track   >   0) ex_MP4SetMetadataTrack(mp4File, track.track, 0);
	if	(track.year    >   0) ex_MP4SetMetadataYear(mp4File, String::FromInt(track.year));
	if	(track.genre  != NIL) ex_MP4SetMetadataGenre(mp4File, track.genre);

	if	(track.comment != NIL && !currentConfig->replace_comments) ex_MP4SetMetadataComment(mp4File, track.comment);
	else if (currentConfig->default_comment != NIL)			   ex_MP4SetMetadataComment(mp4File, currentConfig->default_comment);

	if (currentConfig->copy_picture_tags)
	{
		/* Copy only the first picture. The MP4v2 API doesn't
		 * currently support multiple pictures in a file.
		 */
		if (track.pictures.Length() > 0)
		{
			const Picture	&picInfo = track.pictures.GetFirst();

			ex_MP4SetMetadataCoverArt(mp4File, picInfo.data, picInfo.data.Size());
		}
	}

	String::SetOutputFormat(prevOutFormat);

	ex_MP4Close(mp4File);

	ex_MP4Optimize(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"), NIL, 0);

	if (String::IsUnicode(fileName))
	{
		File(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag")).Delete();
	}

	return Success();
}

Int BoCA::TagMP4::Parse(const String &fileName, Track *track)
{
	MP4FileHandle	 mp4File;

	if (String::IsUnicode(fileName))
	{
		File(fileName).Copy(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"));

		mp4File = ex_MP4Read(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"), 0);
	}
	else
	{
		mp4File = ex_MP4Read(fileName, 0);
	}

	char		*buffer		= NIL;
	unsigned short	 trackNr	= 0;
	unsigned short	 nOfTracks	= 0;

	char	*prevInFormat = String::SetInputFormat("UTF-8");

	if (ex_MP4GetMetadataName(mp4File, &buffer))						{ track->title = buffer; ex_MP4Free(buffer); }
	if (ex_MP4GetMetadataArtist(mp4File, &buffer))						{ track->artist = buffer; ex_MP4Free(buffer); }
	if (ex_MP4GetMetadataYear(mp4File, &buffer))						{ track->year = String(buffer).ToInt(); ex_MP4Free(buffer); }
	if (ex_MP4GetMetadataAlbum(mp4File, &buffer))						{ track->album = buffer; ex_MP4Free(buffer); }
	if (ex_MP4GetMetadataGenre(mp4File, &buffer))						{ track->genre = buffer; ex_MP4Free(buffer); }
	if (ex_MP4GetMetadataTrack(mp4File, (u_int16_t *) &trackNr, (u_int16_t *) &nOfTracks))	{ track->track = trackNr; }
	if (ex_MP4GetMetadataComment(mp4File, &buffer))						{ track->comment = buffer; ex_MP4Free(buffer); }

	for (UnsignedInt i = 0; i < ex_MP4GetMetadataCoverArtCount(mp4File); i++)
	{
		Picture	 picture;

		unsigned char	*buffer	= NIL;
		u_int32_t	 size	= 0;

		ex_MP4GetMetadataCoverArt(mp4File, &buffer, &size, i);

		if (size > 0)
		{
			picture.data.Resize(size);

			memcpy(picture.data, buffer, size);

			if	(buffer[0] == 0xFF && buffer[1] == 0xD8) picture.mime = "image/jpeg";
			else if (buffer[0] == 0x89 && buffer[1] == 0x50 &&
				 buffer[2] == 0x4E && buffer[3] == 0x47 &&
				 buffer[4] == 0x0D && buffer[5] == 0x0A &&
				 buffer[6] == 0x1A && buffer[7] == 0x0A) picture.mime = "image/png";

			picture.type = 0;

			track->pictures.Add(picture);
		}
	}

	String::SetInputFormat(prevInFormat);

	ex_MP4Close(mp4File);

	if (String::IsUnicode(fileName))
	{
		File(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag")).Delete();
	}

	return Success();
}
