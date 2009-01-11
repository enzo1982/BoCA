 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/tagging/tagid3.h>
#include <boca/common/config.h>

using namespace smooth::IO;

BoCA::TagID3::TagID3()
{
	version = 2;
}

BoCA::TagID3::~TagID3()
{
}

Int BoCA::TagID3::Render(const Track &track, Buffer<UnsignedByte> &buffer)
{
	Config			*currentConfig = Config::Get();

	ID3Tag			*tag = ex_ID3Tag_New();

	ex_ID3Tag_SetPadding(tag, false);

	Array<ID3Frame *>	 frames;

	if	(track.artist != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_LEADARTIST));  SetID3v2FrameString(frames.GetLast(), track.artist);								    ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if	(track.title  != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_TITLE));	   SetID3v2FrameString(frames.GetLast(), track.title);								    ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if	(track.album  != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_ALBUM));	   SetID3v2FrameString(frames.GetLast(), track.album);								    ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if	(track.track   >   0) { frames.Add(ex_ID3Frame_NewID(ID3FID_TRACKNUM));    SetID3v2FrameString(frames.GetLast(), String(track.track < 10 ? "0" : "").Append(String::FromInt(track.track))); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if	(track.year    >   0) { frames.Add(ex_ID3Frame_NewID(ID3FID_YEAR));	   SetID3v2FrameString(frames.GetLast(), String::FromInt(track.year));						    ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if	(track.genre  != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_CONTENTTYPE)); SetID3v2FrameString(frames.GetLast(), track.genre);								    ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if	(track.label  != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_PUBLISHER));   SetID3v2FrameString(frames.GetLast(), track.label);								    ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	if	(track.isrc   != NIL) { frames.Add(ex_ID3Frame_NewID(ID3FID_ISRC));	   SetID3v2FrameString(frames.GetLast(), track.isrc);								    ex_ID3Tag_AddFrame(tag, frames.GetLast()); }

	if	(track.comment != NIL && !currentConfig->replace_comments) { frames.Add(ex_ID3Frame_NewID(ID3FID_COMMENT)); SetID3v2FrameString(frames.GetLast(), track.comment);		   ex_ID3Tag_AddFrame(tag, frames.GetLast()); }
	else if (currentConfig->default_comment != NIL)			   { frames.Add(ex_ID3Frame_NewID(ID3FID_COMMENT)); SetID3v2FrameString(frames.GetLast(), currentConfig->default_comment); ex_ID3Tag_AddFrame(tag, frames.GetLast()); }

	if (currentConfig->GetIntValue("Settings", "CopyPictureTags", 1))
	{
		String		 leBOM;

		leBOM[0] = 0xFEFF;

		ID3_TextEnc	 encoding = ID3TE_NONE;
		String		 encString = (version == 1 ? currentConfig->id3v1_encoding : currentConfig->id3v2_encoding);

		if	(encString == "UTF-8")		encoding = ID3TE_UTF8;
		else if (encString == "ISO-8859-1")	encoding = ID3TE_ISO8859_1;
		else if (encString == "UTF-16" ||
			 encString == "UTF-16LE" ||
			 encString == "UCS-2" ||
			 encString == "UCS-2LE")	encoding = ID3TE_UTF16;
		else if (encString == "UTF-16BE" ||
			 encString == "UCS-2BE")	encoding = ID3TE_UTF16BE;

		char		*prevOutFormat = String::SetOutputFormat(encString);

		foreach (const Picture &picInfo, track.pictures)
		{
			ID3Frame	*frame_picture = ex_ID3Frame_NewID(ID3FID_PICTURE);

			ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_picture, ID3FN_TEXTENC), encoding);
			ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame_picture, ID3FN_DESCRIPTION), encoding);

			if	(encoding == ID3TE_UTF16)   ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_picture, ID3FN_DESCRIPTION), (unicode_t *) String(leBOM).Append(picInfo.description).ConvertTo("UTF-16LE"));
			else if (encoding == ID3TE_UTF16BE) ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_picture, ID3FN_DESCRIPTION), (unicode_t *) picInfo.description.ConvertTo("UTF-16BE"));
			else				    ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_picture, ID3FN_DESCRIPTION), picInfo.description);

			ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_picture, ID3FN_MIMETYPE), picInfo.mime.ConvertTo("ISO-8859-1"));
			ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_picture, ID3FN_PICTURETYPE), picInfo.type);
			ex_ID3Field_SetBINARY(ex_ID3Frame_GetField(frame_picture, ID3FN_DATA), picInfo.data, picInfo.data.Size());

			ex_ID3Tag_AddFrame(tag, frame_picture);

			frames.Add(frame_picture);
		}

		String::SetOutputFormat(prevOutFormat);
	}

	buffer.Resize(ex_ID3Tag_Size(tag));

	Int	 size = ex_ID3Tag_Render(tag, buffer, version == 1 ? ID3TT_ID3V1 : ID3TT_ID3V2);

	ex_ID3Tag_Delete(tag);

	for (Int i = 0; i < frames.Length(); i++)
	{
		ex_ID3Frame_Delete(frames.GetNth(i));
	}

	return size;
}

Int BoCA::TagID3::Parse(const Buffer<UnsignedByte> &buffer, Track *track)
{
	ID3Tag		*tag = ex_ID3Tag_New();

	ex_ID3Tag_ParseBuffer(tag, buffer, buffer.Size());

	Int	 retVal = ParseID3Tag(tag, track);

	ex_ID3Tag_Delete(tag);

	return retVal;
}

Int BoCA::TagID3::Parse(const String &fileName, Track *track)
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

	in.Seek(in.Size() - 128);

	/* Look for ID3v1 tag.
	 */
	if (in.InputString(3) == "TAG")
	{
		char	*prevInFormat = String::SetInputFormat("ISO-8859-1");

		track->title	= in.InputString(30);
		track->artist	= in.InputString(30);
		track->album	= in.InputString(30);
		track->year	= in.InputString(4).ToInt();
		track->comment	= in.InputString(28);

		if (in.InputNumber(1) == 0)
		{
			Int	 n = in.InputNumber(1);

			if (n > 0) track->track = n;
		}
		else
		{
			in.RelSeek(-29);

			track->comment = in.InputString(30);
		}

		track->genre	= GetID3CategoryName(in.InputNumber(1));

		String::SetInputFormat(prevInFormat);

		return Success();
	}

	return Error();
}

Int BoCA::TagID3::ParseID3Tag(Void *tag, Track *track)
{
	ID3TagIterator	*iterator = ex_ID3Tag_CreateIterator((ID3Tag *) tag);

	for (UnsignedInt i = 0; i < ex_ID3Tag_NumFrames((ID3Tag *) tag); i++)
	{
		ID3Frame	*frame = ex_ID3TagIterator_GetNext(iterator);

		if	(ex_ID3Frame_GetID(frame) == ID3FID_LEADARTIST)	track->artist	= GetID3v2FrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_TITLE)	track->title	= GetID3v2FrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_ALBUM)	track->album	= GetID3v2FrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_TRACKNUM)	track->track	= GetID3v2FrameString(frame).ToInt();
		else if (ex_ID3Frame_GetID(frame) == ID3FID_YEAR)	track->year	= GetID3v2FrameString(frame).ToInt();
		else if (ex_ID3Frame_GetID(frame) == ID3FID_COMMENT)	track->comment	= GetID3v2FrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_PUBLISHER)	track->label	= GetID3v2FrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_ISRC)	track->isrc	= GetID3v2FrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_CONTENTTYPE)
		{
			String	 s_genre = GetID3v2FrameString(frame);
			String	 genreID;

			if (s_genre[0] == '(')
			{
				for (Int j = 1; j < s_genre.Length(); j++)
				{
					if (s_genre[j] == ')') break;

					genreID[j - 1] = s_genre[j];
				}
			}

			if (genreID == NIL)				  track->genre = s_genre;
			else if (s_genre.Length() > genreID.Length() + 2) track->genre = s_genre.Tail(s_genre.Length() - genreID.Length() - 2);
			else if (genreID != NIL)			  track->genre = GetID3CategoryName(genreID.ToInt());
		}
		else if (ex_ID3Frame_GetID(frame) == ID3FID_PICTURE)
		{
			Picture	 picture;

			int	 tbufsize = 1024;
			char	*abuffer  = new char [tbufsize];
			wchar_t	*wbuffer  = new wchar_t [tbufsize];

			ZeroMemory(abuffer, tbufsize);
			ZeroMemory(wbuffer, tbufsize * 2);

			ID3Field	*field = ex_ID3Frame_GetField(frame, ID3FN_MIMETYPE);

			if (field != NIL)
			{
				ex_ID3Field_GetASCII(field, abuffer, tbufsize);

				picture.mime.ImportFrom("ISO-8859-1", abuffer);

				if	(picture.mime.ToLower() == "jpeg" || picture.mime.ToLower() == "jpg") picture.mime = "image/jpeg";
				else if (picture.mime.ToLower() == "png")				      picture.mime = "image/png";
			}

			field = ex_ID3Frame_GetField(frame, ID3FN_PICTURETYPE);

			if (field != NIL)
			{
				picture.type = ex_ID3Field_GetINT(field);
			}

			ZeroMemory(abuffer, tbufsize);
			ZeroMemory(wbuffer, tbufsize * 2);

			field = ex_ID3Frame_GetField(frame, ID3FN_TEXTENC);

			if (field != NIL)
			{
				Int	 encoding = ex_ID3Field_GetINT(field);

				if (encoding == ID3TE_ISO8859_1 || encoding == ID3TE_UTF8)
				{
					if ((field = ex_ID3Frame_GetField(frame, ID3FN_DESCRIPTION)) != NIL)
					{
						ex_ID3Field_GetASCII(field, abuffer, tbufsize);

						if (encoding == ID3TE_ISO8859_1)	picture.description.ImportFrom("ISO-8859-1", abuffer);
						else if (encoding == ID3TE_UTF8)	picture.description.ImportFrom("UTF-8", abuffer);
					}
				}
				else if (encoding == ID3TE_UTF16 || encoding == ID3TE_UTF16BE)
				{
					if ((field = ex_ID3Frame_GetField(frame, ID3FN_DESCRIPTION)) != NIL)
					{
						ex_ID3Field_GetUNICODE(field, (unicode_t *) wbuffer, tbufsize);

						picture.description.ImportFrom("UTF-16BE", (char *) wbuffer);
					}
				}
			}

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

			delete [] abuffer;
			delete [] wbuffer;
		}
	}

	ex_ID3TagIterator_Delete(iterator);

	return Success();
}

String BoCA::TagID3::GetID3v2FrameString(ID3Frame *frame)
{
	ID3Field	*field = ex_ID3Frame_GetField(frame, ID3FN_TEXTENC);
	String		 result;

	if (field != NIL)
	{
		Int	 encoding = ex_ID3Field_GetINT(field);

		int	 tbufsize = 1024;
		char	*abuffer  = new char [tbufsize];
		wchar_t	*wbuffer  = new wchar_t [tbufsize];

		ZeroMemory(abuffer, tbufsize);
		ZeroMemory(wbuffer, tbufsize * 2);

		if (encoding == ID3TE_ISO8859_1 || encoding == ID3TE_UTF8)
		{
			if ((field = ex_ID3Frame_GetField(frame, ID3FN_TEXT)) != NIL)
			{
				ex_ID3Field_GetASCII(field, abuffer, tbufsize);

				if	(encoding == ID3TE_ISO8859_1)	result.ImportFrom("ISO-8859-1", abuffer);
				else if (encoding == ID3TE_UTF8)	result.ImportFrom("UTF-8", abuffer);
			}
		}
		else if (encoding == ID3TE_UTF16 || encoding == ID3TE_UTF16BE)
		{
			if ((field = ex_ID3Frame_GetField(frame, ID3FN_TEXT)) != NIL)
			{
				ex_ID3Field_GetUNICODE(field, (unicode_t *) wbuffer, tbufsize);

				result.ImportFrom("UTF-16BE", (char *) wbuffer);
			}
		}

		delete [] abuffer;
		delete [] wbuffer;
	}

	return result;
}

Int BoCA::TagID3::SetID3v2FrameString(ID3Frame *frame, const String &text)
{
	String		 leBOM;

	leBOM[0] = 0xFEFF;

	ID3_TextEnc	 encoding = ID3TE_NONE;
	String		 encString = (version == 1 ? Config::Get()->id3v1_encoding : Config::Get()->id3v2_encoding);

	if	(encString == "UTF-8")		encoding = ID3TE_UTF8;
	else if (encString == "ISO-8859-1")	encoding = ID3TE_ISO8859_1;
	else if (encString == "UTF-16" ||
		 encString == "UTF-16LE" ||
		 encString == "UCS-2" ||
		 encString == "UCS-2LE")	encoding = ID3TE_UTF16;
	else if (encString == "UTF-16BE" ||
		 encString == "UCS-2BE")	encoding = ID3TE_UTF16BE;

	char		*prevOutFormat = String::SetOutputFormat(encString);

	ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame, ID3FN_TEXTENC), encoding);
	ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame, ID3FN_TEXT), encoding);

	if	(encoding == ID3TE_UTF16)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame, ID3FN_TEXT), (unicode_t *) String(leBOM).Append(text).ConvertTo("UTF-16LE"));
	else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame, ID3FN_TEXT), (unicode_t *) text.ConvertTo("UTF-16BE"));
	else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame, ID3FN_TEXT), text);

	String::SetOutputFormat(prevOutFormat);

	return Success();
}

const String &BoCA::TagID3::GetID3CategoryName(Int id)
{
	static const String	 empty = "";
	static const String	 array[148] =
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

	if (id < 0 || id > 147) return empty;
	else			return array[id];
}
