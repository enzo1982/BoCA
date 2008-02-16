 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/track.h>
#include <boca/common/config.h>
#include <boca/core/dllinterfaces.h>

BoCA::Track::Track()
{
	channels	= 0;
	rate		= 0;
	bits		= 0;
	length		= -1;
	approxLength	= -1;
	fileSize	= -1;
	order		= BYTE_INTEL;

	isCDTrack	= False;

	drive		= -1;
	discid		= 0;
	cdTrack		= -1;

	track		= -1;
	year		= -1;
}

BoCA::Track::Track(const Track &oTrack)
{
	*this = oTrack;
}

BoCA::Track::~Track()
{
}

BoCA::Track &BoCA::Track::operator =(const Track &oTrack)
{
	channels	= oTrack.channels;
	rate		= oTrack.rate;
	bits		= oTrack.bits;
	length		= oTrack.length;
	approxLength	= oTrack.approxLength;
	fileSize	= oTrack.fileSize;
	order		= oTrack.order;

	isCDTrack	= oTrack.isCDTrack;
	drive		= oTrack.drive;
	cdTrack		= oTrack.cdTrack;

	artist		= oTrack.artist;
	title		= oTrack.title;
	album		= oTrack.album;
	track		= oTrack.track;
	genre		= oTrack.genre;
	year		= oTrack.year;
	comment		= oTrack.comment;

	oArtist		= oTrack.oArtist;
	oTitle		= oTrack.oTitle;
	oAlbum		= oTrack.oAlbum;
	oGenre		= oTrack.oGenre;

	for (Int i = 0; i < oTrack.pictures.Length(); i++)
	{
		pictures.Add(oTrack.pictures.GetNth(i));
	}

	offset		= oTrack.offset;
	discid		= oTrack.discid;
	category	= oTrack.category;
	revision	= oTrack.revision;
	disclength	= oTrack.disclength;
	discComment	= oTrack.discComment;
	playorder	= oTrack.playorder;

	fileSizeString	= oTrack.fileSizeString;
	lengthString	= oTrack.lengthString;

	outfile		= oTrack.outfile;
	origFilename	= oTrack.origFilename;

	return *this;
}

Int BoCA::Track::RenderID3Tag(Int version, Buffer<unsigned char> &buffer)
{
	Config		*currentConfig = Config::Get();

	ID3Tag		*tag = ex_ID3Tag_New();

	ex_ID3Tag_SetPadding(tag, false);

	ID3_TextEnc	 encoding = ID3TE_NONE;
	String		 encString = (version == 1 ? currentConfig->id3v1_encoding : currentConfig->id3v2_encoding);

	if (encString == "UTF-8")		encoding = ID3TE_UTF8;
	else if (encString == "ISO-8859-1")	encoding = ID3TE_ISO8859_1;
	else if (encString == "UTF-16" ||
		 encString == "UTF-16LE" ||
		 encString == "UCS-2" ||
		 encString == "UCS-2LE")	encoding = ID3TE_UTF16;
	else if (encString == "UTF-16BE" ||
		 encString == "UCS-2BE")	encoding = ID3TE_UTF16BE;

	char	*prevOutFormat = String::SetOutputFormat(encString);

	String		 leBOM;

	leBOM[0] = 0xFEFF;

	ID3Frame	*frame_artist = ex_ID3Frame_NewID(ID3FID_LEADARTIST);

	if (artist != NIL)
	{
		ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_artist, ID3FN_TEXTENC), encoding);
		ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame_artist, ID3FN_TEXT), encoding);

		if (encoding == ID3TE_UTF16)		ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_artist, ID3FN_TEXT), (unicode_t *) String(leBOM).Append(artist).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_artist, ID3FN_TEXT), (unicode_t *) artist.ConvertTo("UTF-16BE"));
		else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_artist, ID3FN_TEXT), artist);

		ex_ID3Tag_AddFrame(tag, frame_artist);
	}

	ID3Frame	*frame_title = ex_ID3Frame_NewID(ID3FID_TITLE);

	if (title != NIL)
	{
		ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_title, ID3FN_TEXTENC), encoding);
		ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame_title, ID3FN_TEXT), encoding);

		if (encoding == ID3TE_UTF16)		ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_title, ID3FN_TEXT), (unicode_t *) String(leBOM).Append(title).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_title, ID3FN_TEXT), (unicode_t *) title.ConvertTo("UTF-16BE"));
		else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_title, ID3FN_TEXT), title);

		ex_ID3Tag_AddFrame(tag, frame_title);
	}

	ID3Frame	*frame_album = ex_ID3Frame_NewID(ID3FID_ALBUM);

	if (album != NIL)
	{
		ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_album, ID3FN_TEXTENC), encoding);
		ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame_album, ID3FN_TEXT), encoding);

		if (encoding == ID3TE_UTF16)		ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_album, ID3FN_TEXT), (unicode_t *) String(leBOM).Append(album).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_album, ID3FN_TEXT), (unicode_t *) album.ConvertTo("UTF-16BE"));
		else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_album, ID3FN_TEXT), album);

		ex_ID3Tag_AddFrame(tag, frame_album);
	}

	ID3Frame	*frame_track = ex_ID3Frame_NewID(ID3FID_TRACKNUM);

	if (track > 0)
	{
		ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_track, ID3FN_TEXTENC), encoding);
		ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame_track, ID3FN_TEXT), encoding);

		if (encoding == ID3TE_UTF16)		ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_track, ID3FN_TEXT), (unicode_t *) String(leBOM).Append(String(track < 10 ? "0" : "").Append(String::FromInt(track))).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_track, ID3FN_TEXT), (unicode_t *) String(track < 10 ? "0" : "").Append(String::FromInt(track)).ConvertTo("UTF-16BE"));
		else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_track, ID3FN_TEXT), String(track < 10 ? "0" : "").Append(String::FromInt(track)));

		ex_ID3Tag_AddFrame(tag, frame_track);
	}

	ID3Frame	*frame_year = ex_ID3Frame_NewID(ID3FID_YEAR);

	if (year > 0)
	{
		ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_year, ID3FN_TEXTENC), encoding);
		ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame_year, ID3FN_TEXT), encoding);

		if (encoding == ID3TE_UTF16)		ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_year, ID3FN_TEXT), (unicode_t *) String(leBOM).Append(String::FromInt(year)).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_year, ID3FN_TEXT), (unicode_t *) String::FromInt(year).ConvertTo("UTF-16BE"));
		else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_year, ID3FN_TEXT), String::FromInt(year));

		ex_ID3Tag_AddFrame(tag, frame_year);
	}

	ID3Frame	*frame_genre = ex_ID3Frame_NewID(ID3FID_CONTENTTYPE);

	if (genre != NIL)
	{
		ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_genre, ID3FN_TEXTENC), encoding);
		ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame_genre, ID3FN_TEXT), encoding);

		if (encoding == ID3TE_UTF16)		ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_genre, ID3FN_TEXT), (unicode_t *) String(leBOM).Append(genre).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_genre, ID3FN_TEXT), (unicode_t *) genre.ConvertTo("UTF-16BE"));
		else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_genre, ID3FN_TEXT), genre);

		ex_ID3Tag_AddFrame(tag, frame_genre);
	}

	ID3Frame	*frame_comment = ex_ID3Frame_NewID(ID3FID_COMMENT);

	if (currentConfig->default_comment != NIL) 
	{
		ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_comment, ID3FN_TEXTENC), encoding);
		ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame_comment, ID3FN_TEXT), encoding);

		if (encoding == ID3TE_UTF16)		ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_comment, ID3FN_TEXT), (unicode_t *) String(leBOM).Append(currentConfig->default_comment).ConvertTo("UTF-16LE"));
		else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_comment, ID3FN_TEXT), (unicode_t *) currentConfig->default_comment.ConvertTo("UTF-16BE"));
		else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_comment, ID3FN_TEXT), currentConfig->default_comment);

		ex_ID3Tag_AddFrame(tag, frame_comment);
	}

	Array<ID3Frame *>	 frame_pictures;

	if (currentConfig->copy_picture_tags)
	{
		for (Int i = 0; i < pictures.Length(); i++)
		{
			ID3Frame	*frame_picture = ex_ID3Frame_NewID(ID3FID_PICTURE);
			const Picture	&picInfo = pictures.GetNth(i);

			ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_picture, ID3FN_TEXTENC), encoding);
			ex_ID3Field_SetEncoding(ex_ID3Frame_GetField(frame_picture, ID3FN_DESCRIPTION), encoding);

			if (encoding == ID3TE_UTF16)		ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_picture, ID3FN_DESCRIPTION), (unicode_t *) String(leBOM).Append(picInfo.description).ConvertTo("UTF-16LE"));
			else if (encoding == ID3TE_UTF16BE)	ex_ID3Field_SetUNICODE(ex_ID3Frame_GetField(frame_picture, ID3FN_DESCRIPTION), (unicode_t *) picInfo.description.ConvertTo("UTF-16BE"));
			else					ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_picture, ID3FN_DESCRIPTION), picInfo.description);

			ex_ID3Field_SetASCII(ex_ID3Frame_GetField(frame_picture, ID3FN_MIMETYPE), picInfo.mime.ConvertTo("ISO-8859-1"));
			ex_ID3Field_SetINT(ex_ID3Frame_GetField(frame_picture, ID3FN_PICTURETYPE), picInfo.type);
			ex_ID3Field_SetBINARY(ex_ID3Frame_GetField(frame_picture, ID3FN_DATA), picInfo.data, picInfo.data.Size());

			ex_ID3Tag_AddFrame(tag, frame_picture);

			frame_pictures.Add(frame_picture);
		}
	}

	String::SetOutputFormat(prevOutFormat);

	buffer.Resize(ex_ID3Tag_Size(tag));

	Int	 size = ex_ID3Tag_Render(tag, buffer, version == 1 ? ID3TT_ID3V1 : ID3TT_ID3V2);

	ex_ID3Tag_Delete(tag);
	ex_ID3Frame_Delete(frame_artist);
	ex_ID3Frame_Delete(frame_title);
	ex_ID3Frame_Delete(frame_album);
	ex_ID3Frame_Delete(frame_track);
	ex_ID3Frame_Delete(frame_year);
	ex_ID3Frame_Delete(frame_genre);
	ex_ID3Frame_Delete(frame_comment);

	for (Int j = 0; j < frame_pictures.Length(); j++)
	{
		ex_ID3Frame_Delete(frame_pictures.GetNth(j));
	}

	return size;
}

Bool BoCA::Track::ParseID3V2Tag(Buffer<unsigned char> &buffer)
{
	ID3Tag		*tag = ex_ID3Tag_New();

	ex_ID3Tag_ParseBuffer(tag, buffer, buffer.Size());

	Bool	 retVal = ParseID3V2Tag(tag);

	ex_ID3Tag_Delete(tag);

	return retVal;
}

Bool BoCA::Track::ParseID3V2Tag(const String &fileName)
{
	ID3Tag	*tag = ex_ID3Tag_New();

	ex_ID3Tag_Link(tag, CreateTempFile(fileName));

	RemoveTempFile(fileName);

	Bool	 retVal = ParseID3V2Tag(tag);

	ex_ID3Tag_Delete(tag);

	return retVal;
}

Bool BoCA::Track::ParseID3V2Tag(Void *tag)
{
	ID3TagIterator	*iterator = ex_ID3Tag_CreateIterator((ID3Tag *) tag);

	for (UnsignedInt i = 0; i < ex_ID3Tag_NumFrames((ID3Tag *) tag); i++)
	{
		ID3Frame	*frame = ex_ID3TagIterator_GetNext(iterator);

		if	(ex_ID3Frame_GetID(frame) == ID3FID_LEADARTIST)	artist = GetID3V2FrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_TITLE)	title = GetID3V2FrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_ALBUM)	album = GetID3V2FrameString(frame);
		else if (ex_ID3Frame_GetID(frame) == ID3FID_TRACKNUM)	track = GetID3V2FrameString(frame).ToInt();
		else if (ex_ID3Frame_GetID(frame) == ID3FID_YEAR)	year = GetID3V2FrameString(frame).ToInt();
		else if (ex_ID3Frame_GetID(frame) == ID3FID_CONTENTTYPE)
		{
			String	 s_genre = GetID3V2FrameString(frame);
			String	 genreID;

			if (s_genre[0] == '(')
			{
				for (Int j = 1; j < s_genre.Length(); j++)
				{
					if (s_genre[j] == ')') break;

					genreID[j - 1] = s_genre[j];
				}
			}

			if (genreID == NIL)				  genre = s_genre;
			else if (s_genre.Length() > genreID.Length() + 2) genre = s_genre.Tail(s_genre.Length() - genreID.Length() - 2);
			else if (genreID != NIL)			  genre = GetID3CategoryName(genreID.ToInt());
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

				ex_ID3Field_GetBINARY(field, picture.data, picture.data.Size());
			}

			delete [] abuffer;
			delete [] wbuffer;

			pictures.Add(picture);
		}
	}

	ex_ID3TagIterator_Delete(iterator);

	return True;
}

String BoCA::Track::GetID3V2FrameString(Void *frame)
{
	ID3Field	*field = ex_ID3Frame_GetField((ID3Frame *) frame, ID3FN_TEXTENC);
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
			if ((field = ex_ID3Frame_GetField((ID3Frame *) frame, ID3FN_TEXT)) != NIL)
			{
				ex_ID3Field_GetASCII(field, abuffer, tbufsize);

				if (encoding == ID3TE_ISO8859_1)	result.ImportFrom("ISO-8859-1", abuffer);
				else if (encoding == ID3TE_UTF8)	result.ImportFrom("UTF-8", abuffer);
			}
		}
		else if (encoding == ID3TE_UTF16 || encoding == ID3TE_UTF16BE)
		{
			if ((field = ex_ID3Frame_GetField((ID3Frame *) frame, ID3FN_TEXT)) != NIL)
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

const String &BoCA::Track::GetID3CategoryName(Int id)
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

String BoCA::Track::GetTempFileName(const String &oFileName)
{
	String	 rVal	= oFileName;
	Int	 lastBs	= -1;

	for (Int i = 0; i < rVal.Length(); i++)
	{
		if (rVal[i] > 255)	rVal[i] = '#';
		if (rVal[i] == '\\')	lastBs = i;
	}

	if (rVal == oFileName) return rVal;

	String	 tempDir = S::System::System::GetTempDirectory();

	for (Int j = lastBs + 1; j < rVal.Length(); j++)
	{
		tempDir[tempDir.Length()] = rVal[j];
	}

	return tempDir.Append(".out.temp");
}

String BoCA::Track::CreateTempFile(const String &oFileName)
{
	String		 tempFileName = GetTempFileName(oFileName);

	if (tempFileName == oFileName) return oFileName;

	IO::InStream	*in = new IO::InStream(IO::STREAM_FILE, oFileName, IO::IS_READONLY);
	IO::OutStream	*out = new IO::OutStream(IO::STREAM_FILE, tempFileName, IO::OS_OVERWRITE);

	Buffer<unsigned char>	 buffer;

	buffer.Resize(1024);

	Int	 bytesleft = in->Size();

	while (bytesleft > 0)
	{
		out->OutputData(in->InputData(buffer, Math::Min(1024, bytesleft)), Math::Min(1024, bytesleft));

		bytesleft -= 1024;
	}

	delete in;
	delete out;

	return tempFileName;
}

Bool BoCA::Track::RemoveTempFile(const String &oFileName)
{
	String		 tempFileName = GetTempFileName(oFileName);

	if (tempFileName == oFileName) return True;

	File(tempFileName).Delete();

	return True;
}
