 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
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
	    <tagspec>						\
	      <name>Vorbis Comment</name>			\
	      <coverart supported=\"true\" default=\"false\"/>	\
	      <encodings>					\
		<encoding>UTF-8</encoding>			\
	      </encodings>					\
	    </tagspec>						\
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
	const Config	*currentConfig = GetConfiguration();
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

	if	(info.track	> 0) { RenderTagItem("TRACKNUMBER", String(info.track < 10 ? "0" : NIL).Append(String::FromInt(info.track)), buffer);	     numItems++; }
	if	(info.numTracks > 0) { RenderTagItem("TRACKTOTAL", String(info.numTracks < 10 ? "0" : NIL).Append(String::FromInt(info.numTracks)), buffer); numItems++; }

	if	(info.disc	> 0) { RenderTagItem("DISCNUMBER", String(info.disc < 10 ? "0" : NIL).Append(String::FromInt(info.disc)), buffer);	     numItems++; }
	if	(info.numDiscs	> 0) { RenderTagItem("DISCTOTAL", String(info.numDiscs < 10 ? "0" : NIL).Append(String::FromInt(info.numDiscs)), buffer);    numItems++; }

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	{ RenderTagItem("COMMENT", info.comment, buffer);						  numItems++; }
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL && numItems > 0)		{ RenderTagItem("COMMENT", currentConfig->GetStringValue("Tags", "DefaultComment", NIL), buffer); numItems++; }

	/* Save other text info.
	 */
	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":") + 1);
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

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
		/* This is the official way to store cover art in Vorbis
		 * comments. It is used by most newer software.
		 */
		foreach (const Picture &picInfo, track.pictures)
		{
			Buffer<UnsignedByte>	 picBuffer((picInfo.mime	!= NIL ? strlen(picInfo.mime)	     : 0) +
							   (picInfo.description != NIL ? strlen(picInfo.description) : 0) + picInfo.data.Size() + 32);
			OutStream		 picOut(STREAM_BUFFER, picBuffer, picBuffer.Size());

			picOut.OutputNumberRaw(picInfo.type, 4);

			picOut.OutputNumberRaw(picInfo.mime != NIL ? strlen(picInfo.mime) : 0, 4);
			picOut.OutputString(picInfo.mime);

			picOut.OutputNumberRaw(picInfo.description != NIL ? strlen(picInfo.description) : 0, 4);
			picOut.OutputString(picInfo.description);

			picOut.OutputNumberRaw(0, 4);
			picOut.OutputNumberRaw(0, 4);
			picOut.OutputNumberRaw(0, 4);
			picOut.OutputNumberRaw(0, 4);

			picOut.OutputNumberRaw(picInfo.data.Size(), 4);
			picOut.OutputData(picInfo.data, picInfo.data.Size());

			picOut.Close();

			RenderTagItem("METADATA_BLOCK_PICTURE", Encoding::Base64(picBuffer).Encode(), buffer);

			numItems += 1;
		}
	}

	/* Save chapters.
	 */
	if (track.tracks.Length() > 0 && currentConfig->GetIntValue("Tags", "WriteChapters", True))
	{
		Int64	 offset = 0;

		for (Int i = 0; i < track.tracks.Length(); i++)
		{
			const Track	&chapterTrack  = track.tracks.GetNth(i);
			const Info	&chapterInfo   = chapterTrack.GetInfo();
			const Format	&chapterFormat = chapterTrack.GetFormat();

			String	 value	= String(offset / chapterFormat.rate / 60 / 60 < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate / 60 / 60)).Append(":")
					 .Append(offset / chapterFormat.rate / 60 % 60 < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate / 60 % 60)).Append(":")
					 .Append(offset / chapterFormat.rate % 60      < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate % 60)).Append(".")
					 .Append(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) < 100 ?
						(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) <  10 ?  "00" : "0") : "").Append(String::FromInt(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate)));

			{ RenderTagItem(String("CHAPTER").Append(i + 1 < 100 ? (i + 1 < 10 ? "00" : "0") : "").Append(String::FromInt(i + 1)), value, buffer); numItems++; }

			if (chapterInfo.title != NIL)
			{
				{ RenderTagItem(String("CHAPTER").Append(i + 1 < 100 ? (i + 1 < 10 ? "00" : "0") : "").Append(String::FromInt(i + 1)).Append("NAME"), chapterInfo.title, buffer); numItems++; }
			}

			if	(chapterTrack.length	   >= 0) offset += chapterTrack.length;
			else if (chapterTrack.approxLength >= 0) offset += chapterTrack.approxLength;
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
	Int		 size = id.Length() + strlen(value.Trim()) + 5;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(size - 4, 4);
	out.OutputString(id);
	out.OutputNumber('=', 1);
	out.OutputString(value.Trim());

	return Success();
}

Error BoCA::TaggerVorbis::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	const Config	*currentConfig = GetConfiguration();

	InStream	 in(STREAM_BUFFER, buffer, buffer.Size());

	/* Skip vendor string.
	 */
	String	 prevInFormat = String::SetInputFormat("UTF-8");
	Int	 vendorLength = in.InputNumber(4);

	if (vendorLength < 0 || vendorLength > buffer.Size() - 8) return Error();

	in.RelSeek(vendorLength);

	/* Parse individual comment items.
	 */
	Int	 numItems    = in.InputNumber(4);
	Int	 numCovers   = 0;
	Int	 itemsOffset = in.GetPos();
	Info	 info	     = track.GetInfo();

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
		String	 value	 = comment.Tail(comment.Length() - comment.Find("=") - 1).Trim();

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
		else if (id == "METADATA_BLOCK_PICTURE" && currentConfig->GetIntValue("Tags", "CoverArtReadFromTags", True))
		{
			/* This is the official way to store cover art in Vorbis
			 * comments. It is used by most newer software.
			 */
			Picture			 picture;
			Buffer<UnsignedByte>	 buffer;

			Encoding::Base64(buffer).Decode(value);

			InStream		 picIn(STREAM_BUFFER, buffer, buffer.Size());

			picture.type	    = picIn.InputNumberRaw(4);
			picture.mime	    = picIn.InputString(picIn.InputNumberRaw(4));
			picture.description = picIn.InputString(picIn.InputNumberRaw(4));

			picIn.RelSeek(16);

			Int	 dataSize = picIn.InputNumberRaw(4);

			picture.data.Set(buffer + picIn.GetPos(), dataSize);

			if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
			else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
				 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
				 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
				 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

			if (picture.mime != "-->") track.pictures.Add(picture);
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

			if	(numCovers == 0) picture.type = 3; // Cover (front)
			else if (numCovers == 1) picture.type = 4; // Cover (back)
			else			 picture.type = 0; // Other

			track.pictures.Add(picture);

			numCovers++;
		}
	}

	track.SetInfo(info);

	/* Read chapters.
	 */
	in.Seek(itemsOffset);

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
		String	 value	 = comment.Tail(comment.Length() - comment.Find("=") - 1).Trim();

		if (id.StartsWith("CHAPTER") && currentConfig->GetIntValue("Tags", "ReadChapters", True))
		{
			const Format	&format = track.GetFormat();

			Int	 chapter = id.SubString(7, 3).ToInt();
			String	 field	 = id.Tail(id.Length() - 10);

			/* Chapters must appear in order.
			 */
			if (track.tracks.Length() > chapter ||
			    track.tracks.Length() < chapter - 1)
			{
				track.tracks.RemoveAll();

				break;
			}

			/* Fill track data and add to track list.
			 */
			if (track.tracks.Length() == chapter - 1)
			{
				Track	 nTrack;
				Info	 info = track.GetInfo();

				nTrack.origFilename = track.origFilename;
				nTrack.pictures	    = track.pictures;

				info.track = chapter;

				nTrack.SetInfo(info);
				nTrack.SetFormat(format);

				track.tracks.Add(nTrack, chapter);
			}

			Track	&rTrack = track.tracks.GetReference(chapter);

			/* Set track offset.
			 */
			if (field == NIL)
			{
				rTrack.sampleOffset = Math::Round(value.SubString(0, 2).ToInt() * 60 * 60 * format.rate +
								  value.SubString(3, 2).ToInt() * 60	  * format.rate +
								  value.SubString(6, 2).ToInt()		  * format.rate +
								  value.SubString(9, 3).ToInt()		  * format.rate / 1000.0);

				rTrack.length	    = track.length - rTrack.sampleOffset;
				rTrack.fileSize	    = rTrack.length * format.channels * (format.bits / 8);
			}

			/* Set track title.
			 */
			if (field == "NAME")
			{
				Info	 info	= rTrack.GetInfo();

				info.title = value;

				rTrack.SetInfo(info);
			}

			/* Update previous track length.
			 */
			if (chapter > 1)
			{
				Track	&pTrack = track.tracks.GetReference(chapter - 1);

				pTrack.length	= rTrack.sampleOffset - pTrack.sampleOffset;
				pTrack.fileSize	= pTrack.length * format.channels * (format.bits / 8);
			}
		}
	}

	String::SetInputFormat(prevInFormat);

	return Success();
}
