 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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
		    <tagspec>					\
		      <name>MP4 Metadata</name>			\
		      <coverart supported=\"true\"/>		\
		      <encodings>				\
			<encoding>UTF-8</encoding>		\
		      </encodings>				\
		    </tagspec>					\
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

const String	 BoCA::TaggerMP4::genres[192] =
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

BoCA::TaggerMP4::TaggerMP4()
{
}

BoCA::TaggerMP4::~TaggerMP4()
{
}

Error BoCA::TaggerMP4::RenderStreamInfo(const String &fileName, const Track &track)
{
	const Config	*currentConfig = GetConfiguration();

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

	if (info.artist != NIL) ex_MP4TagsSetArtist(mp4Tags, info.artist.Trim());
	if (info.title  != NIL) ex_MP4TagsSetName(mp4Tags, info.title.Trim());
	if (info.album  != NIL) ex_MP4TagsSetAlbum(mp4Tags, info.album.Trim());
	if (info.year    >   0) ex_MP4TagsSetReleaseDate(mp4Tags, String::FromInt(info.year));
	if (info.genre  != NIL) ex_MP4TagsSetGenre(mp4Tags, info.genre.Trim());

	if (info.track > 0)
	{
		MP4TagTrack	 mp4Track = { (uint16_t) info.track, (uint16_t) info.numTracks };

		ex_MP4TagsSetTrack(mp4Tags, &mp4Track);
	}

	if (info.disc > 0)
	{
		MP4TagDisk	 mp4Disk = { (uint16_t) info.disc, (uint16_t) (info.numDiscs > 0 ? info.numDiscs : info.disc) };

		ex_MP4TagsSetDisk(mp4Tags, &mp4Disk);
	}

	if	(info.comment != NIL && !currentConfig->GetIntValue("Tags", "ReplaceExistingComments", False))	ex_MP4TagsSetComments(mp4Tags, info.comment.Trim());
	else if (currentConfig->GetStringValue("Tags", "DefaultComment", NIL) != NIL)				ex_MP4TagsSetComments(mp4Tags, currentConfig->GetStringValue("Tags", "DefaultComment", NIL).Trim());

	/* Save other text info.
	 */
	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":"));
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == INFO_ALBUMARTIST) ex_MP4TagsSetAlbumArtist(mp4Tags, value.Trim());

		else if	(key == INFO_COMPOSER)    ex_MP4TagsSetComposer(mp4Tags,    value.Trim());

		else if (key == INFO_BPM)
		{
			uint16_t	 tempo = value.ToInt();

			ex_MP4TagsSetTempo(mp4Tags, &tempo);
		}
	}

	/* Save cover art.
	 */
	if (currentConfig->GetIntValue("Tags", "CoverArtWriteToTags", True) && currentConfig->GetIntValue("Tags", "CoverArtWriteToMP4Metadata", True))
	{
		/* Put front and back covers first.
		 */
		Array<Picture>	 pictures;

		foreach (const Picture &picInfo, track.pictures)
		{
			if	(picInfo.type == 3) pictures.InsertAtPos(0, picInfo);
			else if	(picInfo.type == 4) pictures.InsertAtPos((pictures.Length() > 0 && pictures.GetFirst().type == 3) ? 1 : 0, picInfo);
			else			    pictures.Add(picInfo);
		}

		/* Add cover art to tag.
		 */
		foreach (const Picture &picInfo, pictures)
		{
			MP4TagArtwork	 artwork = { const_cast<UnsignedByte *>((const UnsignedByte *) picInfo.data), (uint32_t) picInfo.data.Size(), picInfo.mime == "image/png" ? MP4_ART_PNG : MP4_ART_JPEG };

			ex_MP4TagsAddArtwork(mp4Tags, &artwork);
		}
	}

	/* Set media type.
	 */
	uint8_t	 mediaType = 1;					 //  1 == Music

	if	(track.outfile.EndsWith(".m4b")) mediaType =  2; //  2 == Audiobook
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

			const char	*chapterTitle  = chapterInfo.title.Trim();

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
	const Config	*currentConfig = GetConfiguration();

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

	if	(mp4Tags->name	      != NIL) info.title    = String(mp4Tags->name).Trim();
	if	(mp4Tags->artist      != NIL) info.artist   = String(mp4Tags->artist).Trim();
	if	(mp4Tags->releaseDate != NIL) info.year     = String(mp4Tags->releaseDate).Trim().ToInt();
	if	(mp4Tags->album	      != NIL) info.album    = String(mp4Tags->album).Trim();
	if	(mp4Tags->comments    != NIL) info.comment  = String(mp4Tags->comments).Trim();

	if	(mp4Tags->albumArtist != NIL) info.SetOtherInfo(INFO_ALBUMARTIST, String(mp4Tags->albumArtist).Trim());

	if	(mp4Tags->composer    != NIL) info.SetOtherInfo(INFO_COMPOSER,	  String(mp4Tags->composer).Trim());

	if	(mp4Tags->tempo	      != NIL) info.SetOtherInfo(INFO_BPM,	  String::FromInt(*mp4Tags->tempo));

	if	(mp4Tags->genre	      != NIL) info.genre    = String(mp4Tags->genre).Trim();
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

				if	(i == 0) picture.type = 3; // Cover (front)
				else if (i == 1) picture.type = 4; // Cover (back)
				else		 picture.type = 0; // Other

				picture.data.Set(buffer, size);

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

			info.title = String(chapterList[i].title).Trim();
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
	ex_MP4TagsSetAlbumArtist(mp4Tags, NIL);
	ex_MP4TagsSetComposer(mp4Tags, NIL);
	ex_MP4TagsSetTempo(mp4Tags, NIL);
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

	if (id > 191) return empty;
	else	      return genres[id];
}
