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
#include <id3/tag.h>

#include "id3v2_tag.h"

using namespace smooth::IO;

const String &BoCA::ID3v2Tag::GetComponentSpecs()
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
	    <tagformat>						\
	      <name>ID3v2</name>				\
	      <coverart supported=\"true\"/>			\
	      <encodings free=\"true\">				\
		<encoding>ISO-8859-1</encoding>			\
		<encoding>UTF-8</encoding>			\
		<encoding default=\"true\">UTF-16LE</encoding>	\
		<encoding>UTF-16BE</encoding>			\
	      </encodings>					\
	    </tagformat>					\
	  </component>						\
								\
	";

	return componentSpecs;
}

const String	 BoCA::ID3v2Tag::genres[148] =
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
	"Christian Rock", "Merengue", "Salsa", "Thrash Metal", "Anime", "JPop", "Synthpop" };

String		 BoCA::ID3v2Tag::dummyString;

BoCA::ID3v2Tag::ID3v2Tag()
{
}

BoCA::ID3v2Tag::~ID3v2Tag()
{
}

Error BoCA::ID3v2Tag::RenderBuffer(Buffer<UnsignedByte> &buffer, const Track &track)
{
	Config			*currentConfig = Config::Get();

	ID3_Tag			*tag = new ID3_Tag();
	const Info		&info = track.GetInfo();

	tag->SetPadding(false);

	Array<ID3_Frame *>	 frames;

	if (info.artist != NIL) { frames.Add(new ID3_Frame(ID3FID_LEADARTIST));  SetStringField(frames.GetLast(), ID3FN_TEXT, info.artist);		   tag->AddFrame(frames.GetLast()); }
	if (info.title  != NIL) { frames.Add(new ID3_Frame(ID3FID_TITLE));	 SetStringField(frames.GetLast(), ID3FN_TEXT, info.title);		   tag->AddFrame(frames.GetLast()); }
	if (info.album  != NIL) { frames.Add(new ID3_Frame(ID3FID_ALBUM));	 SetStringField(frames.GetLast(), ID3FN_TEXT, info.album);		   tag->AddFrame(frames.GetLast()); }
	if (info.year    >   0) { frames.Add(new ID3_Frame(ID3FID_YEAR));	 SetStringField(frames.GetLast(), ID3FN_TEXT, String::FromInt(info.year)); tag->AddFrame(frames.GetLast()); }
	if (info.genre  != NIL) { frames.Add(new ID3_Frame(ID3FID_CONTENTTYPE)); SetStringField(frames.GetLast(), ID3FN_TEXT, info.genre);		   tag->AddFrame(frames.GetLast()); }
	if (info.label  != NIL) { frames.Add(new ID3_Frame(ID3FID_PUBLISHER));   SetStringField(frames.GetLast(), ID3FN_TEXT, info.label);		   tag->AddFrame(frames.GetLast()); }
	if (info.isrc   != NIL) { frames.Add(new ID3_Frame(ID3FID_ISRC));	 SetStringField(frames.GetLast(), ID3FN_TEXT, info.isrc);		   tag->AddFrame(frames.GetLast()); }

	if (info.track > 0)
	{
		String	 trackString = String(info.track < 10 ? "0" : "").Append(String::FromInt(info.track));

		if (info.numTracks > 0) trackString.Append("/").Append(info.numTracks < 10 ? "0" : "").Append(String::FromInt(info.numTracks));

		{ frames.Add(new ID3_Frame(ID3FID_TRACKNUM)); SetStringField(frames.GetLast(), ID3FN_TEXT, trackString); tag->AddFrame(frames.GetLast()); }
	}

	if (info.disc > 0 && (info.numDiscs > 1 || info.disc > 1))
	{
		String	 discString = String(info.disc < 10 ? "0" : "").Append(String::FromInt(info.disc));

		if (info.numDiscs > 0) discString.Append("/").Append(info.numDiscs < 10 ? "0" : "").Append(String::FromInt(info.numDiscs));

		{ frames.Add(new ID3_Frame(ID3FID_PARTINSET)); SetStringField(frames.GetLast(), ID3FN_TEXT, discString); tag->AddFrame(frames.GetLast()); }
	}

	if (info.rating > 0)
	{
		Int	 rating = Math::Min(255, info.rating * 256 / 100);

		{ frames.Add(new ID3_Frame(ID3FID_POPULARIMETER)); SetASCIIField(frames.GetLast(), ID3FN_EMAIL, "rating@freac.org"); SetIntegerField(frames.GetLast(), ID3FN_RATING, rating); tag->AddFrame(frames.GetLast()); }
	}

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	{ frames.Add(new ID3_Frame(ID3FID_COMMENT)); SetStringField(frames.GetLast(), ID3FN_TEXT, info.comment);						 tag->AddFrame(frames.GetLast()); }
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				{ frames.Add(new ID3_Frame(ID3FID_COMMENT)); SetStringField(frames.GetLast(), ID3FN_TEXT, currentConfig->GetStringValue("Tags", "DefaultComment", NIL)); tag->AddFrame(frames.GetLast()); }

	/* Save other text info.
	 */
	for (Int i = 0; i < info.other.Length(); i++)
	{
		String	 pair  = info.other.GetNth(i);

		String	 key   = pair.Head(pair.Find(":") + 1);
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == String(INFO_CONTENTGROUP).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_CONTENTGROUP));	SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_SUBTITLE).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_SUBTITLE));		SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }

		else if	(key == String(INFO_BAND).Append(":"))		 { frames.Add(new ID3_Frame(ID3FID_BAND));		SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_CONDUCTOR).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_CONDUCTOR));		SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_REMIX).Append(":"))		 { frames.Add(new ID3_Frame(ID3FID_MIXARTIST));		SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_COMPOSER).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_COMPOSER));		SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_LYRICIST).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_LYRICIST));		SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }

		else if	(key == String(INFO_ORIG_ARTIST).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_ORIGARTIST));	SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_ORIG_ALBUM).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_ORIGALBUM));		SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_ORIG_LYRICIST).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_ORIGLYRICIST));	SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_ORIG_YEAR).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_ORIGYEAR));		SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }

		else if	(key == String(INFO_BPM).Append(":"))		 { frames.Add(new ID3_Frame(ID3FID_BPM));		SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_INITIALKEY).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_INITIALKEY));	SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }

		else if	(key == String(INFO_RADIOSTATION).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_NETRADIOSTATION));   SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_RADIOOWNER).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_NETRADIOOWNER));	SetStringField(frames.GetLast(), ID3FN_TEXT, value); tag->AddFrame(frames.GetLast()); }

		else if	(key == String(INFO_WEB_ARTIST).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_WWWARTIST));		SetASCIIField(frames.GetLast(), ID3FN_URL, value);   tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_WEB_PUBLISHER).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_WWWPUBLISHER));	SetASCIIField(frames.GetLast(), ID3FN_URL, value);   tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_WEB_RADIO).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_WWWRADIOPAGE));	SetASCIIField(frames.GetLast(), ID3FN_URL, value);   tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_WEB_SOURCE).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_WWWAUDIOSOURCE));    SetASCIIField(frames.GetLast(), ID3FN_URL, value);   tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_WEB_COPYRIGHT).Append(":"))	 { frames.Add(new ID3_Frame(ID3FID_WWWCOPYRIGHT));	SetASCIIField(frames.GetLast(), ID3FN_URL, value);   tag->AddFrame(frames.GetLast()); }
		else if	(key == String(INFO_WEB_COMMERCIAL).Append(":")) { frames.Add(new ID3_Frame(ID3FID_WWWCOMMERCIALINFO)); SetASCIIField(frames.GetLast(), ID3FN_URL, value);   tag->AddFrame(frames.GetLast()); }
	}

	/* Save Replay Gain info.
	 */
	if (currentConfig->GetIntValue("Tags", "PreserveReplayGain", True))
	{
		if (info.track_gain != NIL && info.track_peak != NIL)
		{
			{ frames.Add(new ID3_Frame(ID3FID_USERTEXT)); SetStringField(frames.GetLast(), ID3FN_TEXT, info.track_gain); SetStringField(frames.GetLast(), ID3FN_DESCRIPTION, "replaygain_track_gain"); tag->AddFrame(frames.GetLast()); }
			{ frames.Add(new ID3_Frame(ID3FID_USERTEXT)); SetStringField(frames.GetLast(), ID3FN_TEXT, info.track_peak); SetStringField(frames.GetLast(), ID3FN_DESCRIPTION, "replaygain_track_peak"); tag->AddFrame(frames.GetLast()); }
		}

		if (info.album_gain != NIL && info.album_peak != NIL)
		{
			{ frames.Add(new ID3_Frame(ID3FID_USERTEXT)); SetStringField(frames.GetLast(), ID3FN_TEXT, info.album_gain); SetStringField(frames.GetLast(), ID3FN_DESCRIPTION, "replaygain_album_gain"); tag->AddFrame(frames.GetLast()); }
			{ frames.Add(new ID3_Frame(ID3FID_USERTEXT)); SetStringField(frames.GetLast(), ID3FN_TEXT, info.album_peak); SetStringField(frames.GetLast(), ID3FN_DESCRIPTION, "replaygain_album_peak"); tag->AddFrame(frames.GetLast()); }
		}
	}

	/* Save CD table of contents.
	 */
	if (currentConfig->GetIntValue("Tags", "WriteMCDI", True))
	{
		if (info.mcdi.GetData().Size() > 0)
		{
			ID3_Frame	*frame_mcdi = new ID3_Frame(ID3FID_CDID);

			SetBinaryField(frame_mcdi, ID3FN_DATA, info.mcdi.GetData());

			tag->AddFrame(frame_mcdi);

			frames.Add(frame_mcdi);
		}
	}

	/* Save cover art.
	 */
	if (currentConfig->GetIntValue("Tags", "CoverArtWriteToTags", True) && currentConfig->GetIntValue("Tags", "CoverArtWriteToID3v2", True))
	{
		foreach (const Picture &picInfo, track.pictures)
		{
			ID3_Frame	*frame_picture = new ID3_Frame(ID3FID_PICTURE);

			/* Set the description field and try to stay compatible with
			 * iTunes which expects it to be in a single byte encoding.
			 */
			String		 encoding = currentConfig->GetStringValue("Tags", "ID3v2Encoding", "UTF-16LE");

			if (encoding != "UTF-8" && !String::IsUnicode(picInfo.description))
			{
				currentConfig->SetStringValue("Tags", "ID3v2Encoding", "ISO-8859-1");

				SetStringField(frame_picture, ID3FN_DESCRIPTION, picInfo.description);

				currentConfig->SetStringValue("Tags", "ID3v2Encoding", encoding);
			}
			else
			{
				SetStringField(frame_picture, ID3FN_DESCRIPTION, picInfo.description);
			}

			/* Set picture data.
			 */
			SetIntegerField(frame_picture, ID3FN_PICTURETYPE, picInfo.type);
			SetASCIIField(frame_picture, ID3FN_MIMETYPE, picInfo.mime.ConvertTo("ISO-8859-1"));
			SetBinaryField(frame_picture, ID3FN_DATA, picInfo.data);

			tag->AddFrame(frame_picture);

			frames.Add(frame_picture);
		}
	}

	buffer.Resize(tag->Size());

	Int	 size = tag->Render(buffer, ID3TT_ID3V2);

	buffer.Resize(size);

	delete tag;

	for (Int i = 0; i < frames.Length(); i++)
	{
		delete frames.GetNth(i);
	}

	return Success();
}

Error BoCA::ID3v2Tag::ParseBuffer(const Buffer<UnsignedByte> &buffer, Track &track)
{
	Config			*currentConfig = Config::Get();

	ID3_Tag			*tag = new ID3_Tag();

	tag->Parse(buffer, buffer.Size());

	ID3_Tag::Iterator	*iterator = tag->CreateIterator();

	Info			 info = track.GetInfo();

	for (UnsignedInt i = 0; i < tag->NumFrames(); i++)
	{
		ID3_Frame	*frame = iterator->GetNext();

		if	(frame->GetID() == ID3FID_LEADARTIST)	     info.artist  = GetStringField(frame, ID3FN_TEXT);
		else if (frame->GetID() == ID3FID_TITLE)	     info.title	  = GetStringField(frame, ID3FN_TEXT);
		else if (frame->GetID() == ID3FID_ALBUM)	     info.album	  = GetStringField(frame, ID3FN_TEXT);
		else if (frame->GetID() == ID3FID_YEAR)		     info.year	  = GetStringField(frame, ID3FN_TEXT).ToInt();
		else if (frame->GetID() == ID3FID_RELEASETIME)	     info.year	  = GetStringField(frame, ID3FN_TEXT).Head(4).ToInt();
		else if (frame->GetID() == ID3FID_COMMENT)	     info.comment = GetStringField(frame, ID3FN_TEXT);
		else if (frame->GetID() == ID3FID_PUBLISHER)	     info.label	  = GetStringField(frame, ID3FN_TEXT);
		else if (frame->GetID() == ID3FID_ISRC)		     info.isrc	  = GetStringField(frame, ID3FN_TEXT);

		else if (frame->GetID() == ID3FID_CONTENTGROUP)	     info.other.Add(String(INFO_CONTENTGROUP).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_SUBTITLE)	     info.other.Add(String(INFO_SUBTITLE).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame->GetID() == ID3FID_BAND)		     info.other.Add(String(INFO_BAND).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_CONDUCTOR)	     info.other.Add(String(INFO_CONDUCTOR).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_MIXARTIST)	     info.other.Add(String(INFO_REMIX).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_COMPOSER)	     info.other.Add(String(INFO_COMPOSER).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_LYRICIST)	     info.other.Add(String(INFO_LYRICIST).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame->GetID() == ID3FID_ORIGARTIST)	     info.other.Add(String(INFO_ORIG_ARTIST).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_ORIGALBUM)	     info.other.Add(String(INFO_ORIG_ALBUM).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_ORIGLYRICIST)	     info.other.Add(String(INFO_ORIG_LYRICIST).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_ORIGYEAR)	     info.other.Add(String(INFO_ORIG_YEAR).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame->GetID() == ID3FID_BPM)		     info.other.Add(String(INFO_BPM).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_INITIALKEY)	     info.other.Add(String(INFO_INITIALKEY).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame->GetID() == ID3FID_NETRADIOSTATION)   info.other.Add(String(INFO_RADIOSTATION).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));
		else if (frame->GetID() == ID3FID_NETRADIOOWNER)     info.other.Add(String(INFO_RADIOOWNER).Append(":").Append(GetStringField(frame, ID3FN_TEXT)));

		else if (frame->GetID() == ID3FID_WWWARTIST)	     info.other.Add(String(INFO_WEB_ARTIST).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame->GetID() == ID3FID_WWWPUBLISHER)	     info.other.Add(String(INFO_WEB_PUBLISHER).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame->GetID() == ID3FID_WWWRADIOPAGE)	     info.other.Add(String(INFO_WEB_RADIO).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame->GetID() == ID3FID_WWWAUDIOSOURCE)    info.other.Add(String(INFO_WEB_SOURCE).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame->GetID() == ID3FID_WWWCOPYRIGHT)	     info.other.Add(String(INFO_WEB_COPYRIGHT).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));
		else if (frame->GetID() == ID3FID_WWWCOMMERCIALINFO) info.other.Add(String(INFO_WEB_COMMERCIAL).Append(":").Append(GetASCIIField(frame, ID3FN_URL)));

		else if (frame->GetID() == ID3FID_TRACKNUM)
		{
			String	 trackString = GetStringField(frame, ID3FN_TEXT);

			info.track = trackString.ToInt();

			if (trackString.Find("/") >= 0) info.numTracks = trackString.Tail(trackString.Length() - trackString.Find("/") - 1).ToInt();
		}
		else if (frame->GetID() == ID3FID_PARTINSET)
		{
			String	 discString = GetStringField(frame, ID3FN_TEXT);

			info.disc = discString.ToInt();

			if (discString.Find("/") >= 0) info.numDiscs = discString.Tail(discString.Length() - discString.Find("/") - 1).ToInt();
		}
		else if (frame->GetID() == ID3FID_POPULARIMETER)
		{
			Int	 rating = GetIntegerField(frame, ID3FN_RATING);

			if (rating > 0) info.rating = Math::Max(1, Math::Min(255, rating * 100 / 255));
		}
		else if (frame->GetID() == ID3FID_USERTEXT)
		{
			String	 description = GetStringField(frame, ID3FN_DESCRIPTION);
			String	 value	     = GetStringField(frame, ID3FN_TEXT);

			if	(description.ToLower() == "replaygain_track_gain") info.track_gain = value;
			else if (description.ToLower() == "replaygain_track_peak") info.track_peak = value;
			else if (description.ToLower() == "replaygain_album_gain") info.album_gain = value;
			else if (description.ToLower() == "replaygain_album_peak") info.album_peak = value;
		}
		else if (frame->GetID() == ID3FID_CONTENTTYPE)
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
		else if (frame->GetID() == ID3FID_CDID)
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
		else if (frame->GetID() == ID3FID_PICTURE && currentConfig->GetIntValue("Tags", "CoverArtReadFromTags", True))
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
	delete tag;

	return Success();
}

Error BoCA::ID3v2Tag::ParseStreamInfo(const String &fileName, Track &track)
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

Error BoCA::ID3v2Tag::UpdateStreamInfo(const String &fileName, const Track &track)
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

String BoCA::ID3v2Tag::GetStringField(Void *frame, Int fieldType)
{
	ID3_Field	*field = ((ID3_Frame *) frame)->GetField((ID3_FieldID) fieldType);
	String		 result;

	if (field != NIL)
	{
		Int	 encoding = GetIntegerField((ID3_Frame *) frame, ID3FN_TEXTENC);

		if (encoding == ID3TE_ISO8859_1 || encoding == ID3TE_UTF8)
		{
			Buffer<char>	 aBuffer(1024);

			aBuffer.Zero();

			field->Get(aBuffer, aBuffer.Size());

			if	(encoding == ID3TE_ISO8859_1)	result.ImportFrom("ISO-8859-1", aBuffer);
			else if (encoding == ID3TE_UTF8)	result.ImportFrom("UTF-8", aBuffer);
		}
		else if (encoding == ID3TE_UTF16 || encoding == ID3TE_UTF16BE)
		{
			Buffer<wchar_t>	 wBuffer(1024);

			wBuffer.Zero();

			field->Get((unicode_t *) (wchar_t *) wBuffer, wBuffer.Size());

			result.ImportFrom("UTF-16BE", (char *) (wchar_t *) wBuffer);
		}
	}

	return result;
}

Int BoCA::ID3v2Tag::SetStringField(Void *frame, Int fieldType, const String &string)
{
	if (string == NIL) return Error();

	static String	 leBOM;

	if (leBOM == NIL) leBOM[0] = 0xFEFF;

	ID3_TextEnc	 encoding   = ID3TE_NONE;
	String		 encodingID = Config::Get()->GetStringValue("Tags", "ID3v2Encoding", "UTF-16LE");

	if	(encodingID == "UTF-8")					encoding = ID3TE_UTF8;
	else if (encodingID == "ISO-8859-1")				encoding = ID3TE_ISO8859_1;
	else if (encodingID == "UTF-16"	  || encodingID == "UCS-2" ||
		 encodingID == "UTF-16LE" || encodingID == "UCS-2LE")	encoding = ID3TE_UTF16;
	else if (encodingID == "UTF-16BE" || encodingID == "UCS-2BE")	encoding = ID3TE_UTF16BE;

	String		 prevOutFormat = String::SetOutputFormat(encodingID);

	SetIntegerField(frame, ID3FN_TEXTENC, encoding);

	ID3_Field	*field = ((ID3_Frame *) frame)->GetField((ID3_FieldID) fieldType);

	if (field != NIL)
	{
		field->SetEncoding(encoding);

		if	(encoding == ID3TE_UTF16)	field->Set((unicode_t *) String(leBOM).Append(string).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	field->Set((unicode_t *) string.ConvertTo("UTF-16BE"));
		else					field->Set((char *) string);

		String::SetOutputFormat(prevOutFormat.ConvertTo("ISO-8859-1"));

		return Success();
	}

	String::SetOutputFormat(prevOutFormat.ConvertTo("ISO-8859-1"));

	return Error();
}

String BoCA::ID3v2Tag::GetASCIIField(Void *frame, Int fieldType)
{
	ID3_Field	*field = ((ID3_Frame *) frame)->GetField((ID3_FieldID) fieldType);
	String		 result;

	if (field != NIL)
	{
		Buffer<char>	 aBuffer(1024);

		aBuffer.Zero();

		field->Get(aBuffer, aBuffer.Size());

		result.ImportFrom("ISO-8859-1", aBuffer);
	}

	return result;
}

Int BoCA::ID3v2Tag::SetASCIIField(Void *frame, Int fieldType, const String &string)
{
	if (string == NIL) return Error();

	ID3_Field	*field = ((ID3_Frame *) frame)->GetField((ID3_FieldID) fieldType);

	if (field != NIL)
	{
		field->Set((char *) string);

		return Success();
	}

	return Error();
}

Int BoCA::ID3v2Tag::GetIntegerField(Void *frame, Int fieldType)
{
	ID3_Field	*field = ((ID3_Frame *) frame)->GetField((ID3_FieldID) fieldType);

	if (field != NIL)
	{
		return field->Get();
	}

	return -1;
}

Int BoCA::ID3v2Tag::SetIntegerField(Void *frame, Int fieldType, Int value)
{
	ID3_Field	*field = ((ID3_Frame *) frame)->GetField((ID3_FieldID) fieldType);

	if (field != NIL)
	{
		field->Set(value);

		return Success();
	}

	return Error();
}

Int BoCA::ID3v2Tag::GetBinaryField(Void *frame, Int fieldType, Buffer<UnsignedByte> &buffer)
{
	ID3_Field	*field = ((ID3_Frame *) frame)->GetField((ID3_FieldID) fieldType);

	if (field != NIL)
	{
		buffer.Resize(field->Size());

		field->Get(buffer, buffer.Size());

		return Success();
	}

	return Error();
}

Int BoCA::ID3v2Tag::SetBinaryField(Void *frame, Int fieldType, const Buffer<UnsignedByte> &data)
{
	if (data.Size() == 0) return Error();

	ID3_Field	*field = ((ID3_Frame *) frame)->GetField((ID3_FieldID) fieldType);

	if (field != NIL)
	{
		field->Set(data, data.Size());

		return Success();
	}

	return Error();
}

const String &BoCA::ID3v2Tag::GetID3CategoryName(UnsignedInt id)
{
	static const String	 empty;

	if (id > 147) return empty;
	else	      return genres[id];
}
