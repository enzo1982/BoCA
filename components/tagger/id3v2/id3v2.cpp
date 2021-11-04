 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

#include "id3v2.h"

using namespace smooth::IO;

const String &BoCA::TaggerID3v2::GetComponentSpecs()
{
	static String	 componentSpecs = "			\
								\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
	  <component>						\
	    <name>ID3v2 Tagger</name>				\
	    <version>1.0</version>				\
	    <id>id3v2-tag</id>					\
	    <type>tagger</type>					\
	    <format>						\
	      <name>MPEG 1 Audio Layer 3</name>			\
	      <extension>mp3</extension>			\
	    </format>						\
	    <tagspec>						\
	      <name>ID3v2</name>				\
	      <coverart supported=\"true\"/>			\
	      <prependzero allowed=\"true\" default=\"false\"/>	\
	      <encodings free=\"true\">				\
		<encoding>ISO-8859-1</encoding>			\
		<encoding>UTF-8</encoding>			\
		<encoding default=\"true\">UTF-16LE</encoding>	\
		<encoding>UTF-16BE</encoding>			\
	      </encodings>					\
	    </tagspec>						\
	  </component>						\
								\
	";

	return componentSpecs;
}

const String	 BoCA::TaggerID3v2::ConfigID = "Tags";

const String	 BoCA::TaggerID3v2::genres[192] =
      { "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip-Hop", "Jazz",
	"Metal", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno",
	"Industrial", "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack", "Euro-Techno",
	"Ambient", "Trip-Hop", "Vocal", "Jazz+Funk", "Fusion", "Trance", "Classical", "Instrumental",
	"Acid", "House", "Game", "Sound Clip", "Gospel", "Noise", "Alt. Rock", "Bass", "Soul",
	"Punk", "Space", "Meditative", "Instrumental Pop", "Instrumental Rock", "Ethnic", "Gothic",
	"Darkwave", "Techno-Industrial", "Electronic", "Pop-Folk", "Eurodance", "Dream", "Southern Rock",
	"Comedy", "Cult", "Gangsta Rap", "Top 40", "Christian Rap", "Pop/Funk", "Jungle",
	"Native American", "Cabaret", "New Wave", "Psychedelic", "Rave", "Showtunes", "Trailer",
	"Lo-Fi", "Tribal", "Acid Punk", "Acid Jazz", "Polka", "Retro", "Musical", "Rock & Roll",
	"Hard Rock", "Folk", "Folk/Rock", "National Folk", "Swing", "Fast-Fusion", "Bebob", "Latin",
	"Revival", "Celtic", "Bluegrass", "Avantgarde", "Gothic Rock", "Progressive Rock",
	"Psychedelic Rock", "Symphonic Rock", "Slow Rock", "Big Band", "Chorus", "Easy Listening",
	"Acoustic", "Humour", "Speech", "Chanson", "Opera", "Chamber Music", "Sonata", "Symphony",
	"Booty Bass", "Primus", "Porn Groove", "Satire", "Slow Jam", "Club", "Tango", "Samba",
	"Folklore", "Ballad", "Power Ballad", "Rhythmic Soul", "Freestyle", "Duet", "Punk Rock",
	"Drum Solo", "A Capella", "Euro-House", "Dance Hall", "Goa", "Drum & Bass", "Club-House",
	"Hardcore", "Terror", "Indie", "BritPop", "Negerpunk", "Polsk Punk", "Beat",
	"Christian Gangsta Rap", "Heavy Metal", "Black Metal", "Crossover", "Contemporary Christian",
	"Christian Rock", "Merengue", "Salsa", "Thrash Metal", "Anime", "JPop", "Synthpop", "Abstract",
	"Art Rock", "Baroque", "Bhangra", "Big Beat", "Breakbeat", "Chillout", "Downtempo", "Dub", "EBM",
	"Eclectic", "Electro", "Electroclash", "Emo", "Experimental", "Garage", "Global", "IDM",
	"Illbient", "Industro-Goth", "Jam Band", "Krautrock", "Leftfield", "Lounge", "Math Rock",
	"New Romantic", "Nu-Breakz", "Post-Punk", "Post-Rock", "Psytrance", "Shoegaze", "Space Rock",
	"Trop Rock", "World Music", "Neoclassical", "Audiobook", "Audio Theatre", "Neue Deutsche Welle",
	"Podcast", "Indie Rock", "G-Funk", "Dubstep", "Garage Rock", "Psybient" };

BoCA::TaggerID3v2::TaggerID3v2()
{
	textEncoding = ID3TE_NONE;
}

BoCA::TaggerID3v2::~TaggerID3v2()
{
}

Error BoCA::TaggerID3v2::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	ID3_Tag	 tag;

	RenderContainer(tag, track);

	tag.SetPadding(false);

	/* ID3Lib versions used in many Linux distributions are buggy
	 * and return less than the actually used tag size here.
	 *
	 * Round to the next multiple of 2048 to compensate for that.
	 */
	buffer.Resize(((tag.Size() / 2048) + 1) * 2048);

	Int	 size = tag.Render(buffer, ID3TT_ID3V2);

	buffer.Resize(size);

	return Success();
}

Error BoCA::TaggerID3v2::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	ID3_Tag	 tag;

	tag.Parse(buffer, buffer.Size());

	ParseContainer(tag, track);

	return Success();
}

Error BoCA::TaggerID3v2::ParseStreamInfo(const String &fileName, Track &track)
{
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	/* Look for ID3v2 tag.
	 */
	if (in.InputString(3) == "ID3" && in.InputNumber(1) <= 4)
	{
		/* Skip minor version and flags.
		 */
		in.InputNumber(1);
		in.InputNumber(1);

		/* Read tag size as a 4 byte unsynchronized integer.
		 */
		Int	 tagSize = (in.InputNumber(1) << 21) +
				   (in.InputNumber(1) << 14) +
				   (in.InputNumber(1) <<  7) +
				   (in.InputNumber(1)      );

		in.Seek(0);

		Buffer<UnsignedByte>	 buffer(tagSize + 10);

		in.InputData(buffer, buffer.Size());

		return ParseBuffer(buffer, track);
	}

	return Error();
}

Error BoCA::TaggerID3v2::UpdateStreamInfo(const String &fileName, const Track &track)
{
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	/* Check for RIFF or AIFF files.
	 */
	String	 magic = in.InputString(4);

	if (magic == "RIFF" || magic == "FORM")
	{
		in.Close();

		return UpdateChunkedFileTag(fileName, track);
	}

	in.Seek(0);

	/* Look for ID3v2 tag.
	 */
	if (in.InputString(3) == "ID3" && in.InputNumber(1) <= 4)
	{
		/* Skip minor version and flags.
		 */
		in.InputNumber(1);
		in.InputNumber(1);

		/* Read tag size as a 4 byte unsynchronized integer.
		 */
		Int	 tagSize = (in.InputNumber(1) << 21) +
				   (in.InputNumber(1) << 14) +
				   (in.InputNumber(1) <<  7) +
				   (in.InputNumber(1)      );

		/* Skip the tag.
		 */
		in.RelSeek(tagSize);
	}
	else
	{
		in.Seek(0);
	}

	/* Copy to temporary file and write tag.
	 */
	OutStream	 out(STREAM_FILE, fileName.Append(".temp"), OS_REPLACE);

	if (out.GetLastError() == IO_ERROR_OK)
	{
		Buffer<UnsignedByte>	 buffer;

		RenderBuffer(buffer, track);

		out.OutputData(buffer, buffer.Size());

		buffer.Resize(128 * 1024);

		for (Int64 i = in.GetPos(); i < in.Size(); i += buffer.Size())
		{
			Int	 bytes = Math::Min(in.Size() - i, buffer.Size());

			in.InputData(buffer, bytes);
			out.OutputData(buffer, bytes);
		}

		in.Close();
		out.Close();

		File(fileName).Delete();
		File(fileName.Append(".temp")).Move(fileName);

		return Success();
	}

	return Error();
}

Error BoCA::TaggerID3v2::UpdateChunkedFileTag(const String &fileName, const Track &track)
{
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	/* Read RIFF chunk.
	 */
	Bool		 error = False;
	String		 riff  = in.InputString(4);

	if (riff != "RIFF" && riff != "RF64" && riff != "FORM") return Error();

	/* Create output file.
	 */
	OutStream	 out(STREAM_FILE, fileName.Append(".temp"), OS_REPLACE);

	UnsignedInt32	 rSize	 = riff != "FORM" ? in.InputNumber(4) : in.InputNumberRaw(4);
	UnsignedInt32	 rFormat = in.InputNumber(4);

	out.OutputString(riff);

	if (riff != "FORM") out.OutputNumber(rSize, 4);
	else		    out.OutputNumberRaw(rSize, 4);

	out.OutputNumber(rFormat, 4);

	Int64			 dSize = -1;
	Buffer<UnsignedByte>	 buffer(131072);

	while (!error)
	{
		if (in.GetPos() >= in.Size()) break;

		/* Read next chunk.
		 */
		String		 chunk = in.InputString(4);
		UnsignedInt64	 cSize = (UnsignedInt32) (riff != "FORM" ? in.InputNumber(4) : in.InputNumberRaw(4));

		if ((riff != "FORM" && chunk == "id3 ") ||
		    (riff == "FORM" && chunk == "ID3 "))
		{
			/* Skip ID3 chunk.
			 */
			if (!in.RelSeek(cSize + cSize % 2)) error = True;

			continue;
		}

		out.OutputString(chunk);

		if (riff != "FORM") out.OutputNumber(cSize, 4);
		else		    out.OutputNumberRaw(cSize, 4);

		if (chunk == "ds64")
		{
			in.RelSeek(8);

			dSize = in.InputNumber(8);

			in.RelSeek(-16);
		}
		else if (chunk == "data")
		{
			if (rSize == 0xFFFFFFFF || rSize == 0 ||
			    cSize == 0xFFFFFFFF || cSize == 0) error = True;

			if (dSize >= 0) cSize = dSize;
		}

		/* Write chunk data to output file.
		 */
		cSize += cSize % 2;

		while (!error && cSize > 0)
		{
			Int64	 bytes = Math::Min(cSize, buffer.Size());

			in.InputData(buffer, bytes);
			out.OutputData(buffer, bytes);

			cSize -= bytes;
		}
	}

	/* Write new metadata.
	 */
	if (!error)
	{
		/* Write ID3 chunk.
		 */
		buffer.Resize(0);

		RenderBuffer(buffer, track);

		if (riff != "FORM") out.OutputString("id3 ");
		else		    out.OutputString("ID3 ");

		Int	 size = buffer.Size();

		if (riff != "FORM") out.OutputNumber(size, 4);
		else		    out.OutputNumberRaw(size, 4);

		out.OutputData(buffer, size);

		/* Update file size.
		 */
		UnsignedInt64	 fileSize = out.GetPos() - 8;

		rSize = Math::Min(0xFFFFFFFF, fileSize);

		out.Seek(4);

		if (riff != "FORM") out.OutputNumber(rSize, 4);
		else		    out.OutputNumberRaw(rSize, 4);
	}

	/* Clean up.
	 */
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

Int BoCA::TaggerID3v2::RenderContainer(ID3_Container &container, const Track &track, Bool isChapter)
{
	/* Get configuration.
	 */
	const Config	*currentConfig		 = GetConfiguration();

	Bool		 prependZero		 = currentConfig->GetIntValue(ConfigID, "TrackPrependZeroID3v2", False);

	Bool		 writeChapters		 = currentConfig->GetIntValue(ConfigID, "WriteChapters", True);
	Bool		 writeMCDI		 = currentConfig->GetIntValue(ConfigID, "WriteMCDI", True);

	Bool		 preserveReplayGain	 = currentConfig->GetIntValue(ConfigID, "PreserveReplayGain", True);

	Bool		 coverArtWriteToTags	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToTags", True);
	Bool		 coverArtWriteToID3v2	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToID3v2", True);

	Bool		 replaceExistingComments = currentConfig->GetIntValue(ConfigID, "ReplaceExistingComments", False);
	String		 defaultComment		 = currentConfig->GetStringValue(ConfigID, "DefaultComment", NIL);

	/* Set text encoding.
	 */
	textEncodingID = currentConfig->GetStringValue(ConfigID, "ID3v2Encoding", "UTF-16LE");

	if	(textEncodingID == "UTF-8")				      textEncoding = ID3TE_UTF8;
	else if (textEncodingID == "ISO-8859-1")			      textEncoding = ID3TE_ISO8859_1;
	else if (textEncodingID == "UTF-16"   || textEncodingID == "UCS-2" ||
		 textEncodingID == "UTF-16LE" || textEncodingID == "UCS-2LE") textEncoding = ID3TE_UTF16;
	else if (textEncodingID == "UTF-16BE" || textEncodingID == "UCS-2BE") textEncoding = ID3TE_UTF16BE;

	/* Set ID3v2 version according to encoding.
	 */
	if (textEncodingID == "UTF-16BE" || textEncodingID == "UTF-8") container.SetSpec(ID3V2_4_0);
	else							       container.SetSpec(ID3V2_3_0);

	/* Save basic information.
	 */
	Info	 info = track.GetInfo();

	if (info.artist != NIL) { ID3_Frame frame(ID3FID_LEADARTIST);  SetStringField(frame, ID3FN_TEXT, info.artist); container.AddFrame(frame); }
	if (info.title  != NIL) { ID3_Frame frame(ID3FID_TITLE);       SetStringField(frame, ID3FN_TEXT, info.title);  container.AddFrame(frame); }
	if (info.album  != NIL) { ID3_Frame frame(ID3FID_ALBUM);       SetStringField(frame, ID3FN_TEXT, info.album);  container.AddFrame(frame); }
	if (info.genre  != NIL) { ID3_Frame frame(ID3FID_CONTENTTYPE); SetStringField(frame, ID3FN_TEXT, info.genre);  container.AddFrame(frame); }
	if (info.label  != NIL) { ID3_Frame frame(ID3FID_PUBLISHER);   SetStringField(frame, ID3FN_TEXT, info.label);  container.AddFrame(frame); }
	if (info.isrc   != NIL) { ID3_Frame frame(ID3FID_ISRC);	       SetStringField(frame, ID3FN_TEXT, info.isrc);   container.AddFrame(frame); }

	if (info.year > 0)
	{
		ID3_Frame	 frame;

		if (container.GetSpec() == ID3V2_4_0) frame = ID3_Frame(ID3FID_RECORDINGTIME);
		else				      frame = ID3_Frame(ID3FID_YEAR);

		SetStringField(frame, ID3FN_TEXT, String::FromInt(info.year));

		container.AddFrame(frame);
	}

	if (info.track > 0)
	{
		String	 trackString = String(prependZero && info.track < 10 ? "0" : NIL).Append(String::FromInt(info.track));

		if (info.numTracks > 0) trackString.Append("/").Append(prependZero && info.numTracks < 10 ? "0" : NIL).Append(String::FromInt(info.numTracks));

		{ ID3_Frame frame(ID3FID_TRACKNUM); SetStringField(frame, ID3FN_TEXT, trackString); container.AddFrame(frame); }
	}

	if (info.disc > 0)
	{
		String	 discString = String(prependZero && info.disc < 10 ? "0" : NIL).Append(String::FromInt(info.disc));

		if (info.numDiscs > 0) discString.Append("/").Append(prependZero && info.numDiscs < 10 ? "0" : NIL).Append(String::FromInt(info.numDiscs));

		{ ID3_Frame frame(ID3FID_PARTINSET); SetStringField(frame, ID3FN_TEXT, discString); container.AddFrame(frame); }
	}

	if (info.rating > 0)
	{
		Int	 rating = Math::Min(255, info.rating * 256 / 100);

		{ ID3_Frame frame(ID3FID_POPULARIMETER); SetASCIIField(frame, ID3FN_EMAIL, "rating@freac.org"); SetIntegerField(frame, ID3FN_RATING, rating); container.AddFrame(frame); }
	}

	/* Save comment.
	 */
	if ((info.comment != NIL && !replaceExistingComments) || (!isChapter && defaultComment != NIL))
	{
		ID3_Frame	 frame(ID3FID_COMMENT);

		SetASCIIField(frame, ID3FN_LANGUAGE, "eng"); // iTunes recognizes comments only if language is set to English

		if (info.comment != NIL && !replaceExistingComments) SetStringField(frame, ID3FN_TEXT, info.comment, False);
		else						     SetStringField(frame, ID3FN_TEXT, defaultComment);

		container.AddFrame(frame);
	}

	/* Set band to album artist if only album artist is filled.
	 */
	if (info.HasOtherInfo(INFO_ALBUMARTIST) && !info.HasOtherInfo(INFO_BAND))
	{
		info.SetOtherInfo(INFO_BAND, info.GetOtherInfo(INFO_ALBUMARTIST));
		info.SetOtherInfo(INFO_ALBUMARTIST, NIL);
	}

	/* Save other text info.
	 */
	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":"));
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == INFO_ALBUMARTIST)    { ID3_Frame frame(ID3FID_USERTEXT);	  SetStringField(frame, ID3FN_TEXT, value); SetStringField(frame, ID3FN_DESCRIPTION, "Album Artist");  container.AddFrame(frame); }

		else if	(key == INFO_CONTENTGROUP)   { ID3_Frame frame(ID3FID_CONTENTGROUP);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == INFO_SUBTITLE)	     { ID3_Frame frame(ID3FID_SUBTITLE);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == INFO_BAND)	     { ID3_Frame frame(ID3FID_BAND);		  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == INFO_CONDUCTOR)	     { ID3_Frame frame(ID3FID_CONDUCTOR);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == INFO_REMIXER)	     { ID3_Frame frame(ID3FID_MIXARTIST);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == INFO_COMPOSER)	     { ID3_Frame frame(ID3FID_COMPOSER);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == INFO_LYRICIST)	     { ID3_Frame frame(ID3FID_LYRICIST);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == INFO_ORIG_ARTIST)    { ID3_Frame frame(ID3FID_ORIGARTIST);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == INFO_ORIG_ALBUM)     { ID3_Frame frame(ID3FID_ORIGALBUM);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == INFO_ORIG_LYRICIST)  { ID3_Frame frame(ID3FID_ORIGLYRICIST);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if (key == INFO_ORIG_YEAR)
		{
			ID3_Frame	 frame;

			if (container.GetSpec() == ID3V2_4_0) frame = ID3_Frame(ID3FID_ORIGRELEASETIME);
			else				      frame = ID3_Frame(ID3FID_ORIGYEAR);

			SetStringField(frame, ID3FN_TEXT, String::FromInt(info.year));

			container.AddFrame(frame);
		}

		else if	(key == INFO_MOVEMENT)
		{
			if (info.HasOtherInfo(INFO_MOVEMENTTOTAL)) value.Append("/").Append(info.GetOtherInfo(INFO_MOVEMENTTOTAL));

			{ ID3_Frame frame(ID3FID_MOVEMENT); SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		}

		else if	(key == INFO_MOVEMENTNAME)   { ID3_Frame frame(ID3FID_MOVEMENTNAME);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == INFO_BPM)	     { ID3_Frame frame(ID3FID_BPM);		  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == INFO_INITIALKEY)     { ID3_Frame frame(ID3FID_INITIALKEY);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == INFO_COPYRIGHT)	     { ID3_Frame frame(ID3FID_COPYRIGHT);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == INFO_MEDIATYPE)	     { ID3_Frame frame(ID3FID_MEDIATYPE);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if (key == INFO_CATALOGNUMBER)  { ID3_Frame frame(ID3FID_USERTEXT);	  SetStringField(frame, ID3FN_TEXT, value); SetStringField(frame, ID3FN_DESCRIPTION, "CATALOGNUMBER"); container.AddFrame(frame); }
		else if (key == INFO_BARCODE)	     { ID3_Frame frame(ID3FID_USERTEXT);	  SetStringField(frame, ID3FN_TEXT, value); SetStringField(frame, ID3FN_DESCRIPTION, "BARCODE");       container.AddFrame(frame); }

		else if	(key == INFO_DISCSUBTITLE)   { ID3_Frame frame(ID3FID_SETSUBTITLE);	  SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == INFO_RADIOSTATION)   { ID3_Frame frame(ID3FID_NETRADIOSTATION);   SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == INFO_RADIOOWNER)     { ID3_Frame frame(ID3FID_NETRADIOOWNER);     SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if (key == INFO_USERTEXT)	     { ID3_Frame frame(ID3FID_USERTEXT);	  SetStringField(frame, ID3FN_TEXT, value.Tail(value.Length() - value.Find(":|:") - 3)); SetStringField(frame, ID3FN_DESCRIPTION, value.Head(value.Find(":|:"))); container.AddFrame(frame); }

		else if	(key == INFO_WEB_ARTIST)     { ID3_Frame frame(ID3FID_WWWARTIST);	  SetASCIIField(frame, ID3FN_URL, value);   container.AddFrame(frame); }
		else if	(key == INFO_WEB_PUBLISHER)  { ID3_Frame frame(ID3FID_WWWPUBLISHER);      SetASCIIField(frame, ID3FN_URL, value);   container.AddFrame(frame); }
		else if	(key == INFO_WEB_RADIO)	     { ID3_Frame frame(ID3FID_WWWRADIOPAGE);      SetASCIIField(frame, ID3FN_URL, value);   container.AddFrame(frame); }
		else if	(key == INFO_WEB_SOURCE)     { ID3_Frame frame(ID3FID_WWWAUDIOSOURCE);    SetASCIIField(frame, ID3FN_URL, value);   container.AddFrame(frame); }
		else if	(key == INFO_WEB_COPYRIGHT)  { ID3_Frame frame(ID3FID_WWWCOPYRIGHT);      SetASCIIField(frame, ID3FN_URL, value);   container.AddFrame(frame); }
		else if	(key == INFO_WEB_COMMERCIAL) { ID3_Frame frame(ID3FID_WWWCOMMERCIALINFO); SetASCIIField(frame, ID3FN_URL, value);   container.AddFrame(frame); }

		else if (key == INFO_WEB_USERURL)    { ID3_Frame frame(ID3FID_WWWUSER);		  SetASCIIField(frame, ID3FN_URL, value.Tail(value.Length() - value.Find(":|:") - 3));	 SetStringField(frame, ID3FN_DESCRIPTION, value.Head(value.Find(":|:"))); container.AddFrame(frame); }
	}

	/* Save Replay Gain info.
	 */
	if (preserveReplayGain)
	{
		if (info.track_gain != NIL && info.track_peak != NIL)
		{
			{ ID3_Frame frame(ID3FID_USERTEXT); SetStringField(frame, ID3FN_TEXT, info.track_gain); SetStringField(frame, ID3FN_DESCRIPTION, "replaygain_track_gain"); container.AddFrame(frame); }
			{ ID3_Frame frame(ID3FID_USERTEXT); SetStringField(frame, ID3FN_TEXT, info.track_peak); SetStringField(frame, ID3FN_DESCRIPTION, "replaygain_track_peak"); container.AddFrame(frame); }
		}

		if (info.album_gain != NIL && info.album_peak != NIL)
		{
			{ ID3_Frame frame(ID3FID_USERTEXT); SetStringField(frame, ID3FN_TEXT, info.album_gain); SetStringField(frame, ID3FN_DESCRIPTION, "replaygain_album_gain"); container.AddFrame(frame); }
			{ ID3_Frame frame(ID3FID_USERTEXT); SetStringField(frame, ID3FN_TEXT, info.album_peak); SetStringField(frame, ID3FN_DESCRIPTION, "replaygain_album_peak"); container.AddFrame(frame); }
		}
	}

	/* Save CD table of contents.
	 */
	if (writeMCDI && info.mcdi.IsValid() && !isChapter)
	{
		ID3_Frame	 frame_mcdi(ID3FID_CDID);

		SetBinaryField(frame_mcdi, ID3FN_DATA, info.mcdi.GetData());

		container.AddFrame(frame_mcdi);
	}

	/* Save encoder version.
	 */
	if (!isChapter)
	{
		Application	*app = Application::Get();
		ID3_Frame	 frame(ID3FID_ENCODERSETTINGS);

		SetStringField(frame, ID3FN_TEXT, app->getClientName.Call().Append(" ").Append(app->getClientVersion.Call()));

		container.AddFrame(frame);
	}

	/* Save cover art.
	 */
	if (coverArtWriteToTags && coverArtWriteToID3v2)
	{
		foreach (const Picture &picInfo, track.pictures)
		{
			ID3_Frame	 frame_picture(ID3FID_PICTURE);

			/* Set the description field and try to stay compatible with
			 * iTunes which expects it to be in a single byte encoding.
			 */
			if (picInfo.description != NIL)
			{
				String		 savedEncodingID = textEncodingID;
				ID3_TextEnc	 savedEncoding	 = textEncoding;

				if (textEncodingID != "UTF-8" && !String::IsUnicode(picInfo.description))
				{
					textEncodingID = "ISO-8859-1";
					textEncoding   = ID3TE_ISO8859_1;
				}

				SetStringField(frame_picture, ID3FN_DESCRIPTION, picInfo.description);

				textEncodingID = savedEncodingID;
				textEncoding   = savedEncoding;
			}

			/* Set picture data.
			 */
			if (picInfo.mime != NIL) SetASCIIField(frame_picture, ID3FN_MIMETYPE, picInfo.mime.ConvertTo("ISO-8859-1"));

			SetIntegerField(frame_picture, ID3FN_PICTURETYPE, picInfo.type);
			SetBinaryField(frame_picture, ID3FN_DATA, picInfo.data);

			container.AddFrame(frame_picture);
		}
	}

	/* Save chapters.
	 */
	if (!isChapter && track.tracks.Length() > 0 && writeChapters)
	{
		/* Write TOC frame.
		 */
		ID3_Frame	 frame_toc(ID3FID_TOC);

		SetASCIIField(frame_toc, ID3FN_ID, String("toc"));
		SetIntegerField(frame_toc, ID3FN_FLAGS, ID3TF_TOPLEVEL | ID3TF_ORDERED);

		for (Int i = 0; i < track.tracks.Length(); i++)
		{
			frame_toc.GetField(ID3FN_CHAPTERS)->Add((char *) String("chp").Append(String::FromInt(i)));
		}

		container.AddFrame(frame_toc);

		/* Write chapter frames.
		 */
		Int64	 offset = 0;

		for (Int i = 0; i < track.tracks.Length(); i++)
		{
			const Track	&chapterTrack  = track.tracks.GetNth(i);
			const Format	&chapterFormat = chapterTrack.GetFormat();

			ID3_Frame	 frame_chapter(ID3FID_CHAPTER);

			SetASCIIField(frame_chapter, ID3FN_ID, String("chp").Append(String::FromInt(i)));

			SetIntegerField(frame_chapter, ID3FN_STARTTIME, Float(offset) * 1000.0 / chapterFormat.rate);

			if	(chapterTrack.length	   >= 0) SetIntegerField(frame_chapter, ID3FN_ENDTIME, Float(offset + chapterTrack.length)	 * 1000.0 / chapterFormat.rate);
			else if (chapterTrack.approxLength >= 0) SetIntegerField(frame_chapter, ID3FN_ENDTIME, Float(offset + chapterTrack.approxLength) * 1000.0 / chapterFormat.rate);

			/* Render individual chapter information.
			 */
			RenderContainer(*frame_chapter.GetField(ID3FN_FRAMES), chapterTrack, True);

			container.AddFrame(frame_chapter);

			if	(chapterTrack.length	   >= 0) offset += chapterTrack.length;
			else if (chapterTrack.approxLength >= 0) offset += chapterTrack.approxLength;
		}
	}

	return Success();
}

Int BoCA::TaggerID3v2::ParseContainer(const ID3_Container &container, Track &track)
{
	const Config	*currentConfig = GetConfiguration();

	Info	 info = track.GetInfo();

	/* Parse individual comment items.
	 */
	ID3_Container::ConstIterator	*iterator     = container.CreateIterator();
	Bool				 haveChapters = False;

	for (UnsignedInt i = 0; i < container.NumFrames(); i++)
	{
		const ID3_Frame	&frame = *iterator->GetNext();

		if	(frame.GetID() == ID3FID_LEADARTIST)	    info.artist  = GetStringField(frame, ID3FN_TEXT);
		else if (frame.GetID() == ID3FID_TITLE)		    info.title	 = GetStringField(frame, ID3FN_TEXT);
		else if (frame.GetID() == ID3FID_ALBUM)		    info.album	 = GetStringField(frame, ID3FN_TEXT);
		else if (frame.GetID() == ID3FID_YEAR)		    info.year	 = GetStringField(frame, ID3FN_TEXT).ToInt();
		else if (frame.GetID() == ID3FID_RECORDINGTIME)	    info.year	 = GetStringField(frame, ID3FN_TEXT).Head(4).ToInt();
		else if (frame.GetID() == ID3FID_COMMENT)	    info.comment = GetStringField(frame, ID3FN_TEXT, False);
		else if (frame.GetID() == ID3FID_PUBLISHER)	    info.label	 = GetStringField(frame, ID3FN_TEXT);

		else if (frame.GetID() == ID3FID_ISRC)
		{
			if (Info::IsISRC(GetStringField(frame, ID3FN_TEXT))) info.isrc = GetStringField(frame, ID3FN_TEXT);
		}

		else if (frame.GetID() == ID3FID_CONTENTGROUP)	    info.SetOtherInfo(INFO_CONTENTGROUP,   GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_SUBTITLE)	    info.SetOtherInfo(INFO_SUBTITLE,	   GetStringField(frame, ID3FN_TEXT));

		else if (frame.GetID() == ID3FID_BAND)		    info.SetOtherInfo(INFO_BAND,	   GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_CONDUCTOR)	    info.SetOtherInfo(INFO_CONDUCTOR,	   GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_MIXARTIST)	    info.SetOtherInfo(INFO_REMIXER,	   GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_COMPOSER)	    info.SetOtherInfo(INFO_COMPOSER,	   GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_LYRICIST)	    info.SetOtherInfo(INFO_LYRICIST,	   GetStringField(frame, ID3FN_TEXT));

		else if (frame.GetID() == ID3FID_ORIGARTIST)	    info.SetOtherInfo(INFO_ORIG_ARTIST,	   GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_ORIGALBUM)	    info.SetOtherInfo(INFO_ORIG_ALBUM,	   GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_ORIGLYRICIST)	    info.SetOtherInfo(INFO_ORIG_LYRICIST,  GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_ORIGYEAR)	    info.SetOtherInfo(INFO_ORIG_YEAR,	   GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_ORIGRELEASETIME)   info.SetOtherInfo(INFO_ORIG_YEAR,	   GetStringField(frame, ID3FN_TEXT).Head(4));

		else if (frame.GetID() == ID3FID_MOVEMENT)
		{
			String	 movementString = GetStringField(frame, ID3FN_TEXT);

			info.SetOtherInfo(INFO_MOVEMENT, String::FromInt(movementString.ToInt()));

			if (movementString.Contains("/")) info.SetOtherInfo(INFO_MOVEMENTTOTAL, movementString.Tail(movementString.Length() - movementString.Find("/") - 1));
		}

		else if (frame.GetID() == ID3FID_MOVEMENTNAME)	    info.SetOtherInfo(INFO_MOVEMENTNAME,   GetStringField(frame, ID3FN_TEXT));

		else if (frame.GetID() == ID3FID_BPM)
		{
			if (GetStringField(frame, ID3FN_TEXT).ToInt() > 0) info.SetOtherInfo(INFO_BPM, GetStringField(frame, ID3FN_TEXT));
		}

		else if (frame.GetID() == ID3FID_INITIALKEY)	    info.SetOtherInfo(INFO_INITIALKEY,	   GetStringField(frame, ID3FN_TEXT));

		else if (frame.GetID() == ID3FID_COPYRIGHT)	    info.SetOtherInfo(INFO_COPYRIGHT,	   GetStringField(frame, ID3FN_TEXT));

		else if (frame.GetID() == ID3FID_MEDIATYPE)	    info.SetOtherInfo(INFO_MEDIATYPE,	   GetStringField(frame, ID3FN_TEXT));

		else if (frame.GetID() == ID3FID_SETSUBTITLE)	    info.SetOtherInfo(INFO_DISCSUBTITLE,   GetStringField(frame, ID3FN_TEXT));

		else if (frame.GetID() == ID3FID_NETRADIOSTATION)   info.SetOtherInfo(INFO_RADIOSTATION,   GetStringField(frame, ID3FN_TEXT));
		else if (frame.GetID() == ID3FID_NETRADIOOWNER)     info.SetOtherInfo(INFO_RADIOOWNER,	   GetStringField(frame, ID3FN_TEXT));

		else if (frame.GetID() == ID3FID_WWWARTIST)	    info.SetOtherInfo(INFO_WEB_ARTIST,	   GetASCIIField(frame, ID3FN_URL));
		else if (frame.GetID() == ID3FID_WWWPUBLISHER)	    info.SetOtherInfo(INFO_WEB_PUBLISHER,  GetASCIIField(frame, ID3FN_URL));
		else if (frame.GetID() == ID3FID_WWWRADIOPAGE)	    info.SetOtherInfo(INFO_WEB_RADIO,	   GetASCIIField(frame, ID3FN_URL));
		else if (frame.GetID() == ID3FID_WWWAUDIOSOURCE)    info.SetOtherInfo(INFO_WEB_SOURCE,	   GetASCIIField(frame, ID3FN_URL));
		else if (frame.GetID() == ID3FID_WWWCOPYRIGHT)	    info.SetOtherInfo(INFO_WEB_COPYRIGHT,  GetASCIIField(frame, ID3FN_URL));
		else if (frame.GetID() == ID3FID_WWWCOMMERCIALINFO) info.SetOtherInfo(INFO_WEB_COMMERCIAL, GetASCIIField(frame, ID3FN_URL));

		else if (frame.GetID() == ID3FID_WWWUSER)
		{
			String	 description = GetStringField(frame, ID3FN_DESCRIPTION);
			String	 value	     = GetASCIIField(frame, ID3FN_URL);

			if (value == NIL) continue;

			info.other.Add(String(INFO_WEB_USERURL).Append(":").Append(description).Append(":|:").Append(value));
		}
		else if (frame.GetID() == ID3FID_TRACKNUM)
		{
			String	 trackString = GetStringField(frame, ID3FN_TEXT);

			info.track = trackString.ToInt();

			if (trackString.Contains("/")) info.numTracks = trackString.Tail(trackString.Length() - trackString.Find("/") - 1).ToInt();
		}
		else if (frame.GetID() == ID3FID_PARTINSET)
		{
			String	 discString = GetStringField(frame, ID3FN_TEXT);

			info.disc = discString.ToInt();

			if (discString.Contains("/")) info.numDiscs = discString.Tail(discString.Length() - discString.Find("/") - 1).ToInt();
		}
		else if (frame.GetID() == ID3FID_POPULARIMETER)
		{
			Int	 rating = GetIntegerField(frame, ID3FN_RATING);

			if (rating > 0) info.rating = Math::Max(1, Math::Min(255, rating * 100 / 255));
		}
		else if (frame.GetID() == ID3FID_USERTEXT)
		{
			String	 description = GetStringField(frame, ID3FN_DESCRIPTION);
			String	 value	     = GetStringField(frame, ID3FN_TEXT);

			if (value == NIL) continue;

			if	(description.ToLower() == "replaygain_track_gain") info.track_gain = value;
			else if (description.ToLower() == "replaygain_track_peak") info.track_peak = value;
			else if (description.ToLower() == "replaygain_album_gain") info.album_gain = value;
			else if (description.ToLower() == "replaygain_album_peak") info.album_peak = value;

			else if (description.ToLower() == "cuesheet")
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

			else if (description.ToLower() == "album artist")	   info.SetOtherInfo(INFO_ALBUMARTIST,	 value);

			else if (description.ToLower() == "catalognumber")	   info.SetOtherInfo(INFO_CATALOGNUMBER, value);
			else if (description.ToLower() == "barcode")		   info.SetOtherInfo(INFO_BARCODE,	 value);

			else							   info.other.Add(String(INFO_USERTEXT).Append(":").Append(description).Append(":|:").Append(value));
		}
		else if (frame.GetID() == ID3FID_CONTENTTYPE)
		{
			String	 s_genre = GetStringField(frame, ID3FN_TEXT);
			String	 genreID;

			if (s_genre[0] == '(')
			{
				for (Int j = 1; j < s_genre.Length(); j++)
				{
					if (s_genre[j] == ')') break;

					genreID[j - 1] = s_genre[j];
				}
			}

			if	(genreID == NIL)			  info.genre = s_genre;
			else if (s_genre.Length() > genreID.Length() + 2) info.genre = s_genre.Tail(s_genre.Length() - genreID.Length() - 2);
			else						  info.genre = GetID3CategoryName(genreID.ToInt());
		}
		else if (frame.GetID() == ID3FID_CDID)
		{
			Buffer<UnsignedByte>	 data;

			GetBinaryField(frame, ID3FN_DATA, data);

			/* Use a heuristic to detect if this is a valid binary MCDI
			 * field or the commonly used track offset string.
			 */
			Bool	 binary = False;

			for (Int i = 0; i < data.Size(); i++)
			{
				if (data[i] > 0 && data[i] < 0x20) { binary = True; break; }
			}

			if (binary)
			{
				/* Check validity of MCDI data and assign.
				 */
				MCDI	 mcdi(data);

				if (mcdi.IsValid()) info.mcdi.SetData(data);
			}
			else
			{
				/* Found offset string.
				 */
				for (Int i = 0; i < data.Size() / 2; i++)
				{
					info.offsets[i] = ((short *) (UnsignedByte *) data)[i];

					if (info.offsets[i] == 0) break;
				}
			}
		}
		else if (frame.GetID() == ID3FID_PICTURE && currentConfig->GetIntValue(ConfigID, "CoverArtReadFromTags", True))
		{
			Picture			 picture;
			Buffer<UnsignedByte>	 buffer;

			GetBinaryField(frame, ID3FN_DATA, buffer);

			picture.description = GetStringField(frame, ID3FN_DESCRIPTION);
			picture.type	    = GetIntegerField(frame, ID3FN_PICTURETYPE);
			picture.mime	    = GetASCIIField(frame, ID3FN_MIMETYPE);

			picture.data	    = buffer;

			if (picture.data.Size() >= 16)
			{
				if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
				else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
					 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
					 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
					 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

				if (picture.data[0] != 0 && picture.data[1] != 0) track.pictures.Add(picture);
			}
		}
		else if (frame.GetID() == ID3FID_CHAPTER)
		{
			/* Chapters are processed further down.
			 */
			haveChapters = True;
		}
	}

	/* Set album artist to band if only band is filled.
	 */
	if (info.HasOtherInfo(INFO_BAND) && !info.HasOtherInfo(INFO_ALBUMARTIST))
	{
		info.SetOtherInfo(INFO_ALBUMARTIST, info.GetOtherInfo(INFO_BAND));
		info.SetOtherInfo(INFO_BAND, NIL);
	}

	/* Set artist to album artist if artist is not filled.
	 */
	if (info.artist == NIL) info.artist = info.GetOtherInfo(INFO_ALBUMARTIST);

	/* Remove sub-tracks if main track has a track number.
	 */
	if (info.track > 0) track.tracks.RemoveAll();

	track.SetInfo(info);

	delete iterator;

	/* Read chapters.
	 */
	if (haveChapters && currentConfig->GetIntValue(ConfigID, "ReadChapters", True) &&
			  (!currentConfig->GetIntValue(ConfigID, "PreferCueSheetsToChapters", True) || track.tracks.Length() == 0))
	{
		track.tracks.RemoveAll();

		Int		 chapterCount = 0;
		Array<String>	 chapterIDs;

		iterator = container.CreateIterator();

		for (UnsignedInt i = 0; i < container.NumFrames(); i++)
		{
			const ID3_Frame	&frame = *iterator->GetNext();

			if	(frame.GetID() == ID3FID_TOC)
			{
				/* Respect first toplevel TOC only.
				 */
				if (chapterIDs.Length() == 0 && GetIntegerField(frame, ID3FN_FLAGS) & ID3TF_TOPLEVEL)
				{
					ID3_Field	*chapters = frame.GetField(ID3FN_CHAPTERS);
					Buffer<char>	 buffer(1024);

					for (UnsignedInt i = 0; i < chapters->GetNumTextItems(); i++)
					{
						buffer.Zero();

						chapters->Get(buffer, buffer.Size(), i);

						String	 chapterID;

						chapterID.ImportFrom("ISO-8859-1", buffer);
						chapterIDs.Add(chapterID, chapterID.ComputeCRC32());
					}
				}
			}
			else if (frame.GetID() == ID3FID_CHAPTER)
			{
				const Format	&format = track.GetFormat();

				/* Fill track data.
				 */
				Track	 rTrack;

				rTrack.fileName	    = track.fileName;
				rTrack.pictures	    = track.pictures;

				rTrack.sampleOffset = Math::Round(Float(				       frame.GetField(ID3FN_STARTTIME)->Get()) / 1000.0 * format.rate);
				rTrack.length	    = Math::Round(Float(frame.GetField(ID3FN_ENDTIME)->Get() - frame.GetField(ID3FN_STARTTIME)->Get()) / 1000.0 * format.rate);

				if	(track.length	    > 0) rTrack.fileSize = Math::Round(Float(track.fileSize) / track.length * rTrack.length);
				else if (track.approxLength > 0) rTrack.fileSize = Math::Round(Float(track.fileSize) / track.approxLength * rTrack.length);
				else				 rTrack.fileSize = rTrack.length * format.channels * (format.bits / 8);

				rTrack.SetFormat(format);

				/* Set track number and parent track info.
				 */
				Info	 info = track.GetInfo();

				info.track = ++chapterCount;

				rTrack.SetInfo(info);

				/* Parse individual chapter information.
				 */
				ParseContainer(*frame.GetField(ID3FN_FRAMES), rTrack);

				/* Add track to track list.
				 */
				track.tracks.Add(rTrack, GetASCIIField(frame, ID3FN_ID).ComputeCRC32());
			}
		}

		delete iterator;

		/* Ignore chapters that are not listed in TOC.
		 */
		if (chapterIDs.Length() > 0)
		{
			for (Int i = track.tracks.Length() - 1; i >= 0; i--)
			{
				if (chapterIDs.Get(track.tracks.GetNthIndex(i)) == NIL) track.tracks.RemoveNth(i);
			}
		}

		/* Discard chapters if their number does not match TOC.
		 */
		if (track.tracks.Length() != chapterIDs.Length())
		{
			track.tracks.RemoveAll();
		}
	}

	return Success();
}

String BoCA::TaggerID3v2::GetStringField(const ID3_Frame &frame, ID3_FieldID fieldType, Bool trim)
{
	ID3_Field	*field = frame.GetField(fieldType);
	String		 result;

	if (field != NIL)
	{
		Int	 encoding = GetIntegerField(frame, ID3FN_TEXTENC);

		if (encoding == ID3TE_ISO8859_1 || encoding == ID3TE_UTF8)
		{
			Buffer<char>	 aBuffer(field->Size() + 1);

			aBuffer.Zero();

			field->Get(aBuffer, field->Size());

			if	(encoding == ID3TE_ISO8859_1) result.ImportFrom("ISO-8859-1", aBuffer);
			else if (encoding == ID3TE_UTF8)      result.ImportFrom("UTF-8", aBuffer);
		}
		else if (encoding == ID3TE_UTF16 || encoding == ID3TE_UTF16BE)
		{
			Buffer<wchar_t>	 wBuffer(field->Size() + 1);

			wBuffer.Zero();

			field->Get((unicode_t *) (wchar_t *) wBuffer, field->Size());

			result.ImportFrom("UTF-16BE", (char *) (wchar_t *) wBuffer);
		}
	}

	return trim ? result.Trim() : result;
}

Int BoCA::TaggerID3v2::SetStringField(ID3_Frame &frame, ID3_FieldID fieldType, const String &value, Bool trim)
{
	String	 string = trim ? value.Trim() : value;

	if (string == NIL) return Error();

	SetIntegerField(frame, ID3FN_TEXTENC, textEncoding);

	ID3_Field	*field = frame.GetField(fieldType);

	if (field != NIL)
	{
		field->SetEncoding(textEncoding);

		if	(textEncoding == ID3TE_UTF16)	field->Set((unicode_t *) string.ConvertTo("UTF-16LE"));
		else if (textEncoding == ID3TE_UTF16BE)	field->Set((unicode_t *) string.ConvertTo("UTF-16BE"));
		else					field->Set((char *)	 string.ConvertTo(textEncodingID));

		return Success();
	}

	return Error();
}

String BoCA::TaggerID3v2::GetASCIIField(const ID3_Frame &frame, ID3_FieldID fieldType, Bool trim)
{
	ID3_Field	*field = frame.GetField(fieldType);
	String		 result;

	if (field != NIL)
	{
		Buffer<char>	 aBuffer(field->Size() + 1);

		aBuffer.Zero();

		field->Get(aBuffer, field->Size());

		result.ImportFrom("ISO-8859-1", aBuffer);
	}

	return trim ? result.Trim() : result;
}

Int BoCA::TaggerID3v2::SetASCIIField(ID3_Frame &frame, ID3_FieldID fieldType, const String &value, Bool trim)
{
	String	 string = trim ? value.Trim() : value;

	if (string == NIL) return Error();

	ID3_Field	*field = frame.GetField(fieldType);

	if (field != NIL)
	{
		field->Set((char *) string);

		return Success();
	}

	return Error();
}

Int BoCA::TaggerID3v2::GetIntegerField(const ID3_Frame &frame, ID3_FieldID fieldType)
{
	ID3_Field	*field = frame.GetField(fieldType);

	if (field != NIL)
	{
		return field->Get();
	}

	return -1;
}

Int BoCA::TaggerID3v2::SetIntegerField(ID3_Frame &frame, ID3_FieldID fieldType, Int value)
{
	ID3_Field	*field = frame.GetField(fieldType);

	if (field != NIL)
	{
		field->Set(value);

		return Success();
	}

	return Error();
}

Int BoCA::TaggerID3v2::GetBinaryField(const ID3_Frame &frame, ID3_FieldID fieldType, Buffer<UnsignedByte> &buffer)
{
	ID3_Field	*field = frame.GetField(fieldType);

	if (field != NIL)
	{
		buffer.Resize(field->Size());

		field->Get(buffer, buffer.Size());

		return Success();
	}

	return Error();
}

Int BoCA::TaggerID3v2::SetBinaryField(ID3_Frame &frame, ID3_FieldID fieldType, const Buffer<UnsignedByte> &data)
{
	if (data.Size() == 0) return Error();

	ID3_Field	*field = frame.GetField(fieldType);

	if (field != NIL)
	{
		field->Set(data, data.Size());

		return Success();
	}

	return Error();
}

const String &BoCA::TaggerID3v2::GetID3CategoryName(UnsignedInt id)
{
	static const String	 empty;

	if (id > 191) return empty;
	else	      return genres[id];
}
