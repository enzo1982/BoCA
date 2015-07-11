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

String		 BoCA::TaggerID3v2::dummyString;

BoCA::TaggerID3v2::TaggerID3v2()
{
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

	/* Copy to temporary file and write tag.
	 */
	String		 tempFile = String(fileName).Append(".boca.temp");
	OutStream	 out(STREAM_FILE, tempFile, OS_APPEND);

	if (out.GetLastError() == IO_ERROR_OK)
	{
		Buffer<UnsignedByte>	 buffer;

		RenderBuffer(buffer, track);

		out.OutputData(buffer, buffer.Size());

		buffer.Resize(1024);

		for (Int i = in.GetPos(); i < in.Size(); i += buffer.Size())
		{
			Int	 bytes = Math::Min(Int64(buffer.Size()), in.Size() - i);

			in.InputData(buffer, bytes);
			out.OutputData(buffer, bytes);
		}

		in.Close();
		out.Close();

		File(fileName).Delete();
		File(tempFile).Move(fileName);

		return Success();
	}

	return Error();
}

Int BoCA::TaggerID3v2::RenderContainer(ID3_Container &container, const Track &track, Bool isChapter)
{
	const Config	*currentConfig = GetConfiguration();

	const Info	&info = track.GetInfo();

	if (info.artist != NIL) { ID3_Frame frame(ID3FID_LEADARTIST);  SetStringField(frame, ID3FN_TEXT, info.artist);		      container.AddFrame(frame); }
	if (info.title  != NIL) { ID3_Frame frame(ID3FID_TITLE);       SetStringField(frame, ID3FN_TEXT, info.title);		      container.AddFrame(frame); }
	if (info.album  != NIL) { ID3_Frame frame(ID3FID_ALBUM);       SetStringField(frame, ID3FN_TEXT, info.album);		      container.AddFrame(frame); }
	if (info.year    >   0) { ID3_Frame frame(ID3FID_YEAR);	       SetStringField(frame, ID3FN_TEXT, String::FromInt(info.year)); container.AddFrame(frame); }
	if (info.genre  != NIL) { ID3_Frame frame(ID3FID_CONTENTTYPE); SetStringField(frame, ID3FN_TEXT, info.genre);		      container.AddFrame(frame); }
	if (info.label  != NIL) { ID3_Frame frame(ID3FID_PUBLISHER);   SetStringField(frame, ID3FN_TEXT, info.label);		      container.AddFrame(frame); }
	if (info.isrc   != NIL) { ID3_Frame frame(ID3FID_ISRC);	       SetStringField(frame, ID3FN_TEXT, info.isrc);		      container.AddFrame(frame); }

	if (info.track > 0)
	{
		String	 trackString = String(info.track < 10 ? "0" : "").Append(String::FromInt(info.track));

		if (info.numTracks > 0) trackString.Append("/").Append(info.numTracks < 10 ? "0" : "").Append(String::FromInt(info.numTracks));

		{ ID3_Frame frame(ID3FID_TRACKNUM); SetStringField(frame, ID3FN_TEXT, trackString); container.AddFrame(frame); }
	}

	if (info.disc > 0)
	{
		String	 discString = String(info.disc < 10 ? "0" : "").Append(String::FromInt(info.disc));

		if (info.numDiscs > 0) discString.Append("/").Append(info.numDiscs < 10 ? "0" : "").Append(String::FromInt(info.numDiscs));

		{ ID3_Frame frame(ID3FID_PARTINSET); SetStringField(frame, ID3FN_TEXT, discString); container.AddFrame(frame); }
	}

	if (info.rating > 0)
	{
		Int	 rating = Math::Min(255, info.rating * 256 / 100);

		{ ID3_Frame frame(ID3FID_POPULARIMETER); SetASCIIField(frame, ID3FN_EMAIL, "rating@freac.org"); SetIntegerField(frame, ID3FN_RATING, rating); container.AddFrame(frame); }
	}

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	{ ID3_Frame frame(ID3FID_COMMENT); SetStringField(frame, ID3FN_TEXT, info.comment);						    container.AddFrame(frame); }
	else if (!isChapter && currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)		{ ID3_Frame frame(ID3FID_COMMENT); SetStringField(frame, ID3FN_TEXT, currentConfig->GetStringValue("Tags", "DefaultComment", NIL)); container.AddFrame(frame); }

	/* Save other text info.
	 */
	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":") + 1);
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == String(INFO_CONTENTGROUP).Append(":"))	 { ID3_Frame frame(ID3FID_CONTENTGROUP);      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_SUBTITLE).Append(":"))	 { ID3_Frame frame(ID3FID_SUBTITLE);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == String(INFO_BAND).Append(":"))		 { ID3_Frame frame(ID3FID_BAND);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_CONDUCTOR).Append(":"))	 { ID3_Frame frame(ID3FID_CONDUCTOR);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_REMIX).Append(":"))		 { ID3_Frame frame(ID3FID_MIXARTIST);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_COMPOSER).Append(":"))	 { ID3_Frame frame(ID3FID_COMPOSER);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_LYRICIST).Append(":"))	 { ID3_Frame frame(ID3FID_LYRICIST);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == String(INFO_ORIG_ARTIST).Append(":"))	 { ID3_Frame frame(ID3FID_ORIGARTIST);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_ORIG_ALBUM).Append(":"))	 { ID3_Frame frame(ID3FID_ORIGALBUM);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_ORIG_LYRICIST).Append(":"))	 { ID3_Frame frame(ID3FID_ORIGLYRICIST);      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_ORIG_YEAR).Append(":"))	 { ID3_Frame frame(ID3FID_ORIGYEAR);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == String(INFO_BPM).Append(":"))		 { ID3_Frame frame(ID3FID_BPM);		      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_INITIALKEY).Append(":"))	 { ID3_Frame frame(ID3FID_INITIALKEY);	      SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == String(INFO_RADIOSTATION).Append(":"))	 { ID3_Frame frame(ID3FID_NETRADIOSTATION);   SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }
		else if	(key == String(INFO_RADIOOWNER).Append(":"))	 { ID3_Frame frame(ID3FID_NETRADIOOWNER);     SetStringField(frame, ID3FN_TEXT, value); container.AddFrame(frame); }

		else if	(key == String(INFO_WEB_ARTIST).Append(":"))	 { ID3_Frame frame(ID3FID_WWWARTIST);	      SetASCIIField(frame, ID3FN_URL, value);	container.AddFrame(frame); }
		else if	(key == String(INFO_WEB_PUBLISHER).Append(":"))	 { ID3_Frame frame(ID3FID_WWWPUBLISHER);      SetASCIIField(frame, ID3FN_URL, value);	container.AddFrame(frame); }
		else if	(key == String(INFO_WEB_RADIO).Append(":"))	 { ID3_Frame frame(ID3FID_WWWRADIOPAGE);      SetASCIIField(frame, ID3FN_URL, value);	container.AddFrame(frame); }
		else if	(key == String(INFO_WEB_SOURCE).Append(":"))	 { ID3_Frame frame(ID3FID_WWWAUDIOSOURCE);    SetASCIIField(frame, ID3FN_URL, value);	container.AddFrame(frame); }
		else if	(key == String(INFO_WEB_COPYRIGHT).Append(":"))	 { ID3_Frame frame(ID3FID_WWWCOPYRIGHT);      SetASCIIField(frame, ID3FN_URL, value);	container.AddFrame(frame); }
		else if	(key == String(INFO_WEB_COMMERCIAL).Append(":")) { ID3_Frame frame(ID3FID_WWWCOMMERCIALINFO); SetASCIIField(frame, ID3FN_URL, value);	container.AddFrame(frame); }
	}

	/* Save Replay Gain info.
	 */
	if (currentConfig->GetIntValue("Tags", "PreserveReplayGain", True))
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
	if (currentConfig->GetIntValue("Tags", "WriteMCDI", True))
	{
		if (info.mcdi.GetData().Size() > 0)
		{
			ID3_Frame	 frame_mcdi(ID3FID_CDID);

			SetBinaryField(frame_mcdi, ID3FN_DATA, info.mcdi.GetData());

			container.AddFrame(frame_mcdi);
		}
	}

	/* Save cover art.
	 */
	if (currentConfig->GetIntValue("Tags", "CoverArtWriteToTags", True) && currentConfig->GetIntValue("Tags", "CoverArtWriteToID3v2", True))
	{
		foreach (const Picture &picInfo, track.pictures)
		{
			ID3_Frame	 frame_picture(ID3FID_PICTURE);

			/* Set the description field and try to stay compatible with
			 * iTunes which expects it to be in a single byte encoding.
			 */
			if (picInfo.description != NIL)
			{
				Config	*singleByteConfig = Config::Copy(currentConfig);
				String	 encoding	  = singleByteConfig->GetStringValue("Tags", "ID3v2Encoding", "UTF-16LE");

				if (encoding != "UTF-8" && !String::IsUnicode(picInfo.description)) singleByteConfig->SetStringValue("Tags", "ID3v2Encoding", "ISO-8859-1");

				SetConfiguration(singleByteConfig);

				SetStringField(frame_picture, ID3FN_DESCRIPTION, picInfo.description);

				SetConfiguration(currentConfig);

				Config::Free(singleByteConfig);
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
	if (!isChapter && track.tracks.Length() > 0 && currentConfig->GetIntValue("Tags", "WriteChapters", True))
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
	ID3_Container::ConstIterator	*iterator = container.CreateIterator();

	for (UnsignedInt i = 0; i < container.NumFrames(); i++)
	{
		const ID3_Frame	&frame = *iterator->GetNext();

		if	(frame.GetID() == ID3FID_LEADARTIST)	    info.artist  = GetStringField(frame, ID3FN_TEXT);
		else if (frame.GetID() == ID3FID_TITLE)		    info.title	 = GetStringField(frame, ID3FN_TEXT);
		else if (frame.GetID() == ID3FID_ALBUM)		    info.album	 = GetStringField(frame, ID3FN_TEXT);
		else if (frame.GetID() == ID3FID_YEAR)		    info.year	 = GetStringField(frame, ID3FN_TEXT).ToInt();
		else if (frame.GetID() == ID3FID_RELEASETIME)	    info.year	 = GetStringField(frame, ID3FN_TEXT).Head(4).ToInt();
		else if (frame.GetID() == ID3FID_COMMENT)	    info.comment = GetStringField(frame, ID3FN_TEXT);
		else if (frame.GetID() == ID3FID_PUBLISHER)	    info.label	 = GetStringField(frame, ID3FN_TEXT);
		else if (frame.GetID() == ID3FID_ISRC)		    info.isrc	 = GetStringField(frame, ID3FN_TEXT);

		else if (frame.GetID() == ID3FID_CONTENTGROUP)	    info.other.Add(String(INFO_CONTENTGROUP).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_SUBTITLE)	    info.other.Add(String(INFO_SUBTITLE).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame.GetID() == ID3FID_BAND)		    info.other.Add(String(INFO_BAND).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_CONDUCTOR)	    info.other.Add(String(INFO_CONDUCTOR).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_MIXARTIST)	    info.other.Add(String(INFO_REMIX).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_COMPOSER)	    info.other.Add(String(INFO_COMPOSER).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_LYRICIST)	    info.other.Add(String(INFO_LYRICIST).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame.GetID() == ID3FID_ORIGARTIST)	    info.other.Add(String(INFO_ORIG_ARTIST).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_ORIGALBUM)	    info.other.Add(String(INFO_ORIG_ALBUM).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_ORIGLYRICIST)	    info.other.Add(String(INFO_ORIG_LYRICIST).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_ORIGYEAR)	    info.other.Add(String(INFO_ORIG_YEAR).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame.GetID() == ID3FID_BPM)		    info.other.Add(String(INFO_BPM).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_INITIALKEY)	    info.other.Add(String(INFO_INITIALKEY).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame.GetID() == ID3FID_NETRADIOSTATION)   info.other.Add(String(INFO_RADIOSTATION).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame.GetID() == ID3FID_NETRADIOOWNER)     info.other.Add(String(INFO_RADIOOWNER).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame.GetID() == ID3FID_WWWARTIST)	    info.other.Add(String(INFO_WEB_ARTIST).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame.GetID() == ID3FID_WWWPUBLISHER)	    info.other.Add(String(INFO_WEB_PUBLISHER).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame.GetID() == ID3FID_WWWRADIOPAGE)	    info.other.Add(String(INFO_WEB_RADIO).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame.GetID() == ID3FID_WWWAUDIOSOURCE)    info.other.Add(String(INFO_WEB_SOURCE).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame.GetID() == ID3FID_WWWCOPYRIGHT)	    info.other.Add(String(INFO_WEB_COPYRIGHT).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame.GetID() == ID3FID_WWWCOMMERCIALINFO) info.other.Add(String(INFO_WEB_COMMERCIAL).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));

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

			if	(description.ToLower() == "replaygain_track_gain") info.track_gain = value;
			else if (description.ToLower() == "replaygain_track_peak") info.track_peak = value;
			else if (description.ToLower() == "replaygain_album_gain") info.album_gain = value;
			else if (description.ToLower() == "replaygain_album_peak") info.album_peak = value;
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
			else if (genreID != NIL)			  info.genre = GetID3CategoryName(genreID.ToInt());
		}
		else if (frame.GetID() == ID3FID_CDID)
		{
			Buffer<UnsignedByte>	 mcdi;

			GetBinaryField(frame, ID3FN_DATA, mcdi);

			/* Use a heuristic to detect if this is a valid binary MCDI
			 * field or the commonly used track offset string.
			 */
			Bool	 binary = False;

			for (Int i = 0; i < mcdi.Size(); i++)
			{
				if (mcdi[i] > 0 && mcdi[i] < 0x20) { binary = True; break; }
			}

			if (binary)
			{
				/* Found a binary MCDI field.
				 */
				info.mcdi.SetData(mcdi);
			}
			else
			{
				/* Found offset string.
				 */
				for (Int i = 0; i < mcdi.Size() / 2; i++)
				{
					info.offsets[i] = ((short *) (UnsignedByte *) mcdi)[i];

					if (info.offsets[i] == 0) break;
				}
			}
		}
		else if (frame.GetID() == ID3FID_PICTURE && currentConfig->GetIntValue("Tags", "CoverArtReadFromTags", True))
		{
			Picture			 picture;
			Buffer<UnsignedByte>	 buffer;

			picture.description = GetStringField(frame, ID3FN_DESCRIPTION);
			picture.type	    = GetIntegerField(frame, ID3FN_PICTURETYPE);
			picture.mime	    = GetASCIIField(frame, ID3FN_MIMETYPE);

			if	(picture.mime.ToLower() == "jpeg" || picture.mime.ToLower() == "jpg") picture.mime = "image/jpeg";
			else if (picture.mime.ToLower() == "png")				      picture.mime = "image/png";

			GetBinaryField(frame, ID3FN_DATA, buffer);

			picture.data	    = buffer;

			if (picture.data.Size() > 16 && picture.data[0] != 0 && picture.data[1] != 0) track.pictures.Add(picture);
		}
	}

	track.SetInfo(info);

	delete iterator;

	/* Read chapters.
	 */
	Int		 chapterCount = 0;
	Array<String>	 chapterIDs;

	iterator = container.CreateIterator();

	for (UnsignedInt i = 0; i < container.NumFrames(); i++)
	{
		const ID3_Frame	&frame = *iterator->GetNext();

		if	(frame.GetID() == ID3FID_TOC && currentConfig->GetIntValue("Tags", "ReadChapters", True))
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
		else if (frame.GetID() == ID3FID_CHAPTER && currentConfig->GetIntValue("Tags", "ReadChapters", True))
		{
			const Format	&format = track.GetFormat();

			/* Fill track data.
			 */
			Track	 rTrack;

			rTrack.origFilename = track.origFilename;
			rTrack.pictures	    = track.pictures;

			rTrack.sampleOffset = Math::Round(Float(				       frame.GetField(ID3FN_STARTTIME)->Get()) / 1000.0 * format.rate);
			rTrack.length	    = Math::Round(Float(frame.GetField(ID3FN_ENDTIME)->Get() - frame.GetField(ID3FN_STARTTIME)->Get()) / 1000.0 * format.rate);

			rTrack.fileSize	    = rTrack.length * format.channels * (format.bits / 8);

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

	return Success();
}

String BoCA::TaggerID3v2::GetStringField(const ID3_Frame &frame, ID3_FieldID fieldType)
{
	ID3_Field	*field = frame.GetField(fieldType);
	String		 result;

	if (field != NIL)
	{
		Int	 encoding = GetIntegerField(frame, ID3FN_TEXTENC);

		if (encoding == ID3TE_ISO8859_1 || encoding == ID3TE_UTF8)
		{
			Buffer<char>	 aBuffer(1024);

			aBuffer.Zero();

			field->Get(aBuffer, aBuffer.Size());

			if	(encoding == ID3TE_ISO8859_1) result.ImportFrom("ISO-8859-1", aBuffer);
			else if (encoding == ID3TE_UTF8)      result.ImportFrom("UTF-8", aBuffer);
		}
		else if (encoding == ID3TE_UTF16 || encoding == ID3TE_UTF16BE)
		{
			Buffer<wchar_t>	 wBuffer(1024);

			wBuffer.Zero();

			field->Get((unicode_t *) (wchar_t *) wBuffer, wBuffer.Size());

			result.ImportFrom("UTF-16BE", (char *) (wchar_t *) wBuffer);
		}
	}

	return result.Trim();
}

Int BoCA::TaggerID3v2::SetStringField(ID3_Frame &frame, ID3_FieldID fieldType, const String &string)
{
	if (string == NIL) return Error();

	const Config	*config = GetConfiguration();

	ID3_TextEnc	 encoding   = ID3TE_NONE;
	String		 encodingID = config->GetStringValue("Tags", "ID3v2Encoding", "UTF-16LE");

	if	(encodingID == "UTF-8")				      encoding = ID3TE_UTF8;
	else if (encodingID == "ISO-8859-1")			      encoding = ID3TE_ISO8859_1;
	else if (encodingID == "UTF-16"	  || encodingID == "UCS-2" ||
		 encodingID == "UTF-16LE" || encodingID == "UCS-2LE") encoding = ID3TE_UTF16;
	else if (encodingID == "UTF-16BE" || encodingID == "UCS-2BE") encoding = ID3TE_UTF16BE;

	String		 prevOutFormat = String::SetOutputFormat(encodingID);

	SetIntegerField(frame, ID3FN_TEXTENC, encoding);

	ID3_Field	*field = frame.GetField(fieldType);

	if (field != NIL)
	{
		field->SetEncoding(encoding);

		if	(encoding == ID3TE_UTF16)   field->Set((unicode_t *) string.Trim().ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE) field->Set((unicode_t *) string.Trim().ConvertTo("UTF-16BE"));
		else				    field->Set((char *) string.Trim());

		String::SetOutputFormat(prevOutFormat.ConvertTo("ISO-8859-1"));

		return Success();
	}

	String::SetOutputFormat(prevOutFormat.ConvertTo("ISO-8859-1"));

	return Error();
}

String BoCA::TaggerID3v2::GetASCIIField(const ID3_Frame &frame, ID3_FieldID fieldType)
{
	ID3_Field	*field = frame.GetField(fieldType);
	String		 result;

	if (field != NIL)
	{
		Buffer<char>	 aBuffer(1024);

		aBuffer.Zero();

		field->Get(aBuffer, aBuffer.Size());

		result.ImportFrom("ISO-8859-1", aBuffer);
	}

	return result.Trim();
}

Int BoCA::TaggerID3v2::SetASCIIField(ID3_Frame &frame, ID3_FieldID fieldType, const String &string)
{
	if (string == NIL) return Error();

	ID3_Field	*field = frame.GetField(fieldType);

	if (field != NIL)
	{
		field->Set((char *) string.Trim());

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
