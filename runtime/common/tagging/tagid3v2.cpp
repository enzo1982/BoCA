 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/tagging/tagid3v2.h>
#include <boca/common/config.h>

using namespace smooth::IO;

const char *INFO_BAND		= (char *) "Band/orchestra/accompaniment";
const char *INFO_PERFORMER	= (char *) "Conductor/performer refinement";
const char *INFO_REMIX		= (char *) "Interpreted, remixed or otherwise modified by";
const char *INFO_COMPOSER	= (char *) "Composer";
const char *INFO_LYRICIST	= (char *) "Lyricist/Text writer";

const char *INFO_ORIG_ARTIST	= (char *) "Original artist(s)/performer(s)";
const char *INFO_ORIG_ALBUM	= (char *) "Original album/movie/show title";
const char *INFO_ORIG_LYRICIST	= (char *) "Original lyricist(s)/text writer(s)";
const char *INFO_ORIG_YEAR	= (char *) "Original release year";

const String	 BoCA::TagID3v2::genres[148] =
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

String		 BoCA::TagID3v2::dummyString;

BoCA::TagID3v2::TagID3v2()
{
}

BoCA::TagID3v2::~TagID3v2()
{
}

Int BoCA::TagID3v2::Render(const Track &track, Buffer<UnsignedByte> &buffer)
{
	Config			*currentConfig = Config::Get();

	ID3Tag			*tag = ex_ID3Tag_New();
	const Info		&info = track.GetInfo();

	ex_ID3Tag_SetPadding(tag, false);

	Array<ID3Frame *>	 frames;

	if (info.artist != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_LEADARTIST));  SetFrameString(frames.GetLast(), info.artist);		   ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if (info.title  != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_TITLE));	     SetFrameString(frames.GetLast(), info.title);		   ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if (info.album  != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_ALBUM));	     SetFrameString(frames.GetLast(), info.album);		   ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if (info.year    >   0) { frames.Add(ex_ID3Frame_NewID(ID3FID_YEAR));	     SetFrameString(frames.GetLast(), String::FromInt(info.year)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if (info.genre  != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_CONTENTTYPE)); SetFrameString(frames.GetLast(), info.genre);		   ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if (info.label  != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_PUBLISHER));   SetFrameString(frames.GetLast(), info.label);		   ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if (info.isrc   != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_ISRC));	     SetFrameString(frames.GetLast(), info.isrc);		   ex_ID3Tag_AddFrame(tag, frames.GetLast()); }

	if (info.track > 0)
	{
		String	 trackString = String(info.track < 10 ? "0" : "").Append(String::FromInt(info.track));

		if (info.numTracks > 0) trackString.Append("/").Append(info.numTracks < 10 ? "0" : "").Append(String::FromInt(info.numTracks));

		{ frames.Add(ex_ID3Frame_NewID(ID3FID_TRACKNUM)); SetFrameString(frames.GetLast(), trackString); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	}

	if (info.disc > 0 && (info.numDiscs > 1 || info.disc > 1))
	{
		String	 discString = String(info.disc < 10 ? "0" : "").Append(String::FromInt(info.disc));

		if (info.numDiscs > 0) discString.Append("/").Append(info.numDiscs < 10 ? "0" : "").Append(String::FromInt(info.numDiscs));

		{ frames.Add(ex_ID3Frame_NewID(ID3FID_PARTINSET)); SetFrameString(frames.GetLast(), discString); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	}

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	{ frames.Add(ex_ID3Frame_NewID(ID3FID_COMMENT)); SetFrameString(frames.GetLast(), info.comment);						 ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				{ frames.Add(ex_ID3Frame_NewID(ID3FID_COMMENT)); SetFrameString(frames.GetLast(), currentConfig->GetStringValue("Tags", "DefaultComment", NIL)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }

	/* Save other text info.
	 */
	for (Int i = 0; i < info.other.Length(); i++)
	{
		String	 value = info.other.GetNth(i);

		if	(value.StartsWith(String(INFO_BAND).Append(":")))	   { frames.Add(ex_ID3Frame_NewID(ID3FID_BAND));	 SetFrameString(frames.GetLast(), value.Tail(value.Length() - value.Find(":") - 1)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
		else if	(value.StartsWith(String(INFO_PERFORMER).Append(":")))	   { frames.Add(ex_ID3Frame_NewID(ID3FID_CONDUCTOR));	 SetFrameString(frames.GetLast(), value.Tail(value.Length() - value.Find(":") - 1)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
		else if	(value.StartsWith(String(INFO_REMIX).Append(":")))	   { frames.Add(ex_ID3Frame_NewID(ID3FID_MIXARTIST));	 SetFrameString(frames.GetLast(), value.Tail(value.Length() - value.Find(":") - 1)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
		else if	(value.StartsWith(String(INFO_COMPOSER).Append(":")))	   { frames.Add(ex_ID3Frame_NewID(ID3FID_COMPOSER));	 SetFrameString(frames.GetLast(), value.Tail(value.Length() - value.Find(":") - 1)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
		else if	(value.StartsWith(String(INFO_LYRICIST).Append(":")))	   { frames.Add(ex_ID3Frame_NewID(ID3FID_LYRICIST));	 SetFrameString(frames.GetLast(), value.Tail(value.Length() - value.Find(":") - 1)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }

		else if	(value.StartsWith(String(INFO_ORIG_ARTIST).Append(":")))   { frames.Add(ex_ID3Frame_NewID(ID3FID_ORIGARTIST));	 SetFrameString(frames.GetLast(), value.Tail(value.Length() - value.Find(":") - 1)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
		else if	(value.StartsWith(String(INFO_ORIG_ALBUM).Append(":")))    { frames.Add(ex_ID3Frame_NewID(ID3FID_ORIGALBUM));	 SetFrameString(frames.GetLast(), value.Tail(value.Length() - value.Find(":") - 1)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
		else if	(value.StartsWith(String(INFO_ORIG_LYRICIST).Append(":"))) { frames.Add(ex_ID3Frame_NewID(ID3FID_ORIGLYRICIST)); SetFrameString(frames.GetLast(), value.Tail(value.Length() - value.Find(":") - 1)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
		else if	(value.StartsWith(String(INFO_ORIG_YEAR).Append(":")))	   { frames.Add(ex_ID3Frame_NewID(ID3FID_ORIGYEAR));	 SetFrameString(frames.GetLast(), value.Tail(value.Length() - value.Find(":") - 1)); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	}

	/* Save Replay Gain info.
	 */
	if (currentConfig->GetIntValue("Tags", "PreserveReplayGain", True))
	{
		if (info.track_gain != NIL && info.track_peak != NIL)
		{
			{ frames.Add(ex_ID3Frame_NewID(ID3FID_USERTEXT)); SetFrameString(frames.GetLast(), info.track_gain, "replaygain_track_gain"); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
			{ frames.Add(ex_ID3Frame_NewID(ID3FID_USERTEXT)); SetFrameString(frames.GetLast(), info.track_peak, "replaygain_track_peak"); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
		}

		if (info.album_gain != NIL && info.album_peak != NIL)
		{
			{ frames.Add(ex_ID3Frame_NewID(ID3FID_USERTEXT)); SetFrameString(frames.GetLast(), info.album_gain, "replaygain_album_gain"); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
			{ frames.Add(ex_ID3Frame_NewID(ID3FID_USERTEXT)); SetFrameString(frames.GetLast(), info.album_peak, "replaygain_album_peak"); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
		}
	}

	/* Save CD table of contents.
	 */
	if (currentConfig->GetIntValue("Tags", "WriteMCDI", True))
	{
		if (info.mcdi.Size() > 0)
		{
			ID3Frame	*frame_mcdi = ex_ID3Frame_NewID(ID3FID_CDID);

			ex_ID3Field_SetBINARY(ex_ID3Frame_GetField(frame_mcdi, ID3FN_DATA), info.mcdi, info.mcdi.Size());

			ex_ID3Tag_AddFrame(tag, frame_mcdi);

			frames.Add(frame_mcdi);
		}
	}

	/* Save cover art.
	 */
	if (currentConfig->GetIntValue("Tags", "WriteCoverArt", True) && currentConfig->GetIntValue("Tags", "WriteCoverArtID3v2", True))
	{
		foreach (const Picture &picInfo, track.pictures)
		{
			ID3Frame	*frame_picture = ex_ID3Frame_NewID(ID3FID_PICTURE);

			SetFrameString(frame_picture, NIL, picInfo.description);

			ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_picture, ID3FN_MIMETYPE), picInfo.mime.ConvertTo("ISO-8859-1"));
			ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_picture, ID3FN_PICTURETYPE), picInfo.type);
			ex_ID3Field_SetBINARY(ex_ID3Frame_GetField(frame_picture, ID3FN_DATA), picInfo.data, picInfo.data.Size());

			ex_ID3Tag_AddFrame(tag, frame_picture);

			frames.Add(frame_picture);
		}
	}

	buffer.Resize(ex_ID3Tag_Size(tag));

	Int	 size = ex_ID3Tag_Render(tag, buffer, ID3TT_ID3V2);

	ex_ID3Tag_Delete(tag);

	for (Int i = 0; i < frames.Length(); i++)
	{
		ex_ID3Frame_Delete(frames.GetNth(i));
	}

	return size;
}

Int BoCA::TagID3v2::Parse(const Buffer<UnsignedByte> &buffer, Track *track)
{
	ID3Tag		*tag = ex_ID3Tag_New();

	ex_ID3Tag_ParseBuffer(tag, buffer, buffer.Size());

	ID3TagIterator	*iterator = ex_ID3Tag_CreateIterator(tag);

	Info		&info = track->GetInfo();

	for (UnsignedInt i = 0; i < ex_ID3Tag_NumFrames(tag); i++)
	{
		ID3Frame	*frame = ex_ID3TagIterator_GetNext(iterator);

		if	(ex_ID3Frame_GetID(frame) == ID3FID_LEADARTIST)	  info.artist	= GetFrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_TITLE)	  info.title	= GetFrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_ALBUM)	  info.album	= GetFrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_YEAR)	  info.year	= GetFrameString(frame).ToInt();
		else if (ex_ID3Frame_GetID(frame) == ID3FID_COMMENT)	  info.comment	= GetFrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_PUBLISHER)	  info.label	= GetFrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_ISRC)	  info.isrc	= GetFrameString(frame);

		else if (ex_ID3Frame_GetID(frame) == ID3FID_BAND)	  info.other.Add(String(INFO_BAND).Append(":").Append(GetFrameString(frame)));
		else if (ex_ID3Frame_GetID(frame) == ID3FID_CONDUCTOR)	  info.other.Add(String(INFO_PERFORMER).Append(":").Append(GetFrameString(frame)));
		else if (ex_ID3Frame_GetID(frame) == ID3FID_MIXARTIST)	  info.other.Add(String(INFO_REMIX).Append(":").Append(GetFrameString(frame)));
		else if (ex_ID3Frame_GetID(frame) == ID3FID_COMPOSER)	  info.other.Add(String(INFO_COMPOSER).Append(":").Append(GetFrameString(frame)));
		else if (ex_ID3Frame_GetID(frame) == ID3FID_LYRICIST)	  info.other.Add(String(INFO_LYRICIST).Append(":").Append(GetFrameString(frame)));

		else if (ex_ID3Frame_GetID(frame) == ID3FID_ORIGARTIST)	  info.other.Add(String(INFO_ORIG_ARTIST).Append(":").Append(GetFrameString(frame)));
		else if (ex_ID3Frame_GetID(frame) == ID3FID_ORIGALBUM)	  info.other.Add(String(INFO_ORIG_ALBUM).Append(":").Append(GetFrameString(frame)));
		else if (ex_ID3Frame_GetID(frame) == ID3FID_ORIGLYRICIST) info.other.Add(String(INFO_ORIG_LYRICIST).Append(":").Append(GetFrameString(frame)));
		else if (ex_ID3Frame_GetID(frame) == ID3FID_ORIGYEAR)	  info.other.Add(String(INFO_ORIG_YEAR).Append(":").Append(GetFrameString(frame)));

		else if (ex_ID3Frame_GetID(frame) == ID3FID_TRACKNUM)
		{
			String	 trackString = GetFrameString(frame);

			info.track = trackString.ToInt();

			if (trackString.Find("/") >= 0) info.numTracks = trackString.Tail(trackString.Length() - trackString.Find("/") - 1).ToInt();
		}
		else if (ex_ID3Frame_GetID(frame) == ID3FID_PARTINSET)
		{
			String	 discString = GetFrameString(frame);

			info.disc = discString.ToInt();

			if (discString.Find("/") >= 0) info.numDiscs = discString.Tail(discString.Length() - discString.Find("/") - 1).ToInt();
		}
		else if (ex_ID3Frame_GetID(frame) == ID3FID_USERTEXT)
		{
			String	 description;
			String	 value = GetFrameString(frame, description);

			if	(description.ToLower() == "replaygain_track_gain") info.track_gain = value;
			else if (description.ToLower() == "replaygain_track_peak") info.track_peak = value;
			else if (description.ToLower() == "replaygain_album_gain") info.album_gain = value;
			else if (description.ToLower() == "replaygain_album_peak") info.album_peak = value;
		}
		else if (ex_ID3Frame_GetID(frame) == ID3FID_CONTENTTYPE)
		{
			String	 s_genre = GetFrameString(frame);
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
		else if (ex_ID3Frame_GetID(frame) == ID3FID_CDID)
		{
			ID3Field	*field = ex_ID3Frame_GetField(frame, ID3FN_DATA);

			if (field != NIL)
			{
				Buffer<UnsignedByte>	 mcdi(ex_ID3Field_Size(field));

				ex_ID3Field_GetBINARY(field, mcdi, mcdi.Size());

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
					info.mcdi.Resize(mcdi.Size());

					memcpy(info.mcdi, mcdi, mcdi.Size());
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
		}
		else if (ex_ID3Frame_GetID(frame) == ID3FID_PICTURE)
		{
			Picture		 picture;
			ID3Field	*field = ex_ID3Frame_GetField(frame, ID3FN_MIMETYPE);

			if (field != NIL)
			{
				Buffer<char>	 aBuffer(1024);

				aBuffer.Zero();

				ex_ID3Field_GetASCII(field, aBuffer, aBuffer.Size());

				picture.mime.ImportFrom("ISO-8859-1", aBuffer);

				if	(picture.mime.ToLower() == "jpeg" || picture.mime.ToLower() == "jpg") picture.mime = "image/jpeg";
				else if (picture.mime.ToLower() == "png")				      picture.mime = "image/png";
			}

			field = ex_ID3Frame_GetField(frame, ID3FN_PICTURETYPE);

			if (field != NIL)
			{
				picture.type = ex_ID3Field_GetINT(field);
			}

			GetFrameString(frame, picture.description);

			field = ex_ID3Frame_GetField(frame, ID3FN_DATA);

			if (field != NIL)
			{
				picture.data.Resize(ex_ID3Field_Size(field));

				if (picture.data.Size() > 16 && picture.data[0] != 0 && picture.data[1] != 0)
				{
					ex_ID3Field_GetBINARY(field, picture.data, picture.data.Size());

					track->pictures.Add(picture);
				}
			}
		}
	}

	ex_ID3TagIterator_Delete(iterator);

	ex_ID3Tag_Delete(tag);

	return Success();
}

Int BoCA::TagID3v2::Parse(const String &fileName, Track *track)
{
	InStream	 in(STREAM_FILE, fileName, IS_READONLY);

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

		return Parse(buffer, track);
	}

	return Error();
}

String BoCA::TagID3v2::GetFrameString(ID3Frame *frame, String &description)
{
	ID3Field	*field = ex_ID3Frame_GetField(frame, ID3FN_TEXTENC);
	String		 result;

	if (field != NIL)
	{
		Int		 encoding = ex_ID3Field_GetINT(field);

		Buffer<char>	 aBuffer(1024);
		Buffer<wchar_t>	 wBuffer(1024);

		aBuffer.Zero();
		wBuffer.Zero();

		if (encoding == ID3TE_ISO8859_1 || encoding == ID3TE_UTF8)
		{
			if ((field = ex_ID3Frame_GetField(frame, ID3FN_TEXT)) != NIL)
			{
				ex_ID3Field_GetASCII(field, aBuffer, aBuffer.Size());

				if	(encoding == ID3TE_ISO8859_1)	result.ImportFrom("ISO-8859-1", aBuffer);
				else if (encoding == ID3TE_UTF8)	result.ImportFrom("UTF-8", aBuffer);
			}

			if ((field = ex_ID3Frame_GetField(frame, ID3FN_DESCRIPTION)) != NIL)
			{
				ex_ID3Field_GetASCII(field, aBuffer, aBuffer.Size());

				if	(encoding == ID3TE_ISO8859_1)	description.ImportFrom("ISO-8859-1", aBuffer);
				else if (encoding == ID3TE_UTF8)	description.ImportFrom("UTF-8", aBuffer);
			}
		}
		else if (encoding == ID3TE_UTF16 || encoding == ID3TE_UTF16BE)
		{
			if ((field = ex_ID3Frame_GetField(frame, ID3FN_TEXT)) != NIL)
			{
				ex_ID3Field_GetUNICODE(field, (unicode_t *) (wchar_t *) wBuffer, wBuffer.Size());

				result.ImportFrom("UTF-16BE", (char *) (wchar_t *) wBuffer);
			}

			if ((field = ex_ID3Frame_GetField(frame, ID3FN_DESCRIPTION)) != NIL)
			{
				ex_ID3Field_GetUNICODE(field, (unicode_t *) (wchar_t *) wBuffer, wBuffer.Size());

				description.ImportFrom("UTF-16BE", (char *) (wchar_t *) wBuffer);
			}
		}
	}

	return result;
}

Int BoCA::TagID3v2::SetFrameString(ID3Frame *frame, const String &text, const String &description)
{
	String		 leBOM;

	leBOM[0] = 0xFEFF;

	ID3_TextEnc	 encoding = ID3TE_NONE;
	String		 encString = Config::Get()->GetStringValue("Tags", "ID3v2Encoding", "UTF-16LE");

	if	(encString == "UTF-8")		encoding = ID3TE_UTF8;
	else if (encString == "ISO-8859-1")	encoding = ID3TE_ISO8859_1;
	else if (encString == "UTF-16"	 ||
		 encString == "UTF-16LE" ||
		 encString == "UCS-2"	 ||
		 encString == "UCS-2LE")	encoding = ID3TE_UTF16;
	else if (encString == "UTF-16BE" ||
		 encString == "UCS-2BE")	encoding = ID3TE_UTF16BE;

	char		*prevOutFormat = String::SetOutputFormat(encString);

	ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame, ID3FN_TEXTENC), encoding);

	if (text != NIL)
	{
		ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame, ID3FN_TEXT), encoding);

		if	(encoding == ID3TE_UTF16)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame, ID3FN_TEXT), (unicode_t *) String(leBOM).Append(text).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame, ID3FN_TEXT), (unicode_t *) text.ConvertTo("UTF-16BE"));
		else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame, ID3FN_TEXT), text);
	}

	if (description != NIL)
	{
		ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame, ID3FN_DESCRIPTION), encoding);

		if	(encoding == ID3TE_UTF16)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame, ID3FN_DESCRIPTION), (unicode_t *) String(leBOM).Append(description).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame, ID3FN_DESCRIPTION), (unicode_t *) description.ConvertTo("UTF-16BE"));
		else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame, ID3FN_DESCRIPTION), description);
	}

	String::SetOutputFormat(prevOutFormat);

	return Success();
}

const String &BoCA::TagID3v2::GetID3CategoryName(Int id)
{
	static const String	 empty;

	if (id < 0 || id > 147) return empty;
	else			return genres[id];
}
