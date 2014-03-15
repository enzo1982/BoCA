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
#include <smooth/dll.h>

#include "mp4.h"

const String &BoCA::TaggerMP4::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (mp4v2dll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>MP4 Tagger</name>			\
		    <version>1.0</version>			\
		    <id>mp4-tag</id>				\
		    <type>tagger</type>				\
		    <format>					\
		      <name>MPEG-4 Audio Files</name>		\
		      <extension>m4a</extension>		\
		      <extension>m4b</extension>		\
		      <extension>m4r</extension>		\
		      <extension>mp4</extension>		\
		      <extension>3gp</extension>		\
		    </format>					\
		    <tagformat>					\
		      <name>MP4 Metadata</name>			\
		      <coverart supported=\"true\"/>		\
		      <encodings>				\
			<encoding>UTF-8</encoding>		\
		      </encodings>				\
		    </tagformat>				\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeMP4v2DLL();
}

const String	 BoCA::TaggerMP4::genres[148] =
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

BoCA::TaggerMP4::TaggerMP4()
{
}

BoCA::TaggerMP4::~TaggerMP4()
{
}

Error BoCA::TaggerMP4::RenderStreamInfo(const String &fileName, const Track &track)
{
	Config		*currentConfig = Config::Get();

	MP4FileHandle	 mp4File;
	const Info	&info = track.GetInfo();

	if (String::IsUnicode(fileName))
	{
		File(fileName).Copy(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"));

		mp4File = ex_MP4Modify(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"), 0);
	}
	else
	{
		mp4File = ex_MP4Modify(fileName, 0);
	}

	const MP4Tags	*mp4Tags = ex_MP4TagsAlloc();

	ex_MP4TagsFetch(mp4Tags, mp4File);

	String	 prevOutFormat = String::SetOutputFormat("UTF-8");

	if (info.artist != NIL) ex_MP4TagsSetArtist(mp4Tags, info.artist);
	if (info.title  != NIL) ex_MP4TagsSetName(mp4Tags, info.title);
	if (info.album  != NIL) ex_MP4TagsSetAlbum(mp4Tags, info.album);
	if (info.year    >   0) ex_MP4TagsSetReleaseDate(mp4Tags, String::FromInt(info.year));
	if (info.genre  != NIL) ex_MP4TagsSetGenre(mp4Tags, info.genre);

	if (info.track > 0)
	{
		MP4TagTrack	 mp4Track = { (uint16_t) info.track, (uint16_t) info.numTracks };

		ex_MP4TagsSetTrack(mp4Tags, &mp4Track);
	}

	if (info.disc > 0 && (info.numDiscs > 1 || info.disc > 1))
	{
		MP4TagDisk	 mp4Disk = { (uint16_t) info.disc, (uint16_t) info.numDiscs };

		ex_MP4TagsSetDisk(mp4Tags, &mp4Disk);
	}

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	ex_MP4TagsSetComments(mp4Tags, info.comment);
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				ex_MP4TagsSetComments(mp4Tags, currentConfig->GetStringValue("Tags", "DefaultComment", NIL));

	/* Save other text info.
	 */
	for (Int i = 0; i < info.other.Length(); i++)
	{
		const String	&pair  = info.other.GetNth(i);

		String		 key   = pair.Head(pair.Find(":") + 1);
		String		 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if (key == String(INFO_COMPOSER).Append(":")) ex_MP4TagsSetComposer(mp4Tags, value);
	}

	/* Save cover art.
	 */
	if (currentConfig->GetIntValue("Tags", "CoverArtWriteToTags", True) && currentConfig->GetIntValue("Tags", "CoverArtWriteToMP4Metadata", True))
	{
		foreach (const Picture &picInfo, track.pictures)
		{
			MP4TagArtwork	 artwork = { const_cast<UnsignedByte *>((const UnsignedByte *) picInfo.data), (uint32_t) picInfo.data.Size(), picInfo.mime == "image/png" ? MP4_ART_PNG : MP4_ART_JPEG };

			ex_MP4TagsAddArtwork(mp4Tags, &artwork);
		}
	}

	/* Set media type.
	 */
	uint8_t	 mediaType = 1;					 //  1 == Music

	if	(track.outfile.EndsWith(".m4b")) mediaType = 2;	 //  2 == Audiobook
	else if (track.outfile.EndsWith(".m4r")) mediaType = 14; // 14 == Ringtone

	ex_MP4TagsSetMediaType(mp4Tags, &mediaType);

	ex_MP4TagsStore(mp4Tags, mp4File);
	ex_MP4TagsFree(mp4Tags);

	/* Save chapters.
	 */
	if (track.tracks.Length() > 0 && currentConfig->GetIntValue("Tags", "WriteChapters", True))
	{
		MP4Chapter_t	*chapterList = new MP4Chapter_t [track.tracks.Length()];
		uint32_t	 chapterCount = track.tracks.Length();

		for (UnsignedInt i = 0; i < chapterCount; i++)
		{
			const Track	&chapterTrack  = track.tracks.GetNth(i);
			const Info	&chapterInfo   = chapterTrack.GetInfo();
			const Format	&chapterFormat = chapterTrack.GetFormat();

			const char	*chapterTitle  = chapterInfo.title;

			memset(chapterList[i].title, 0, MP4V2_CHAPTER_TITLE_MAX + 1);

			if (chapterTitle != NIL) strncpy(chapterList[i].title, chapterTitle, Math::Min(strlen(chapterTitle), MP4V2_CHAPTER_TITLE_MAX));

			if	(chapterTrack.length	   >= 0) chapterList[i].duration = Math::Round(Float(chapterTrack.length)	* MP4_MSECS_TIME_SCALE / chapterFormat.rate);
			else if (chapterTrack.approxLength >= 0) chapterList[i].duration = Math::Round(Float(chapterTrack.approxLength) * MP4_MSECS_TIME_SCALE / chapterFormat.rate);
			else					 chapterList[i].duration = MP4_INVALID_DURATION;
		}

		ex_MP4SetChapters(mp4File, chapterList, chapterCount, (MP4ChapterType) currentConfig->GetIntValue("Tags", "WriteChaptersType", MP4ChapterTypeAny));

		delete [] chapterList;
	}

	String::SetOutputFormat(prevOutFormat);

	ex_MP4Close(mp4File, 0);

	if (String::IsUnicode(fileName))
	{
		ex_MP4Optimize(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"), NIL);

		File(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag")).Move(fileName);
	}
	else
	{
		ex_MP4Optimize(fileName, NIL);
	}

	return Success();
}

Error BoCA::TaggerMP4::ParseStreamInfo(const String &fileName, Track &track)
{
	Config		*currentConfig = Config::Get();

	MP4FileHandle	 mp4File;
	Info		 info = track.GetInfo();

	if (String::IsUnicode(fileName))
	{
		File(fileName).Copy(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"));

		mp4File = ex_MP4Read(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"));
	}
	else
	{
		mp4File = ex_MP4Read(fileName);
	}

	const MP4Tags	*mp4Tags = ex_MP4TagsAlloc();

	ex_MP4TagsFetch(mp4Tags, mp4File);

	String	 prevInFormat = String::SetInputFormat("UTF-8");

	if	(mp4Tags->name	      != NIL) info.title    = mp4Tags->name;
	if	(mp4Tags->artist      != NIL) info.artist   = mp4Tags->artist;
	if	(mp4Tags->releaseDate != NIL) info.year     = String(mp4Tags->releaseDate).ToInt();
	if	(mp4Tags->album	      != NIL) info.album    = mp4Tags->album;
	if	(mp4Tags->comments    != NIL) info.comment  = mp4Tags->comments;

	if	(mp4Tags->composer    != NIL) info.other.Add(String(INFO_COMPOSER).Append(":").Append(mp4Tags->composer));

	if	(mp4Tags->genre	      != NIL) info.genre    = mp4Tags->genre;
	else if (mp4Tags->genreType   != NIL) info.genre    = GetID3CategoryName(*mp4Tags->genreType - 1);

	if (mp4Tags->track != NIL)
	{
		info.track	= mp4Tags->track->index;
		info.numTracks	= mp4Tags->track->total;
	}

	if (mp4Tags->disk != NIL)
	{
		info.disc	= mp4Tags->disk->index;
		info.numDiscs	= mp4Tags->disk->total;
	}

	if (currentConfig->GetIntValue("Tags", "CoverArtReadFromTags", True))
	{
		for (UnsignedInt i = 0; i < mp4Tags->artworkCount; i++)
		{
			unsigned char	*buffer	= (unsigned char *) mp4Tags->artwork[i].data;
			uint32_t	 size	= mp4Tags->artwork[i].size;

			if (size > 0)
			{
				Picture	 picture;

				picture.data.Set(buffer, size);

				if	(buffer[0] == 0xFF && buffer[1] == 0xD8) picture.mime = "image/jpeg";
				else if (buffer[0] == 0x89 && buffer[1] == 0x50 &&
					 buffer[2] == 0x4E && buffer[3] == 0x47 &&
					 buffer[4] == 0x0D && buffer[5] == 0x0A &&
					 buffer[6] == 0x1A && buffer[7] == 0x0A) picture.mime = "image/png";

				picture.type = 0;

				track.pictures.Add(picture);
			}
		}
	}

	track.SetInfo(info);

	ex_MP4TagsFree(mp4Tags);

	/* Read chapters.
	 */
	MP4Chapter_t	*chapterList  = NIL;
	uint32_t	 chapterCount = 0;

	ex_MP4GetChapters(mp4File, &chapterList, &chapterCount, MP4ChapterTypeAny);

	if (chapterList != NIL && chapterCount > 1 && currentConfig->GetIntValue("Tags", "ReadChapters", True))
	{
		MP4Duration	 offset = 0;

		for (UnsignedInt i = 0; i < chapterCount; i++)
		{
			const Format	&format = track.GetFormat();

			/* Fill track data.
			 */
			Track	 rTrack;

			rTrack.origFilename = track.origFilename;
			rTrack.pictures	    = track.pictures;

			rTrack.sampleOffset = Math::Round(Float(offset)			 / MP4_MSECS_TIME_SCALE * format.rate);
			rTrack.length	    = Math::Round(Float(chapterList[i].duration) / MP4_MSECS_TIME_SCALE * format.rate);

			rTrack.fileSize	    = rTrack.length * format.channels * (format.bits / 8);

			rTrack.SetFormat(format);

			/* Set track title.
			 */
			Info	 info = track.GetInfo();

			info.title = chapterList[i].title;
			info.track = i + 1;

			rTrack.SetInfo(info);

			/* Add track to track list.
			 */
			track.tracks.Add(rTrack);

			offset += chapterList[i].duration;
		}

		ex_MP4Free(chapterList);
	}

	String::SetInputFormat(prevInFormat);

	ex_MP4Close(mp4File, 0);

	if (String::IsUnicode(fileName))
	{
		File(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag")).Delete();
	}

	return Success();
}

Error BoCA::TaggerMP4::UpdateStreamInfo(const String &fileName, const Track &track)
{
	MP4FileHandle	 mp4File;

	if (String::IsUnicode(fileName))
	{
		File(fileName).Copy(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"));

		mp4File = ex_MP4Modify(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"), 0);
	}
	else
	{
		mp4File = ex_MP4Modify(fileName, 0);
	}

	const MP4Tags	*mp4Tags = ex_MP4TagsAlloc();

	ex_MP4TagsFetch(mp4Tags, mp4File);

	/* Remove metadata first.
	 */
	ex_MP4TagsSetArtist(mp4Tags, NIL);
	ex_MP4TagsSetName(mp4Tags, NIL);
	ex_MP4TagsSetAlbum(mp4Tags, NIL);
	ex_MP4TagsSetTrack(mp4Tags, NIL);
	ex_MP4TagsSetDisk(mp4Tags, NIL);
	ex_MP4TagsSetReleaseDate(mp4Tags, NIL);
	ex_MP4TagsSetGenre(mp4Tags, NIL);
	ex_MP4TagsSetGenreType(mp4Tags, NIL);
	ex_MP4TagsSetComments(mp4Tags, NIL);

	for (Int i = mp4Tags->artworkCount - 1; i >= 0; i--)
	{
		ex_MP4TagsRemoveArtwork(mp4Tags, i);
	}

	ex_MP4TagsStore(mp4Tags, mp4File);

	ex_MP4TagsFree(mp4Tags);

	ex_MP4Close(mp4File, 0);

	if (String::IsUnicode(fileName))
	{
		RenderStreamInfo(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag"), track);

		File(fileName).Delete();
		File(Utilities::GetNonUnicodeTempFileName(fileName).Append(".tag")).Move(fileName);
	}
	else
	{
		RenderStreamInfo(fileName, track);
	}

	return Success();
}

const String &BoCA::TaggerMP4::GetID3CategoryName(UnsignedInt id)
{
	static const String	 empty;

	if (id > 147) return empty;
	else	      return genres[id];
}
