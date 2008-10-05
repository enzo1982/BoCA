 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
	Config		*currentConfig = Config::Get();

	buffer.Resize(4 + strlen(vendorString.ConvertTo("UTF-8")) + 4);

	Int		 numItems = 0;

	if	(track.artist != NIL) { RenderTagItem("ARTIST", track.artist, buffer);			    numItems++; }
	if	(track.title  != NIL) { RenderTagItem("TITLE", track.title, buffer);			    numItems++; }
	if	(track.album  != NIL) { RenderTagItem("ALBUM", track.album, buffer);			    numItems++; }
	if	(track.track   >   0) { RenderTagItem("TRACKNUMBER", String::FromInt(track.track), buffer); numItems++; }
	if	(track.year    >   0) { RenderTagItem("DATE", String::FromInt(track.year), buffer);	    numItems++; }
	if	(track.genre  != NIL) { RenderTagItem("GENRE", track.genre, buffer);			    numItems++; }
	if	(track.label  != NIL) { RenderTagItem("ORGANIZATION", track.label, buffer);		    numItems++; }
	if	(track.isrc   != NIL) { RenderTagItem("ISRC", track.isrc, buffer);			    numItems++; }

	if	(track.comment != NIL && !currentConfig->replace_comments) { RenderTagItem("COMMENT", track.comment, buffer);		       numItems++; }
	else if (currentConfig->default_comment != NIL)			   { RenderTagItem("COMMENT", currentConfig->default_comment, buffer); numItems++; }

	RenderTagHeader(vendorString, numItems, buffer);

	return buffer.Size();
}

Int BoCA::TagVorbis::RenderTagHeader(const String &vendorString, Int numItems, Buffer<UnsignedByte> &buffer)
{
	OutStream	 out(STREAM_BUFFER, buffer, 4 + strlen(vendorString.ConvertTo("UTF-8")) + 4);

	out.OutputNumber(strlen(vendorString.ConvertTo("UTF-8")), 4);
	out.OutputString(vendorString.ConvertTo("UTF-8"));
	out.OutputNumber(numItems, 4);

	return Success();
}

Int BoCA::TagVorbis::RenderTagItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
{
	Int		 size = id.Length() + strlen(value.ConvertTo("UTF-8")) + 5;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(size - 4, 4);
	out.OutputString(id);
	out.OutputNumber('=', 1);
	out.OutputString(value.ConvertTo("UTF-8"));

	return Success();
}

Int BoCA::TagVorbis::Parse(const String &fileName, Track *track)
{
	InStream	 in(STREAM_FILE, fileName, IS_READONLY);

	ogg_sync_state	 oy;

	ex_ogg_sync_init(&oy);

	Int	 size = Math::Min(4096, in.Size());
	char	*fbuffer = ex_ogg_sync_buffer(&oy, size);

	in.InputData(fbuffer, size);

	ex_ogg_sync_wrote(&oy, size);

	if (oy.data != NIL)
	{
	}

	ex_ogg_sync_clear(&oy);

	return Error();
}
