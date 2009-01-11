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
	char	*prevOutFormat = String::SetOutputFormat("UTF-8");

	buffer.Resize(4 + strlen(vendorString) + 4);

	Int	 numItems = 0;

	if	(track.artist != NIL) { RenderTagItem("ARTIST", track.artist, buffer);				    numItems++; }
	if	(track.title  != NIL) { RenderTagItem("TITLE", track.title, buffer);				    numItems++; }
	if	(track.album  != NIL) { RenderTagItem("ALBUM", track.album, buffer);				    numItems++; }
	if	(track.track   >   0) { RenderTagItem("TRACKNUMBER", String(track.track < 10 ? "0" : "")
								    .Append(String::FromInt(track.track)), buffer); numItems++; }
	if	(track.year    >   0) { RenderTagItem("DATE", String::FromInt(track.year), buffer);		    numItems++; }
	if	(track.genre  != NIL) { RenderTagItem("GENRE", track.genre, buffer);				    numItems++; }
	if	(track.label  != NIL) { RenderTagItem("ORGANIZATION", track.label, buffer);			    numItems++; }
	if	(track.isrc   != NIL) { RenderTagItem("ISRC", track.isrc, buffer);				    numItems++; }

	if	(track.comment != NIL && !currentConfig->replace_comments) { RenderTagItem("COMMENT", track.comment, buffer);		       numItems++; }
	else if (currentConfig->default_comment != NIL && numItems > 0)	   { RenderTagItem("COMMENT", currentConfig->default_comment, buffer); numItems++; }

	if (currentConfig->GetIntValue("Settings", "CopyPictureTags", 1) && currentConfig->GetIntValue("Settings", "WriteVorbisCoverArt", 0))
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

	for (Int i = 0; i < numItems; i++)
	{
		Int	 length = in.InputNumber(4);
		String	 comment = in.InputString(length);

		String	 id = comment.Head(comment.Find("="));
		String	 value = comment.Tail(comment.Length() - comment.Find("=") - 1);

		if	(id == "ARTIST")       track->artist  = value;
		else if (id == "TITLE")	       track->title   = value;
		else if (id == "ALBUM")	       track->album   = value;
		else if (id == "TRACKNUMBER")  track->track   = value.ToInt();
		else if (id == "DATE")	       track->year    = value.ToInt();
		else if (id == "GENRE")	       track->genre   = value;
		else if (id == "COMMENT")      track->comment = value;
		else if (id == "ORGANIZATION") track->label   = value;
		else if (id == "ISRC")	       track->isrc    = value;
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
