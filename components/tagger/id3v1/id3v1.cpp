 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "id3v1.h"

using namespace smooth::IO;

const String &BoCA::TaggerID3v1::GetComponentSpecs()
{
	static String	 componentSpecs = "				\
									\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
	  <component>							\
	    <name>ID3v1 Tagger</name>					\
	    <version>1.0</version>					\
	    <id>id3v1-tag</id>						\
	    <type>tagger</type>						\
	    <format>							\
	      <name>MPEG 1 Audio Layer 3</name>				\
	      <extension>mp3</extension>				\
	    </format>							\
	    <tagspec default=\"false\">					\
	      <name>ID3v1</name>					\
              <encodings free=\"true\">					\
		<encoding default=\"true\">ISO-8859-1</encoding>	\
		<encoding>ISO-8859-2</encoding>				\
		<encoding>ISO-8859-5</encoding>				\
		<encoding>ISO-8859-7</encoding>				\
		<encoding>CP1251</encoding>				\
	      </encodings>						\
	    </tagspec>							\
	  </component>							\
									\
	";

	return componentSpecs;
}

const String	 BoCA::TaggerID3v1::genres[192] =
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

BoCA::TaggerID3v1::TaggerID3v1()
{
}

BoCA::TaggerID3v1::~TaggerID3v1()
{
}

Error BoCA::TaggerID3v1::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	Config		*currentConfig = Config::Get();

	buffer.Resize(128);

	OutStream	 out(STREAM_BUFFER, buffer, buffer.Size());

	out.OutputString("TAG");

	const Info	&info = track.GetInfo();
	String		 prevOutFormat = String::SetOutputFormat(currentConfig->GetStringValue("Tags", "ID3v1Encoding", "ISO-8859-1"));

	{ out.OutputString(info.title.Head(Math::Min(30, info.title.Length())));   for (Int i = 0; i < 30 - info.title.Length(); i++) out.OutputNumber(0, 1); }
	{ out.OutputString(info.artist.Head(Math::Min(30, info.artist.Length()))); for (Int i = 0; i < 30 - info.artist.Length(); i++) out.OutputNumber(0, 1); }
	{ out.OutputString(info.album.Head(Math::Min(30, info.album.Length())));   for (Int i = 0; i < 30 - info.album.Length(); i++) out.OutputNumber(0, 1); }

	if (info.year > 0) { out.OutputString(String().FillN('0', 4 - String::FromInt(info.year).Length())); out.OutputString(String::FromInt(info.year).Tail(Math::Min(4, String::FromInt(info.year).Length()))); }
	else		   { out.OutputNumber(0, 4); }

	String		 comment;

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	comment = info.comment;
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				comment = currentConfig->GetStringValue("Tags", "DefaultComment", NIL);

	if (info.track > 0)
	{
		{ out.OutputString(comment.Head(Math::Min(28, comment.Length()))); for (Int i = 0; i < 28 - comment.Length(); i++) out.OutputNumber(0, 1); }

		out.OutputNumber(0, 1);
		out.OutputNumber(info.track, 1);
	}
	else
	{
		{ out.OutputString(comment.Head(Math::Min(30, comment.Length()))); for (Int i = 0; i < 30 - comment.Length(); i++) out.OutputNumber(0, 1); }
	}

	out.OutputNumber(GetID3CategoryID(info.genre), 1);

	String::SetOutputFormat(prevOutFormat);

	return Success();
}

Error BoCA::TaggerID3v1::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	if (buffer.Size() != 128) return Error();

	InStream	 in(STREAM_BUFFER, buffer, buffer.Size());

	/* Check if we actually have an
	 * ID3v1 tag, then parse it.
	 */
	if (in.InputString(3) == "TAG")
	{
		String	 prevInFormat = String::SetInputFormat("ISO-8859-1");

		Info	 info = track.GetInfo();

		info.title	= in.InputString(30);
		info.artist	= in.InputString(30);
		info.album	= in.InputString(30);
		info.year	= in.InputString(4).ToInt();
		info.comment	= in.InputString(28);

		if (in.InputNumber(1) == 0)
		{
			Int	 n = in.InputNumber(1);

			if (n > 0) info.track = n;
		}
		else
		{
			in.RelSeek(-29);

			info.comment = in.InputString(30);
		}

		info.genre	= GetID3CategoryName(in.InputNumber(1));

		track.SetInfo(info);

		String::SetInputFormat(prevInFormat);

		return Success();
	}

	return Error();
}

Error BoCA::TaggerID3v1::ParseStreamInfo(const String &fileName, Track &track)
{
	InStream		 in(STREAM_FILE, fileName, IS_READ);
	Buffer<UnsignedByte>	 buffer(128);

	/* Copy tag to buffer and parse it.
	 */
	in.Seek(in.Size() - 128);
	in.InputData(buffer, 128);

	return ParseBuffer(buffer, track);
}

Error BoCA::TaggerID3v1::UpdateStreamInfo(const String &fileName, const Track &track)
{
	Int		 offset = 0;
	InStream	 in(STREAM_FILE, fileName, IS_READ);

	in.Seek(in.Size() - 128);

	if (in.InputString(3) == "TAG") offset = -128;

	in.Close();

	OutStream	 out(STREAM_FILE, fileName, OS_APPEND);

	if (out.GetLastError() == IO_ERROR_OK)
	{
		out.RelSeek(offset);

		Buffer<UnsignedByte>	 buffer(128);

		RenderBuffer(buffer, track);

		out.OutputData(buffer, 128);

		return Success();
	}

	return Error();
}

const String &BoCA::TaggerID3v1::GetID3CategoryName(UnsignedInt id)
{
	static const String	 empty;

	if (id > 191) return empty;
	else	      return genres[id];
}

UnsignedInt BoCA::TaggerID3v1::GetID3CategoryID(const String &name)
{
	for (UnsignedInt i = 0; i < 192; i++)
	{
		if (genres[i] == name) return i;
	}

	return 12; // Other
}
