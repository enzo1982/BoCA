 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2023 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

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
	      <coverart supported=\"true\"/>			\
	      <prependzero allowed=\"true\" default=\"true\"/>	\
	      <encodings>					\
		<encoding>UTF-8</encoding>			\
	      </encodings>					\
	    </tagspec>						\
	  </component>						\
								\
	";

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadOggDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
}

const String	 BoCA::TaggerVorbis::ConfigID = "Tags";

BoCA::TaggerVorbis::TaggerVorbis()
{
}

BoCA::TaggerVorbis::~TaggerVorbis()
{
}

Error BoCA::TaggerVorbis::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	/* Get configuration.
	 */
	const Config	*currentConfig		 = GetConfiguration();

	Bool		 prependZero		 = currentConfig->GetIntValue(ConfigID, "TrackPrependZeroVorbisComment", True);

	Bool		 writeChapters		 = currentConfig->GetIntValue(ConfigID, "WriteChapters", True);
	Bool		 writeMCDI		 = currentConfig->GetIntValue(ConfigID, "WriteMCDI", True);

	Bool		 preserveReplayGain	 = currentConfig->GetIntValue(ConfigID, "PreserveReplayGain", True);

	Bool		 albumArtWriteToTags	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToTags", True);
	Bool		 albumArtWriteToVorbis	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToVorbisComment", True);

	Bool		 replaceExistingComments = currentConfig->GetIntValue(ConfigID, "ReplaceExistingComments", False);
	String		 defaultComment		 = currentConfig->GetStringValue(ConfigID, "DefaultComment", NIL);

	/* Set output encoding.
	 */
	String::OutputFormat	 outputFormat("UTF-8");

	/* Save basic information.
	 */
	const Info	&info = track.GetInfo();

	buffer.Resize(4 + (vendorString != NIL ? strlen(vendorString) : 0) + 4);

	Int	 numItems = 0;

	if	(info.artist != NIL) { RenderTagItem("ARTIST",	    info.artist,		  buffer); numItems++; }
	if	(info.title  != NIL) { RenderTagItem("TITLE",	    info.title,			  buffer); numItems++; }
	if	(info.album  != NIL) { RenderTagItem("ALBUM",	    info.album,			  buffer); numItems++; }
	if	(info.year    >   0) { RenderTagItem("DATE",	    String::FromInt(info.year),   buffer); numItems++; }
	if	(info.genre  != NIL) { RenderTagItem("GENRE",	    info.genre,			  buffer); numItems++; }
	if	(info.label  != NIL) { RenderTagItem("PUBLISHER",   info.label,			  buffer); numItems++; }
	if	(info.isrc   != NIL) { RenderTagItem("ISRC",	    info.isrc,			  buffer); numItems++; }

	if	(info.track	> 0) { RenderTagItem("TRACKNUMBER", String(prependZero && info.track	 < 10 ? "0" : NIL).Append(String::FromInt(info.track)),	    buffer); numItems++; }
	if	(info.numTracks > 0) { RenderTagItem("TRACKTOTAL",  String(prependZero && info.numTracks < 10 ? "0" : NIL).Append(String::FromInt(info.numTracks)), buffer); numItems++; }

	if	(info.disc	> 0) { RenderTagItem("DISCNUMBER",  String(prependZero && info.disc	 < 10 ? "0" : NIL).Append(String::FromInt(info.disc)),	    buffer); numItems++; }
	if	(info.numDiscs	> 0) { RenderTagItem("DISCTOTAL",   String(prependZero && info.numDiscs  < 10 ? "0" : NIL).Append(String::FromInt(info.numDiscs)),  buffer); numItems++; }

	if	(info.rating   >= 0) { RenderTagItem("RATING",	    String::FromInt(info.rating), buffer); numItems++; }

	if	(info.comment != NIL && !replaceExistingComments) { RenderTagItem("COMMENT", info.comment,   buffer, False); numItems++; }
	else if (defaultComment != NIL && numItems > 0)		  { RenderTagItem("COMMENT", defaultComment, buffer	  ); numItems++; }

	/* Save other text info.
	 */
	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":"));
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == INFO_ALBUMARTIST)		   { RenderTagItem("ALBUMARTIST",		   value, buffer       ); numItems++; }

		else if	(key == INFO_CONTENTGROUP)		   { RenderTagItem("GROUPING",			   value, buffer       ); numItems++; }
		else if	(key == INFO_SUBTITLE)			   { RenderTagItem("SUBTITLE",			   value, buffer       ); numItems++; }

		else if	(key == INFO_BAND)			   { RenderTagItem("ENSEMBLE",			   value, buffer       ); numItems++; }
		else if	(key == INFO_PERFORMER)			   { RenderTagItem("PERFORMER",			   value, buffer       ); numItems++; }
		else if	(key == INFO_CONDUCTOR)			   { RenderTagItem("CONDUCTOR",			   value, buffer       ); numItems++; }
		else if	(key == INFO_REMIXER)			   { RenderTagItem("REMIXER",			   value, buffer       ); numItems++; }
		else if	(key == INFO_COMPOSER)			   { RenderTagItem("COMPOSER",			   value, buffer       ); numItems++; }
		else if	(key == INFO_LYRICIST)			   { RenderTagItem("LYRICIST",			   value, buffer       ); numItems++; }
		else if	(key == INFO_ARRANGER)			   { RenderTagItem("ARRANGER",			   value, buffer       ); numItems++; }
		else if	(key == INFO_PRODUCER)			   { RenderTagItem("PRODUCER",			   value, buffer       ); numItems++; }
		else if	(key == INFO_ENGINEER)			   { RenderTagItem("ENGINEER",			   value, buffer       ); numItems++; }

		else if	(key == INFO_MOVEMENT)			   { RenderTagItem("MOVEMENT",			   value, buffer       ); numItems++; }
		else if	(key == INFO_MOVEMENTTOTAL)		   { RenderTagItem("MOVEMENTTOTAL",		   value, buffer       ); numItems++; }
		else if	(key == INFO_MOVEMENTNAME)		   { RenderTagItem("MOVEMENTNAME",		   value, buffer       ); numItems++; }

		else if	(key == INFO_BPM)			   { RenderTagItem("BPM",			   value, buffer       ); numItems++; }
		else if	(key == INFO_INITIALKEY)		   { RenderTagItem("INITIALKEY",		   value, buffer       ); numItems++; }

		else if	(key == INFO_COPYRIGHT)			   { RenderTagItem("COPYRIGHT",			   value, buffer       ); numItems++; }

		else if	(key == INFO_MEDIATYPE)			   { RenderTagItem("MEDIA",			   value, buffer       ); numItems++; }
		else if	(key == INFO_CATALOGNUMBER)		   { RenderTagItem("CATALOGNUMBER",		   value, buffer       ); numItems++; }
		else if	(key == INFO_BARCODE)			   { RenderTagItem("BARCODE",			   value, buffer       ); numItems++; }

		else if	(key == INFO_RELEASECOUNTRY)		   { RenderTagItem("RELEASECOUNTRY",		   value, buffer       ); numItems++; }

		else if	(key == INFO_DISCSUBTITLE)		   { RenderTagItem("DISCSUBTITLE",		   value, buffer       ); numItems++; }

		else if	(key == INFO_LYRICS)			   { RenderTagItem("LYRICS",			   value, buffer, False); numItems++; }

		else if	(key == INFO_SCRIPT)			   { RenderTagItem("SCRIPT",			   value, buffer);	  numItems++; }

		else if	(key == INFO_SORT_ARTIST)		   { RenderTagItem("ARTISTSORT",		   value, buffer       ); numItems++; }
		else if	(key == INFO_SORT_ALBUM)		   { RenderTagItem("ALBUMSORT",			   value, buffer       ); numItems++; }
		else if	(key == INFO_SORT_ALBUMARTIST)		   { RenderTagItem("ALBUMARTISTSORT",		   value, buffer       ); numItems++; }
		else if	(key == INFO_SORT_COMPOSER)		   { RenderTagItem("COMPOSERSORT",		   value, buffer       ); numItems++; }
		else if	(key == INFO_SORT_TITLE)		   { RenderTagItem("TITLESORT",			   value, buffer       ); numItems++; }

		else if	(key == INFO_WEB_ARTIST)		   { RenderTagItem("WEBSITE",			   value, buffer       ); numItems++; }

		else if	(key == INFO_ASIN)			   { RenderTagItem("ASIN",			   value, buffer       ); numItems++; }

		else if	(key == INFO_MUSICBRAINZ_ARTISTID)	   { RenderTagItem("MUSICBRAINZ_ARTISTID",	   value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_ALBUMID)	   { RenderTagItem("MUSICBRAINZ_ALBUMID",	   value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_ALBUMARTISTID)	   { RenderTagItem("MUSICBRAINZ_ALBUMARTISTID",	   value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_WORKID)	   { RenderTagItem("MUSICBRAINZ_WORKID",	   value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_DISCID)	   { RenderTagItem("MUSICBRAINZ_DISCID",	   value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_TRACKID)	   { RenderTagItem("MUSICBRAINZ_TRACKID",	   value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_ORIGINALARTISTID) { RenderTagItem("MUSICBRAINZ_ORIGINALARTISTID", value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_ORIGINALALBUMID)  { RenderTagItem("MUSICBRAINZ_ORIGINALALBUMID",  value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_RELEASEGROUPID)   { RenderTagItem("MUSICBRAINZ_RELEASEGROUPID",   value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_RELEASETRACKID)   { RenderTagItem("MUSICBRAINZ_RELEASETRACKID",   value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_TRMID)		   { RenderTagItem("MUSICBRAINZ_TRMID",		   value, buffer       ); numItems++; }

		else if	(key == INFO_MUSICBRAINZ_RELEASETYPE)	   { RenderTagItem("RELEASETYPE",		   value, buffer       ); numItems++; }
		else if	(key == INFO_MUSICBRAINZ_RELEASESTATUS)	   { RenderTagItem("RELEASESTATUS",		   value, buffer       ); numItems++; }
	}

	/* Save Replay Gain info.
	 */
	if (preserveReplayGain)
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
	if (writeMCDI)
	{
		if	(info.mcdi.IsValid()) { RenderTagItem("CDTOC", info.mcdi.GetOffsetString(), buffer); numItems++; }
		else if	(info.offsets != NIL) { RenderTagItem("CDTOC", info.offsets,		    buffer); numItems++; }
	}

	/* Save encoder version.
	 */
	Application	*app = Application::Get();

	{ RenderTagItem("ENCODER", app->getClientName.Call().Append(" ").Append(app->getClientVersion.Call()), buffer);	numItems++; }

	/* Save album art.
	 */
	if (albumArtWriteToTags && albumArtWriteToVorbis)
	{
		/* This is the official way to store album art in Vorbis
		 * comments. It is used by most newer software.
		 */
		foreach (const Picture &picInfo, track.pictures)
		{
			Buffer<UnsignedByte>	 picBuffer;

			CreateMetadataBlockPicture(picBuffer, picInfo, False);

			RenderTagItem("METADATA_BLOCK_PICTURE", Encoding::Base64(picBuffer).Encode(), buffer);

			numItems++;
		}
	}

	/* Save chapters.
	 */
	if (track.tracks.Length() > 0 && writeChapters)
	{
		Int64	 offset = 0;

		for (Int i = 0; i < track.tracks.Length(); i++)
		{
			const Track	&chapterTrack  = track.tracks.GetNth(i);
			const Info	&chapterInfo   = chapterTrack.GetInfo();
			const Format	&chapterFormat = chapterTrack.GetFormat();

			String	 value	= String(offset / chapterFormat.rate / 60 / 60 < 10 ? "0" : NIL).Append(String::FromInt(offset / chapterFormat.rate / 60 / 60)).Append(":")
					 .Append(offset / chapterFormat.rate / 60 % 60 < 10 ? "0" : NIL).Append(String::FromInt(offset / chapterFormat.rate / 60 % 60)).Append(":")
					 .Append(offset / chapterFormat.rate % 60      < 10 ? "0" : NIL).Append(String::FromInt(offset / chapterFormat.rate % 60)).Append(".")
					 .Append(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) < 100 ?
						(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) <  10 ?  "00" : "0") : NIL).Append(String::FromInt(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate)));

			{ RenderTagItem(String("CHAPTER").Append(i + 1 < 100 ? (i + 1 < 10 ? "00" : "0") : NIL).Append(String::FromInt(i + 1)), value, buffer); numItems++; }

			if (chapterInfo.title != NIL)
			{
				{ RenderTagItem(String("CHAPTER").Append(i + 1 < 100 ? (i + 1 < 10 ? "00" : "0") : NIL).Append(String::FromInt(i + 1)).Append("NAME"), chapterInfo.title, buffer); numItems++; }
			}

			if	(chapterTrack.length	   >= 0) offset += chapterTrack.length;
			else if (chapterTrack.approxLength >= 0) offset += chapterTrack.approxLength;
		}
	}

	/* Render tag header.
	 */
	RenderTagHeader(vendorString, numItems, buffer);

	return Success();
}

Int BoCA::TaggerVorbis::RenderTagHeader(const String &vendorString, Int numItems, Buffer<UnsignedByte> &buffer)
{
	Int		 vendorStringSize = vendorString != NIL ? strlen(vendorString) : 0;
	OutStream	 out(STREAM_BUFFER, buffer, 4 + vendorStringSize + 4);

	out.OutputNumber(vendorStringSize, 4);
	out.OutputString(vendorString);
	out.OutputNumber(numItems, 4);

	return Success();
}

Int BoCA::TaggerVorbis::RenderTagItem(const String &id, const String &value, Buffer<UnsignedByte> &buffer, Bool trim)
{
	String		 data     = trim ? value.Trim() : value;
	Int		 dataSize = data != NIL ? strlen(data) : 0;
	Int		 size	  = id.Length() + dataSize + 5;

	buffer.Resize(buffer.Size() + size);

	OutStream	 out(STREAM_BUFFER, buffer + buffer.Size() - size, size);

	out.OutputNumber(size - 4, 4);
	out.OutputString(id);
	out.OutputNumber('=', 1);
	out.OutputString(data);

	return Success();
}

Error BoCA::TaggerVorbis::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	const Config	*currentConfig = GetConfiguration();

	InStream	 in(STREAM_BUFFER, buffer, buffer.Size());

	/* Read vendor string.
	 */
	String::InputFormat	 inputFormat("UTF-8");
	Int			 vendorLength = in.InputNumber(4);

	if (vendorLength < 0 || vendorLength > buffer.Size() - 8) return Error();

	vendorString = in.InputString(vendorLength);

	/* Parse individual comment items.
	 */
	Int	 numItems     = in.InputNumber(4);
	Int	 numAlbumArt  = 0;
	Bool	 haveChapters = False;
	Int	 itemsOffset  = in.GetPos();
	Info	 info	      = track.GetInfo();

	for (Int i = 0; i < numItems; i++)
	{
		/* Read and check next comment string length.
		 */
		Int	 length	 = in.InputNumber(4);

		if (length < 0 || length > buffer.Size() - in.GetPos()) return Error();

		/* Read and assign actual comment string.
		 */
		String	 comment = in.InputString(length);

		String	 id	 = comment.Head(comment.Find("=")).ToUpper();
		String	 value	 = comment.Tail(comment.Length() - comment.Find("=") - 1).Trim();

		if (value == NIL) continue;

		if	(id == "ARTIST")	  info.artist	 = value;
		else if (id == "TITLE")		  info.title	 = value;
		else if (id == "ALBUM")		  info.album	 = value;
		else if (id == "DATE")		  info.year	 = value.ToInt();
		else if (id == "GENRE")		  info.genre	 = value;
		else if (id == "COMMENT")	  info.comment	 = comment.Tail(comment.Length() - comment.Find("=") - 1);

		else if (id == "LABEL"	   ||
			 id == "PUBLISHER" ||
			 id == "ORGANIZATION")	  info.label	 = value;

		else if (id == "ISRC")
		{
			if (Info::IsISRC(value))  info.isrc	 = value;
		}

		else if (id == "TRACKNUMBER")
		{
			info.track = value.ToInt();

			if (value.Contains("/")) info.numTracks = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
		}

		else if (id == "TRACKTOTAL" ||
			 id == "TOTALTRACKS")	  info.numTracks = value.ToInt();

		else if (id == "DISCNUMBER")
		{
			info.disc = value.ToInt();

			if (value.Contains("/")) info.numDiscs  = value.Tail(value.Length() - value.Find("/") - 1).ToInt();
		}

		else if (id == "DISCTOTAL" ||
			 id == "TOTALDISCS")	  info.numDiscs  = value.ToInt();

		else if (id == "RATING")	  info.rating	 = Math::Min(100, value.ToInt());

		else if (id == "ALBUMARTIST")	  info.SetOtherInfo(INFO_ALBUMARTIST,	   value);

		else if (id == "GROUPING")	  info.SetOtherInfo(INFO_CONTENTGROUP,	   value);
		else if (id == "SUBTITLE")	  info.SetOtherInfo(INFO_SUBTITLE,	   value);

		else if (id == "BAND"	  ||
			 id == "ENSEMBLE" ||
			 id == "ORCHESTRA")	  info.SetOtherInfo(INFO_BAND,		   value);

		else if (id == "PERFORMER")	  info.SetOtherInfo(INFO_PERFORMER,	   value);
		else if (id == "CONDUCTOR")	  info.SetOtherInfo(INFO_CONDUCTOR,	   value);
		else if (id == "REMIXER")	  info.SetOtherInfo(INFO_REMIXER,	   value);
		else if (id == "COMPOSER")	  info.SetOtherInfo(INFO_COMPOSER,	   value);
		else if (id == "LYRICIST")	  info.SetOtherInfo(INFO_LYRICIST,	   value);
		else if (id == "ARRANGER")	  info.SetOtherInfo(INFO_ARRANGER,	   value);
		else if (id == "PRODUCER")	  info.SetOtherInfo(INFO_PRODUCER,	   value);
		else if (id == "ENGINEER")	  info.SetOtherInfo(INFO_ENGINEER,	   value);

		else if (id == "MOVEMENT" ||
			 id == "PART")
		{
			info.SetOtherInfo(INFO_MOVEMENT, String::FromInt(value.ToInt()));

			if (value.Contains("/")) info.SetOtherInfo(INFO_MOVEMENTTOTAL, String::FromInt(value.Tail(value.Length() - value.Find("/") - 1).ToInt()));
		}

		else if (id == "MOVEMENTTOTAL"	||
			 id == "TOTALMOVEMENTS"	||
			 id == "PARTTOTAL"	||
			 id == "TOTALPARTS")	  info.SetOtherInfo(INFO_MOVEMENTTOTAL,	   value);

		else if (id == "MOVEMENTNAME" ||
			 id == "PARTNAME")	  info.SetOtherInfo(INFO_MOVEMENTNAME,	   value);

		else if (id == "BPM")
		{
			if (value.ToInt() > 0) info.SetOtherInfo(INFO_BPM, value);
		}

		else if (id == "INITIALKEY")	  info.SetOtherInfo(INFO_INITIALKEY,	   value);

		else if (id == "COPYRIGHT")	  info.SetOtherInfo(INFO_COPYRIGHT,	   value);

		else if (id == "MEDIA"	     ||
			 id == "SOURCE"	     ||
			 id == "SOURCEMEDIA" ||
			 id == "SOURCE MEDIUM")	  info.SetOtherInfo(INFO_MEDIATYPE,	   value);

		else if (id == "LABELNO"   ||
			 id == "CATALOG"   ||
			 id == "CATALOGID" ||
			 id == "CATALOGUE" ||
			 id == "CATALOGNUMBER")	  info.SetOtherInfo(INFO_CATALOGNUMBER,	   value);

		else if (id == "BARCODE"       ||
			 id == "EAN"	       ||
			 id == "EAN/UPC"       ||
			 id == "EAN/UPN"       ||
			 id == "UPC"	       ||
			 id == "UPN"	       ||
			 id == "PRODUCTNUMBER")	  info.SetOtherInfo(INFO_BARCODE,	   value);

		else if (id == "RELEASECOUNTRY")  info.SetOtherInfo(INFO_RELEASECOUNTRY,   value);

		else if (id == "DISCSUBTITLE")	  info.SetOtherInfo(INFO_DISCSUBTITLE,	   value);

		else if (id == "LYRICS")	  info.SetOtherInfo(INFO_LYRICS,	   comment.Tail(comment.Length() - comment.Find("=") - 1));

		else if (id == "SCRIPT")	  info.SetOtherInfo(INFO_SCRIPT,	   value);

		else if (id == "ARTISTSORT")	  info.SetOtherInfo(INFO_SORT_ARTIST,	   value);
		else if (id == "ALBUMSORT")	  info.SetOtherInfo(INFO_SORT_ALBUM,	   value);
		else if (id == "ALBUMARTISTSORT") info.SetOtherInfo(INFO_SORT_ALBUMARTIST, value);
		else if (id == "COMPOSERSORT")	  info.SetOtherInfo(INFO_SORT_COMPOSER,	   value);
		else if (id == "TITLESORT")	  info.SetOtherInfo(INFO_SORT_TITLE,	   value);

		else if (id == "WEBSITE")	  info.SetOtherInfo(INFO_WEB_ARTIST,	   value);

		else if (id == "ASIN")		  info.SetOtherInfo(INFO_ASIN,		   value);

		else if (id.StartsWith("MUSICBRAINZ"))
		{
			if	(id == "MUSICBRAINZ_ARTISTID")	       info.SetOtherInfo(INFO_MUSICBRAINZ_ARTISTID,	    value);
			else if	(id == "MUSICBRAINZ_ALBUMID")	       info.SetOtherInfo(INFO_MUSICBRAINZ_ALBUMID,	    value);
			else if	(id == "MUSICBRAINZ_ALBUMARTISTID")    info.SetOtherInfo(INFO_MUSICBRAINZ_ALBUMARTISTID,    value);
			else if	(id == "MUSICBRAINZ_WORKID")	       info.SetOtherInfo(INFO_MUSICBRAINZ_WORKID,	    value);
			else if	(id == "MUSICBRAINZ_DISCID")	       info.SetOtherInfo(INFO_MUSICBRAINZ_DISCID,	    value);
			else if	(id == "MUSICBRAINZ_TRACKID")	       info.SetOtherInfo(INFO_MUSICBRAINZ_TRACKID,	    value);
			else if	(id == "MUSICBRAINZ_ORIGINALARTISTID") info.SetOtherInfo(INFO_MUSICBRAINZ_ORIGINALARTISTID, value);
			else if	(id == "MUSICBRAINZ_ORIGINALALBUMID")  info.SetOtherInfo(INFO_MUSICBRAINZ_ORIGINALALBUMID,  value);
			else if	(id == "MUSICBRAINZ_RELEASEGROUPID")   info.SetOtherInfo(INFO_MUSICBRAINZ_RELEASEGROUPID,   value);
			else if	(id == "MUSICBRAINZ_RELEASETRACKID")   info.SetOtherInfo(INFO_MUSICBRAINZ_RELEASETRACKID,   value);
			else if	(id == "MUSICBRAINZ_TRMID")	       info.SetOtherInfo(INFO_MUSICBRAINZ_TRMID,	    value);
		}

		else if	(id == "RELEASETYPE")	  info.SetOtherInfo(INFO_MUSICBRAINZ_RELEASETYPE,   value);
		else if	(id == "RELEASESTATUS")	  info.SetOtherInfo(INFO_MUSICBRAINZ_RELEASESTATUS, value);

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
		else if (id == "METADATA_BLOCK_PICTURE")
		{
			if (currentConfig->GetIntValue(ConfigID, "CoverArtReadFromTags", True))
			{
				/* This is the official way to store album art in Vorbis
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

				if (picture.mime != "-->" && picture.data.Size() >= 16)
				{
					if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
					else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
						 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
						 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
						 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

					if (picture.data[0] != 0 && picture.data[1] != 0) track.pictures.Add(picture);
				}
			}
		}
		else if (id == "COVERART")
		{
			if (currentConfig->GetIntValue(ConfigID, "CoverArtReadFromTags", True))
			{
				/* This is an unofficial way to store album art in Vorbis
				 * comments. It is used by some existing software.
				 */
				Picture			 picture;
				Buffer<UnsignedByte>	 buffer;

				Encoding::Base64(buffer).Decode(value);

				if	(numAlbumArt == 0) picture.type = 3; // Cover (front)
				else if (numAlbumArt == 1) picture.type = 4; // Cover (back)
				else			   picture.type = 0; // Other

				picture.data = buffer;

				if (picture.data.Size() >= 16)
				{
					if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
					else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
						 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
						 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
						 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

					if (picture.data[0] != 0 && picture.data[1] != 0) track.pictures.Add(picture);
				}

				numAlbumArt++;
			}
		}
		else if (id == "CUESHEET")
		{
			if (currentConfig->GetIntValue(ConfigID, "ReadEmbeddedCueSheets", True))
			{
				String::OutputFormat	 outputFormat("UTF-8");
 
				/* Output cuesheet to temporary file.
				 */
				String		 cuesheet = value.Replace("\r\n", "\n");
				String		 cueFile  = S::System::System::GetTempDirectory().Append("cuesheet_temp_").Append(String::FromInt(track.fileName.ComputeCRC32())).Append(".cue");
				OutStream	 out(STREAM_FILE, cueFile, OS_REPLACE);

				/* Write UTF-8 BOM.
				 */
				if (value[0] != 0xFEFF) out.OutputNumberRaw(0xEFBBBF, 3);

				/* Write cuesheet line by line.
				 */
				const Array<String>	&lines = cuesheet.Explode("\n");

				out.OutputLine(String("FILE \"").Append(track.fileName).Append("\" WAVE"));

				foreach (const String &line, lines)
				{
					if (!line.Trim().StartsWith("FILE")) out.OutputLine(line);
				}

				out.Close();

				/* Get cue sheet stream info.
				 */
				AS::Registry		&boca	 = AS::Registry::Get();
				AS::DecoderComponent	*decoder = (AS::DecoderComponent *) boca.CreateComponentByID("cuesheet-dec");

				if (decoder != NIL)
				{
					Track	 cueTrack;
					Config	*cueConfig = Config::Copy(GetConfiguration());

					cueConfig->SetIntValue("Tags", "ReadChapters", False);
					cueConfig->SetIntValue("Tags", "ReadEmbeddedCueSheets", False);

					cueConfig->SetIntValue("CueSheet", "ReadInformationTags", True);
					cueConfig->SetIntValue("CueSheet", "PreferCueSheets", True);
					cueConfig->SetIntValue("CueSheet", "LookForAlternativeFiles", False);
					cueConfig->SetIntValue("CueSheet", "IgnoreErrors", False);

					decoder->SetConfiguration(cueConfig);
					decoder->GetStreamInfo(cueFile, cueTrack);

					boca.DeleteComponent(decoder);

					Config::Free(cueConfig);

					if (cueTrack.tracks.Length() > 0) track.tracks = cueTrack.tracks;
				}

				File(cueFile).Delete();
			}
		}
		else if (id.StartsWith("CHAPTER"))
		{
			/* Chapters are processed further down.
			 */
			haveChapters = True;
		}
		else
		{
			/* Save any other tags as user defined text.
			 */
			 info.other.Add(String(INFO_USERTEXT).Append(":").Append(id).Append(":|:").Append(value));
		}
	}

	/* Set artist to album artist if artist is not filled.
	 */
	if (info.artist == NIL) info.artist = info.GetOtherInfo(INFO_ALBUMARTIST);

	/* Remove sub-tracks if main track has a track number.
	 */
	if (info.track > 0) track.tracks.RemoveAll();

	track.SetInfo(info);

	/* Read chapters.
	 */
	if (haveChapters && currentConfig->GetIntValue(ConfigID, "ReadChapters", True) &&
			  (!currentConfig->GetIntValue(ConfigID, "PreferCueSheetsToChapters", True) || track.tracks.Length() == 0))
	{
		track.tracks.RemoveAll();

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

			if (id.StartsWith("CHAPTER"))
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

					nTrack.fileName = track.fileName;
					nTrack.pictures = track.pictures;

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

					if	(track.length	    > 0) rTrack.fileSize = Math::Round(Float(track.fileSize) / track.length * rTrack.length);
					else if (track.approxLength > 0) rTrack.fileSize = Math::Round(Float(track.fileSize) / track.approxLength * rTrack.length);
					else				 rTrack.fileSize = rTrack.length * format.channels * (format.bits / 8);
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

					if	(track.length	    > 0) pTrack.fileSize = Math::Round(Float(track.fileSize) / track.length * pTrack.length);
					else if (track.approxLength > 0) pTrack.fileSize = Math::Round(Float(track.fileSize) / track.approxLength * pTrack.length);
					else				 pTrack.fileSize = pTrack.length * format.channels * (format.bits / 8);
				}
			}
		}
	}

	return Success();
}

Error BoCA::TaggerVorbis::RenderStreamInfo(const String &fileName, const Track &track)
{
	return UpdateStreamInfo(fileName, track);
}

Error BoCA::TaggerVorbis::UpdateStreamInfo(const String &fileName, const Track &track)
{
	/* Get configuration.
	 */
	const Config	*currentConfig	     = GetConfiguration();

	Bool		 albumArtWriteToTags = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToTags", True);

	/* Open input file.
	 */
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	if (in.InputString(4) != "OggS") return False;

	in.Seek(0);

	/* Create temporary file.
	 */
	OutStream	 out(STREAM_FILE, fileName.Append(".temp"), OS_REPLACE);

	if (out.GetLastError() != IO_ERROR_OK) return Error();

	/* Set up Ogg state.
	 */
	ogg_sync_state		 oy;
	ogg_stream_state	 os_in;
	ogg_stream_state	 os_out;

	ex_ogg_sync_init(&oy);

	/* Get stream format.
	 */
	Bool	 initialized   = False;
	Bool	 error	       = False;
	Int	 packetNum     = 0;
	Bool	 wrotePictures = False;

	Bool	 isVorbis      = False;
	Bool	 isOpus	       = False;
	Bool	 isSpeex       = False;
	Bool	 isFLAC	       = False;

	do
	{
		Int	 size	= Math::Min(Int64(4096), in.Size() - in.GetPos());
		char	*buffer	= ex_ogg_sync_buffer(&oy, size);

		in.InputData(buffer, size);

		ex_ogg_sync_wrote(&oy, size);

		ogg_page	 og;

		while (ex_ogg_sync_pageout(&oy, &og) == 1)
		{
			if (!initialized)
			{
				ex_ogg_stream_init(&os_in, ex_ogg_page_serialno(&og));
				ex_ogg_stream_init(&os_out, ex_ogg_page_serialno(&og));

				initialized = True;
			}

			ex_ogg_stream_pagein(&os_in, &og);

			ogg_packet	 op;

			while (ex_ogg_stream_packetout(&os_in, &op) == 1)
			{
				/* Found header packet.
				 */
				if (packetNum == 0)
				{
					if	(op.packet[0] == 0x01 && op.packet[1] == 'v' && op.packet[2] == 'o' && op.packet[3] == 'r' && op.packet[4] == 'b' && op.packet[5] == 'i' && op.packet[6] == 's')			isVorbis = True;
					else if (op.packet[0] ==  'O' && op.packet[1] == 'p' && op.packet[2] == 'u' && op.packet[3] == 's' && op.packet[4] == 'T' && op.packet[5] == 'a' && op.packet[6] == 'g' && op.packet[7] == 's') isOpus	 = True;
					else if (op.packet[0] ==  'S' && op.packet[1] == 'p' && op.packet[2] == 'e' && op.packet[3] == 'e' && op.packet[4] == 'x')									isSpeex	 = True;
					else if (op.packet[0] == 0x7F && op.packet[1] == 'F' && op.packet[2] == 'L' && op.packet[3] == 'A' && op.packet[4] == 'C')									isFLAC	 = True;

					if (!isVorbis && !isOpus && !isSpeex && !isFLAC) { error = True; break; }

					ex_ogg_stream_packetin(&os_out, &op);
				}

				/* Found Vorbis Comment packet.
				 */
				if (packetNum == 1)
				{
					Int	 offset = 0;

					if	(isVorbis) offset = 7;
					else if (isOpus)   offset = 8;
					else if (isFLAC)   offset = 4;

					Track			 inputTrack;
					Buffer<UnsignedByte>	 comments(op.bytes - offset);

					memcpy(comments, op.packet + offset, op.bytes - offset);

					if (ParseBuffer(comments, inputTrack) != Success()) { error = True; break; }

					/* For FLAC, pictures are stored separately.
					 */
					Track	 copy = track;

					if (isFLAC) copy.pictures.RemoveAll();

					RenderBuffer(comments, copy);

					/* Output updated packet.
					 */
					comments.Resize(comments.Size() + offset + (isVorbis ? 1 : 0));

					memmove(comments + offset, comments, comments.Size() - offset);
					memcpy(comments, op.packet, offset);

					if	(isVorbis) comments[comments.Size() - 1] = 1;
					else if (isFLAC)   OutStream(STREAM_BUFFER, comments + 1, 3).OutputNumberRaw(comments.Size() - 4, 3);

					ogg_packet	 header_comm = { comments, comments.Size(), 0, 0, 0, 0 };

					ex_ogg_stream_packetin(&os_out, &header_comm);
				}

				/* Other packet.
				 */
				if (packetNum >= 2)
				{
					/* Pass packet to output stream unless it's a FLAC picture packet.
					 */
					if (!isFLAC || op.packet[0] != 0x06) ex_ogg_stream_packetin(&os_out, &op);

					/* Save picture packets to FLAC if not already done.
					 */
					if (isFLAC && op.packet[0] == 0x06 && albumArtWriteToTags && !wrotePictures)
					{
						foreach (const Picture &picInfo, track.pictures)
						{
							Buffer<UnsignedByte>	 picBuffer;

							CreateMetadataBlockPicture(picBuffer, picInfo, True);

							ogg_packet	 meta_pic = { picBuffer, picBuffer.Size(), 0, 0, 0, 0 };

							ex_ogg_stream_packetin(&os_out, &meta_pic);

							WriteOggPackets(os_out, out);
						}

						wrotePictures = True;
					}
				}

				packetNum++;
			}

			if (error) break;

			WriteOggPackets(os_out, out);
		}

		if (error) break;
	}
	while (in.GetPos() < in.Size());

	ex_ogg_stream_clear(&os_in);
	ex_ogg_stream_clear(&os_out);

	ex_ogg_sync_clear(&oy);

	in.Close();
	out.Close();

	if (error)
	{
		File(fileName.Append(".temp")).Delete();

		return Error();
	}

	File(fileName).Delete();
	File(fileName.Append(".temp")).Move(fileName);

	return Success();
}

Void BoCA::TaggerVorbis::CreateMetadataBlockPicture(Buffer<UnsignedByte> &buffer, const Picture &picture, Bool withHeader)
{
	buffer.Resize((picture.mime	   != NIL ? strlen(picture.mime)	: 0) +
		      (picture.description != NIL ? strlen(picture.description) : 0) + picture.data.Size() + 32 + (withHeader ? 4 : 0));

	OutStream	 out(STREAM_BUFFER, buffer, buffer.Size());

	if (withHeader)
	{
		out.OutputNumberRaw(0x06, 1);
		out.OutputNumberRaw(buffer.Size() - 4, 3);
	}

	out.OutputNumberRaw(picture.type, 4);

	out.OutputNumberRaw(picture.mime != NIL ? strlen(picture.mime) : 0, 4);
	out.OutputString(picture.mime);

	out.OutputNumberRaw(picture.description != NIL ? strlen(picture.description) : 0, 4);
	out.OutputString(picture.description);

	out.OutputNumberRaw(0, 4);
	out.OutputNumberRaw(0, 4);
	out.OutputNumberRaw(0, 4);
	out.OutputNumberRaw(0, 4);

	out.OutputNumberRaw(picture.data.Size(), 4);
	out.OutputData(picture.data, picture.data.Size());
}

Void BoCA::TaggerVorbis::WriteOggPackets(ogg_stream_state &os, OutStream &out)
{
	do
	{
		ogg_page	 og;

		if (ex_ogg_stream_flush(&os, &og) == 0) break;

		out.OutputData(og.header, og.header_len);
		out.OutputData(og.body, og.body_len);
	}
	while (True);
}
