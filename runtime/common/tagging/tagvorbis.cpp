 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/tagging/tagvorbis.h>
#include <boca/common/config.h>
#include <boca/core/dllinterfaces.h>

using namespace smooth::IO;

BoCA::TagVorbis::TagVorbis()
{
}

BoCA::TagVorbis::~TagVorbis()
{
}

Int BoCA::TagVorbis::Render(const Track &track, Buffer<UnsignedByte> &buffer, const String &vendorString)
{
	Config	*currentConfig = Config::Get();
	char	*prevOutFormat = currentConfig->GetStringValue("Tags", "VorbisCommentEncoding", "UTF-8");

	const Info	&info = track.GetInfo();

	buffer.Resize(4 + strlen(vendorString) + 4);

	Int	 numItems = 0;

	if	(info.artist != NIL) { RenderTagItem("ARTIST", info.artist, buffer);				  numItems++; }
	if	(info.title  != NIL) { RenderTagItem("TITLE", info.title, buffer);				  numItems++; }
	if	(info.album  != NIL) { RenderTagItem("ALBUM", info.album, buffer);				  numItems++; }
	if	(info.track   >   0) { RenderTagItem("TRACKNUMBER", String(info.track < 10 ? "0" : "")
								   .Append(String::FromInt(info.track)), buffer); numItems++; }
	if	(info.year    >   0) { RenderTagItem("DATE", String::FromInt(info.year), buffer);		  numItems++; }
	if	(info.genre  != NIL) { RenderTagItem("GENRE", info.genre, buffer);				  numItems++; }
	if	(info.label  != NIL) { RenderTagItem("ORGANIZATION", info.label, buffer);			  numItems++; }
	if	(info.isrc   != NIL) { RenderTagItem("ISRC", info.isrc, buffer);				  numItems++; }

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	{ RenderTagItem("COMMENT", info.comment, buffer);						  numItems++; }
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL && numItems > 0)		{ RenderTagItem("COMMENT", currentConfig->GetStringValue("Tags", "DefaultComment", NIL), buffer); numItems++; }

	if (currentConfig->GetIntValue("Tags", "PreserveReplayGain", True))
	{
		if (info.track_gain != NIL && info.track_peak != NIL)
		{
			{ RenderTagItem("replaygain_track_gain", info.track_gain, buffer); numItems++; }
			{ RenderTagItem("replaygain_track_peak", info.track_peak, buffer); numItems++; }
		}

		if (info.album_gain != NIL && info.album_peak != NIL)
		{
			{ RenderTagItem("replaygain_album_gain", info.album_gain, buffer); numItems++; }
			{ RenderTagItem("replaygain_album_peak", info.album_peak, buffer); numItems++; }
		}
	}

	if (currentConfig->GetIntValue("Tags", "WriteCoverArt", True) && currentConfig->GetIntValue("Tags", "WriteCoverArtVorbisComment", False))
	{
		/* This is an unofficial way to store cover art in Vorbis
		 * comments. It is used by some existing software.
		 *
		 * Copy only the first picture. It's not clear if any other
		 * software can handle multiple pictures in Vorbis comments.
		 */
		if (track.pictures.Length() > 0)
		{
			const Picture		&picInfo = track.pictures.GetFirst();
			Buffer<UnsignedByte>	 picBuffer(picInfo.data.Size());

			memcpy(picBuffer, picInfo.data, picInfo.data.Size());

			RenderTagItem("COVERARTMIME", picInfo.mime, buffer);
			RenderTagItem("COVERART", Encoding::Base64(picBuffer).Encode(), buffer);

			numItems += 2;
		}
	}

	RenderTagHeader(vendorString, numItems, buffer);

	String::SetOutputFormat(prevOutFormat);

	return buffer.Size();
}

Int BoCA::TagVorbis::RenderTagHeader(const String &vendorString, Int numItems, Buffer<UnsignedByte> &buffer)
{
	OutStream	 out(STREAM_BUFFER, buffer, 4 + strlen(vendorString) + 4);

	out.OutputNumber(strlen(vendorString), 4);
	out.OutputString(vendorString);
	out.OutputNumber(numItems, 4);

	return Success();
}

Int BoCA::TagVorbis::RenderTagItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
{
	Int		 size = id.Length() + strlen(value) + 5;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(size - 4, 4);
	out.OutputString(id);
	out.OutputNumber('=', 1);
	out.OutputString(value);

	return Success();
}

Int BoCA::TagVorbis::Parse(const Buffer<UnsignedByte> &buffer, Track *track)
{
	char		*prevInFormat = String::SetInputFormat("UTF-8");
	InStream	 in(STREAM_BUFFER, buffer, buffer.Size());

	/* Skip vendor string.
	 */
	in.RelSeek(in.InputNumber(4));

	/* Parse individual comment items.
	 */
	Int	 numItems = in.InputNumber(4);
	Info	&info = track->GetInfo();

	for (Int i = 0; i < numItems; i++)
	{
		Int	 length	 = in.InputNumber(4);
		String	 comment = in.InputString(length);

		String	 id	 = comment.Head(comment.Find("=")).ToUpper();
		String	 value	 = comment.Tail(comment.Length() - comment.Find("=") - 1);

		if	(id == "ARTIST")       info.artist  = value;
		else if (id == "TITLE")	       info.title   = value;
		else if (id == "ALBUM")	       info.album   = value;
		else if (id == "TRACKNUMBER")  info.track   = value.ToInt();
		else if (id == "DATE")	       info.year    = value.ToInt();
		else if (id == "GENRE")	       info.genre   = value;
		else if (id == "COMMENT")      info.comment = value;
		else if (id == "ORGANIZATION") info.label   = value;
		else if (id == "ISRC")	       info.isrc    = value;
		else if (id.StartsWith("REPLAYGAIN"))
		{
			if	(id == "REPLAYGAIN_TRACK_GAIN") info.track_gain = value;
			else if (id == "REPLAYGAIN_TRACK_PEAK") info.track_peak = value;
			else if (id == "REPLAYGAIN_ALBUM_GAIN") info.album_gain = value;
			else if (id == "REPLAYGAIN_ALBUM_PEAK") info.album_peak = value;
		}
		else if (id == "COVERART")
		{
			/* This is an unofficial way to store cover art in Vorbis
			 * comments. It is used by some existing software.
			 */
			Picture	 picture;

			Encoding::Base64(picture.data).Decode(value);

			if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
			else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
				 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
				 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
				 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

			picture.type = 0;

			track->pictures.Add(picture);
		}
	}

	String::SetInputFormat(prevInFormat);

	return Success();
}
