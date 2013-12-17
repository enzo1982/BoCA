 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <string.h>

#include "vorbis.h"

using namespace smooth::IO;

const String &BoCA::TaggerVorbis::GetComponentSpecs()
{
	static String	 componentSpecs = "			\
								\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
	  <component>						\
	    <name>Vorbis Comment Tagger</name>			\
	    <version>1.0</version>				\
	    <id>vorbis-tag</id>					\
	    <type>tagger</type>					\
	    <tagformat>						\
	      <name>Vorbis Comment</name>			\
	      <coverart supported=\"true\" default=\"false\"/>	\
	      <encodings>					\
		<encoding>UTF-8</encoding>			\
	      </encodings>					\
	    </tagformat>					\
	  </component>						\
								\
	";

	return componentSpecs;
}

BoCA::TaggerVorbis::TaggerVorbis()
{
}

BoCA::TaggerVorbis::~TaggerVorbis()
{
}

Error BoCA::TaggerVorbis::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	Config		*currentConfig = Config::Get();
	String		 prevOutFormat = String::SetOutputFormat("UTF-8");

	const Info	&info = track.GetInfo();

	buffer.Resize(4 + strlen(vendorString) + 4);

	Int	 numItems = 0;

	if	(info.artist != NIL) { RenderTagItem("ARTIST", info.artist, buffer);		  numItems++; }
	if	(info.title  != NIL) { RenderTagItem("TITLE", info.title, buffer);		  numItems++; }
	if	(info.album  != NIL) { RenderTagItem("ALBUM", info.album, buffer);		  numItems++; }
	if	(info.year    >   0) { RenderTagItem("DATE", String::FromInt(info.year), buffer); numItems++; }
	if	(info.genre  != NIL) { RenderTagItem("GENRE", info.genre, buffer);		  numItems++; }
	if	(info.label  != NIL) { RenderTagItem("ORGANIZATION", info.label, buffer);	  numItems++; }
	if	(info.isrc   != NIL) { RenderTagItem("ISRC", info.isrc, buffer);		  numItems++; }

	if	(info.track	> 0) { RenderTagItem("TRACKNUMBER", String(info.track < 10 ? "0" : NIL).Append(String::FromInt(info.track)), buffer);	    numItems++; }
	if	(info.numTracks > 0) { RenderTagItem("TRACKTOTAL", String(info.numTracks < 10 ? "0" : NIL).Append(String::FromInt(info.numTracks)), buffer); numItems++; }

	if	(info.disc	> 0) { RenderTagItem("DISCNUMBER", String(info.disc < 10 ? "0" : NIL).Append(String::FromInt(info.disc)), buffer);	    numItems++; }
	if	(info.numDiscs	> 0) { RenderTagItem("DISCTOTAL", String(info.numDiscs < 10 ? "0" : NIL).Append(String::FromInt(info.numDiscs)), buffer);    numItems++; }

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	{ RenderTagItem("COMMENT", info.comment, buffer);						  numItems++; }
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL && numItems > 0)		{ RenderTagItem("COMMENT", currentConfig->GetStringValue("Tags", "DefaultComment", NIL), buffer); numItems++; }

	/* Save other text info.
	 */
	for (Int i = 0; i < info.other.Length(); i++)
	{
		const String	&pair  = info.other.GetNth(i);

		String		 key   = pair.Head(pair.Find(":") + 1);
		String		 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == String(INFO_CONDUCTOR).Append(":")) { RenderTagItem("PERFORMER", value, buffer); numItems++; }
		else if	(key == String(INFO_COMPOSER).Append(":"))  { RenderTagItem("COMPOSER",  value, buffer); numItems++; }
		else if	(key == String(INFO_LYRICIST).Append(":"))  { RenderTagItem("LYRICIST",  value, buffer); numItems++; }
	}

	/* Save Replay Gain info.
	 */
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

	/* Save CD table of contents.
	 */
	if (currentConfig->GetIntValue("Tags", "WriteMCDI", True))
	{
		if	(info.mcdi.GetData().Size() > 0) { RenderTagItem("CDTOC", info.mcdi.GetOffsetString(), buffer);	numItems++; }
		else if	(info.offsets != NIL)		 { RenderTagItem("CDTOC", info.offsets, buffer);		numItems++; }
	}

	/* Save cover art.
	 */
	if (currentConfig->GetIntValue("Tags", "CoverArtWriteToTags", True) && currentConfig->GetIntValue("Tags", "CoverArtWriteToVorbisComment", False))
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

	return Success();
}

Int BoCA::TaggerVorbis::RenderTagHeader(const String &vendorString, Int numItems, Buffer<UnsignedByte> &buffer)
{
	OutStream	 out(STREAM_BUFFER, buffer, 4 + strlen(vendorString) + 4);

	out.OutputNumber(strlen(vendorString), 4);
	out.OutputString(vendorString);
	out.OutputNumber(numItems, 4);

	return Success();
}

Int BoCA::TaggerVorbis::RenderTagItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer)
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

Error BoCA::TaggerVorbis::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	Config		*currentConfig = Config::Get();

	InStream	 in(STREAM_BUFFER, buffer, buffer.Size());

	/* Skip vendor string.
	 */
	Int		 vendorLength = in.InputNumber(4);

	if (vendorLength < 0 || vendorLength > buffer.Size() - 8) return Error();

	in.RelSeek(vendorLength);

	/* Parse individual comment items.
	 */
	Int	 numItems = in.InputNumber(4);
	Info	 info = track.GetInfo();
	String	 prevInFormat = String::SetInputFormat("UTF-8");

	for (Int i = 0; i < numItems; i++)
	{
		/* Read and check next comment string length.
		 */
		Int	 length	 = in.InputNumber(4);

		if (length < 0 || length > buffer.Size() - in.GetPos()) break;

		/* Read and assign actual comment string.
		 */
		String	 comment = in.InputString(length);

		String	 id	 = comment.Head(comment.Find("=")).ToUpper();
		String	 value	 = comment.Tail(comment.Length() - comment.Find("=") - 1);

		if	(id == "ARTIST")       info.artist    = value;
		else if (id == "TITLE")	       info.title     = value;
		else if (id == "ALBUM")	       info.album     = value;
		else if (id == "DATE")	       info.year      = value.ToInt();
		else if (id == "GENRE")	       info.genre     = value;
		else if (id == "COMMENT")      info.comment   = value;
		else if (id == "ORGANIZATION") info.label     = value;
		else if (id == "ISRC")	       info.isrc      = value;

		else if (id == "TRACKNUMBER")  info.track     = value.ToInt();
		else if (id == "TRACKTOTAL")   info.numTracks = value.ToInt();

		else if (id == "DISCNUMBER")   info.disc      = value.ToInt();
		else if (id == "DISCTOTAL")    info.numDiscs  = value.ToInt();

		else if (id == "PERFORMER")    info.other.Add(String(INFO_CONDUCTOR).Append(":").Append(value));
		else if (id == "COMPOSER")     info.other.Add(String(INFO_COMPOSER).Append(":").Append(value));
		else if (id == "LYRICIST")     info.other.Add(String(INFO_LYRICIST).Append(":").Append(value));

		else if (id.StartsWith("REPLAYGAIN"))
		{
			if	(id == "REPLAYGAIN_TRACK_GAIN") info.track_gain = value;
			else if (id == "REPLAYGAIN_TRACK_PEAK") info.track_peak = value;
			else if (id == "REPLAYGAIN_ALBUM_GAIN") info.album_gain = value;
			else if (id == "REPLAYGAIN_ALBUM_PEAK") info.album_peak = value;
		}
		else if (id == "CDTOC")
		{
			info.offsets = value;
		}
		else if (id == "COVERART" && currentConfig->GetIntValue("Tags", "CoverArtReadFromTags", True))
		{
			/* This is an unofficial way to store cover art in Vorbis
			 * comments. It is used by some existing software.
			 */
			Picture			 picture;
			Buffer<UnsignedByte>	 buffer;

			Encoding::Base64(buffer).Decode(value);

			picture.data = buffer;

			if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
			else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
				 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
				 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
				 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

			picture.type = 0;

			track.pictures.Add(picture);
		}
	}

	track.SetInfo(info);

	String::SetInputFormat(prevInFormat);

	return Success();
}