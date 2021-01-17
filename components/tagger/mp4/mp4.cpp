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
#include <smooth/dll.h>

#include "mp4.h"
#include "config.h"

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

const String	 BoCA::TaggerMP4::ConfigID = "Tags";

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
	configLayer = NIL;
}

BoCA::TaggerMP4::~TaggerMP4()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Error BoCA::TaggerMP4::RenderStreamInfo(const String &fileName, const Track &track)
{
	/* Get configuration.
	 */
	const Config	*currentConfig		 = GetConfiguration();

	Bool		 writeChapters		 = currentConfig->GetIntValue(ConfigID, "WriteChapters", True);

	Bool		 preserveReplayGain	 = currentConfig->GetIntValue(ConfigID, "PreserveReplayGain", True);

	Bool		 coverArtWriteToTags	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToTags", True);
	Bool		 coverArtWriteToMP4	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToMP4Metadata", True);

	Bool		 replaceExistingComments = currentConfig->GetIntValue(ConfigID, "ReplaceExistingComments", False);
	String		 defaultComment		 = currentConfig->GetStringValue(ConfigID, "DefaultComment", NIL);

	/* Open MP4 file for modification.
	 */
	const Info	&info	 = track.GetInfo();

	MP4FileHandle	 mp4File = ex_MP4Modify(fileName.ConvertTo("UTF-8"), 0);

	if (mp4File == NIL) return Error();

	const MP4Tags	*mp4Tags = ex_MP4TagsAlloc();

	ex_MP4TagsFetch(mp4Tags, mp4File);

	String::OutputFormat	 outputFormat("UTF-8");

	if (info.artist != NIL) ex_MP4TagsSetArtist(mp4Tags, info.artist.Trim());
	if (info.title  != NIL) ex_MP4TagsSetName(mp4Tags, info.title.Trim());
	if (info.album  != NIL) ex_MP4TagsSetAlbum(mp4Tags, info.album.Trim());
	if (info.year    >   0) ex_MP4TagsSetReleaseDate(mp4Tags, String::FromInt(info.year));
	if (info.genre  != NIL) ex_MP4TagsSetGenre(mp4Tags, info.genre.Trim());

	if (info.track > 0)
	{
		MP4TagTrack	 mp4Track = { (uint16_t) info.track, (uint16_t) (info.numTracks > 0 ? info.numTracks : 0 ) };

		ex_MP4TagsSetTrack(mp4Tags, &mp4Track);
	}

	if (info.disc > 0)
	{
		MP4TagDisk	 mp4Disk = { (uint16_t) info.disc, (uint16_t) (info.numDiscs > 0 ? info.numDiscs : 0) };

		ex_MP4TagsSetDisk(mp4Tags, &mp4Disk);
	}

	if	(info.comment != NIL && !replaceExistingComments) ex_MP4TagsSetComments(mp4Tags, info.comment.Trim());
	else if (defaultComment != NIL)				  ex_MP4TagsSetComments(mp4Tags, String(defaultComment).Trim());

	/* Save other text info.
	 */
	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":"));
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == INFO_ALBUMARTIST)  ex_MP4TagsSetAlbumArtist(mp4Tags, value.Trim());

		else if	(key == INFO_CONTENTGROUP) ex_MP4TagsSetGrouping(mp4Tags,    value.Trim());

		else if	(key == INFO_COMPOSER)     ex_MP4TagsSetComposer(mp4Tags,    value.Trim());

		else if (key == INFO_BPM)
		{
			uint16_t	 tempo = value.ToInt();

			ex_MP4TagsSetTempo(mp4Tags, &tempo);
		}

		else if	(key == INFO_COPYRIGHT)    ex_MP4TagsSetCopyright(mp4Tags,   value.Trim());
	}

	/* Save encoder version.
	 */
	Application	*app = Application::Get();

	ex_MP4TagsSetEncodingTool(mp4Tags, app->getClientName.Call().Append(" ").Append(app->getClientVersion.Call()));

	/* Save cover art.
	 */
	if (coverArtWriteToTags && coverArtWriteToMP4)
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
	uint8_t	 mediaType = 1;					    //  1 == Music

	if	(track.outputFile.Contains(".m4b")) mediaType =  2; //  2 == Audiobook
	else if (track.outputFile.Contains(".m4r")) mediaType = 14; // 14 == Ringtone

	ex_MP4TagsSetMediaType(mp4Tags, &mediaType);

	ex_MP4TagsStore(mp4Tags, mp4File);
	ex_MP4TagsFree(mp4Tags);

	/* Save generic iTunes metadata.
	 */
	if (info.label != NIL) AddItmfItem(mp4File, "LABEL", info.label);
	if (info.isrc  != NIL) AddItmfItem(mp4File, "ISRC",  info.isrc);

	foreach (const String &pair, info.other)
	{
		String	 key   = pair.Head(pair.Find(":"));
		String	 value = pair.Tail(pair.Length() - pair.Find(":") - 1);

		if (value == NIL) continue;

		if	(key == INFO_SUBTITLE)	    AddItmfItem(mp4File, "SUBTITLE",	  value);

		else if	(key == INFO_CONDUCTOR)     AddItmfItem(mp4File, "CONDUCTOR",	  value);
		else if	(key == INFO_REMIXER)	    AddItmfItem(mp4File, "REMIXER",	  value);
		else if	(key == INFO_LYRICIST)	    AddItmfItem(mp4File, "LYRICIST",	  value);
		else if	(key == INFO_PRODUCER)	    AddItmfItem(mp4File, "PRODUCER",	  value);
		else if	(key == INFO_ENGINEER)	    AddItmfItem(mp4File, "ENGINEER",	  value);

		else if	(key == INFO_INITIALKEY)    AddItmfItem(mp4File, "initialkey",	  value);

		else if	(key == INFO_CATALOGNUMBER) AddItmfItem(mp4File, "CATALOGNUMBER", value);
		else if	(key == INFO_BARCODE)	    AddItmfItem(mp4File, "BARCODE",	  value);

		else if	(key == INFO_DISCSUBTITLE)  AddItmfItem(mp4File, "DISCSUBTITLE",  value);
	}

	/* Save Replay Gain info.
	 */
	if (preserveReplayGain)
	{
		if (info.track_gain != NIL && info.track_peak != NIL)
		{
			AddItmfItem(mp4File, "replaygain_track_gain", info.track_gain);
			AddItmfItem(mp4File, "replaygain_track_peak", info.track_peak);
		}

		if (info.album_gain != NIL && info.album_peak != NIL)
		{
			AddItmfItem(mp4File, "replaygain_album_gain", info.album_gain);
			AddItmfItem(mp4File, "replaygain_album_peak", info.album_peak);
		}
	}

	/* Save chapters.
	 */
	if (track.tracks.Length() > 0 && writeChapters)
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

		ex_MP4DeleteChapters(mp4File, MP4ChapterTypeAny, MP4_INVALID_TRACK_ID);
		ex_MP4SetChapters(mp4File, chapterList, chapterCount, (MP4ChapterType) currentConfig->GetIntValue(ConfigureMP4::ConfigID, "ChapterType", MP4ChapterTypeQt));

		delete [] chapterList;
	}

	ex_MP4Close(mp4File, 0);

	/* Optimize MP4 structure.
	 */
	String	 tempName = fileName.Append(".temp");

	ex_MP4Optimize(fileName.ConvertTo("UTF-8"), tempName.ConvertTo("UTF-8"));

	File(fileName).Delete();
	File(tempName).Move(fileName);

	return Success();
}

Bool BoCA::TaggerMP4::AddItmfItem(MP4FileHandle mp4File, const String &id, const String &value)
{
	/* Add iTunes metadata item.
	 */
	MP4ItmfItem	*item = ex_MP4ItmfItemAlloc("----", 1);

	item->mean = (char *) "com.apple.iTunes";
	item->name = id;

	item->dataList.elements[0].typeCode  = MP4_ITMF_BT_UTF8;
	item->dataList.elements[0].value     = (uint8_t *) value.ConvertTo("UTF-8");
	item->dataList.elements[0].valueSize = strlen((char *) item->dataList.elements[0].value);

	if (id == "ISRC") item->dataList.elements[0].typeCode = MP4_ITMF_BT_ISRC;

	ex_MP4ItmfAddItem(mp4File, item);

	item->mean = NIL;
	item->name = NIL;

	item->dataList.elements[0].typeCode  = MP4_ITMF_BT_IMPLICIT;
	item->dataList.elements[0].value     = NIL;
	item->dataList.elements[0].valueSize = 0;

	ex_MP4ItmfItemFree(item);

	return True;
}

Error BoCA::TaggerMP4::ParseStreamInfo(const String &fileName, Track &track)
{
	const Config	*currentConfig = GetConfiguration();

	Info		 info	 = track.GetInfo();

	MP4FileHandle	 mp4File = ex_MP4Read(fileName.ConvertTo("UTF-8"));

	if (mp4File == NIL) return Error();

	const MP4Tags	*mp4Tags = ex_MP4TagsAlloc();

	ex_MP4TagsFetch(mp4Tags, mp4File);

	String::InputFormat	 inputFormat("UTF-8");

	if	(mp4Tags->name	      != NIL) info.title   = String(mp4Tags->name).Trim();
	if	(mp4Tags->artist      != NIL) info.artist  = String(mp4Tags->artist).Trim();
	if	(mp4Tags->releaseDate != NIL) info.year    = String(mp4Tags->releaseDate).Trim().ToInt();
	if	(mp4Tags->album	      != NIL) info.album   = String(mp4Tags->album).Trim();
	if	(mp4Tags->comments    != NIL) info.comment = String(mp4Tags->comments).Trim();

	if	(mp4Tags->albumArtist != NIL) info.SetOtherInfo(INFO_ALBUMARTIST,  String(mp4Tags->albumArtist).Trim());

	if	(mp4Tags->grouping    != NIL) info.SetOtherInfo(INFO_CONTENTGROUP, String(mp4Tags->grouping).Trim());

	if	(mp4Tags->composer    != NIL) info.SetOtherInfo(INFO_COMPOSER,	   String(mp4Tags->composer).Trim());

	if	(mp4Tags->tempo	      != NIL) info.SetOtherInfo(INFO_BPM,	   String::FromInt(*mp4Tags->tempo));

	if	(mp4Tags->copyright   != NIL) info.SetOtherInfo(INFO_COPYRIGHT,	   String(mp4Tags->copyright).Trim());

	if	(mp4Tags->genre	      != NIL) info.genre   = String(mp4Tags->genre).Trim();
	else if (mp4Tags->genreType   != NIL) info.genre   = GetID3CategoryName(*mp4Tags->genreType - 1);

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

	if (currentConfig->GetIntValue(ConfigID, "CoverArtReadFromTags", True))
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

	/* Set artist to album artist if artist is not filled.
	 */
	if (info.artist == NIL) info.artist = info.GetOtherInfo(INFO_ALBUMARTIST);

	ex_MP4TagsFree(mp4Tags);

	/* Parse generic iTunes metadata.
	 */
	ParseItmfItems(mp4File, info);

	track.SetInfo(info);

	/* Read chapters.
	 */
	if (currentConfig->GetIntValue(ConfigID, "ReadChapters", True))
	{
		MP4Chapter_t	*chapterList  = NIL;
		uint32_t	 chapterCount = 0;

		ex_MP4GetChapters(mp4File, &chapterList, &chapterCount, MP4ChapterTypeAny);

		if (chapterList != NIL && chapterCount > 1)
		{
			MP4Duration	 offset = 0;

			for (UnsignedInt i = 0; i < chapterCount; i++)
			{
				const Format	&format = track.GetFormat();

				/* Fill track data.
				 */
				Track	 rTrack;

				rTrack.fileName	    = track.fileName;
				rTrack.pictures	    = track.pictures;

				rTrack.sampleOffset = Math::Round(Float(offset) / MP4_MSECS_TIME_SCALE * format.rate);

				if (chapterList[i].duration > 0 || i < chapterCount - 1)
				{
					rTrack.length = Math::Round(Float(chapterList[i].duration) / MP4_MSECS_TIME_SCALE * format.rate);
				}
				else
				{
					if	(track.length	    > 0) rTrack.length	     = track.length	  - rTrack.sampleOffset;
					else if (track.approxLength > 0) rTrack.approxLength = track.approxLength - rTrack.sampleOffset;
				}

				if	(track.length	    > 0) rTrack.fileSize = Math::Round(Float(track.fileSize) / track.length * rTrack.length);
				else if (track.approxLength > 0) rTrack.fileSize = Math::Round(Float(track.fileSize) / track.approxLength * rTrack.length);
				else				 rTrack.fileSize = rTrack.length * format.channels * (format.bits / 8);

				rTrack.SetFormat(format);

				/* Set track title.
				 */
				Info	 info = track.GetInfo();

				if (String(chapterList[i].title).Trim() != NIL) info.title = String(chapterList[i].title).Trim();

				info.track = i + 1;

				rTrack.SetInfo(info);

				/* Add track to track list.
				 */
				track.tracks.Add(rTrack);

				offset += chapterList[i].duration;
			}
		}

		if (chapterList != NIL) ex_MP4Free(chapterList);
	}

	ex_MP4Close(mp4File, 0);

	return Success();
}

Bool BoCA::TaggerMP4::ParseItmfItems(MP4FileHandle mp4File, Info &info)
{
	String::InputFormat	 inputFormat("UTF-8");

	/* Look for iTunes metadata items.
	 */
	MP4ItmfItemList	*items = ex_MP4ItmfGetItemsByMeaning(mp4File, "com.apple.iTunes", NIL);

	if (items != NIL)
	{
		/* Loop over items.
		 */
		for (UnsignedInt i = 0; i < items->size; i++)
		{
			MP4ItmfItem	 item = items->elements[i];

			/* Read and assign value string.
			 */
			String	 id    = String(item.name).ToUpper();
			String	 value = GetItmfItemValue(item);

			if (value == NIL) continue;

			if	(id == "LABEL"	   ||
				 id == "PUBLISHER")	info.label = value;

			else if (id == "ISRC")		info.isrc  = value;

			else if (id == "SUBTITLE")	info.SetOtherInfo(INFO_SUBTITLE,      value);

			else if (id == "CONDUCTOR")	info.SetOtherInfo(INFO_CONDUCTOR,     value);
			else if (id == "REMIXER")	info.SetOtherInfo(INFO_REMIXER,	      value);
			else if (id == "LYRICIST")	info.SetOtherInfo(INFO_LYRICIST,      value);
			else if (id == "PRODUCER")	info.SetOtherInfo(INFO_PRODUCER,      value);
			else if (id == "ENGINEER")	info.SetOtherInfo(INFO_ENGINEER,      value);

			else if (id == "INITIALKEY")	info.SetOtherInfo(INFO_INITIALKEY,    value);

			else if (id == "CATALOGNUMBER")	info.SetOtherInfo(INFO_CATALOGNUMBER, value);
			else if (id == "BARCODE")	info.SetOtherInfo(INFO_BARCODE,	      value);

			else if (id == "DISCSUBTITLE")	info.SetOtherInfo(INFO_DISCSUBTITLE,  value);

			else if (id.StartsWith("REPLAYGAIN"))
			{
				if	(id == "REPLAYGAIN_TRACK_GAIN") info.track_gain = value;
				else if (id == "REPLAYGAIN_TRACK_PEAK") info.track_peak = value;
				else if (id == "REPLAYGAIN_ALBUM_GAIN") info.album_gain = value;
				else if (id == "REPLAYGAIN_ALBUM_PEAK") info.album_peak = value;
			}
		}

		ex_MP4ItmfItemListFree(items);
	}

	/* Look for Replay Gain items.
	 */
	items = ex_MP4ItmfGetItemsByMeaning(mp4File, "org.hydrogenaudio.replaygain", NIL);

	if (items != NIL)
	{
		/* Loop over items.
		 */
		for (UnsignedInt i = 0; i < items->size; i++)
		{
			MP4ItmfItem	 item = items->elements[i];

			/* Read and assign value string.
			 */
			String	 id    = String(item.name).ToUpper();
			String	 value = GetItmfItemValue(item);

			if (value == NIL) continue;

			if	(id.StartsWith("REPLAYGAIN"))
			{
				if	(id == "REPLAYGAIN_TRACK_GAIN") info.track_gain = value;
				else if (id == "REPLAYGAIN_TRACK_PEAK") info.track_peak = value;
				else if (id == "REPLAYGAIN_ALBUM_GAIN") info.album_gain = value;
				else if (id == "REPLAYGAIN_ALBUM_PEAK") info.album_peak = value;
			}
		}

		ex_MP4ItmfItemListFree(items);
	}

	return True;
}

String BoCA::TaggerMP4::GetItmfItemValue(MP4ItmfItem &item)
{
	if (item.dataList.size == 0) return NIL;

	/* Check type code.
	 */
	if (item.dataList.elements[0].typeCode != MP4_ITMF_BT_UTF8 &&
	    item.dataList.elements[0].typeCode != MP4_ITMF_BT_URL  &&
	    item.dataList.elements[0].typeCode != MP4_ITMF_BT_ISRC) return NIL;

	/* Read value into buffer.
	 */
	Buffer<char>	 buffer(item.dataList.elements[0].valueSize + 1);

	memset(buffer, 0, item.dataList.elements[0].valueSize + 1);
	memcpy(buffer, item.dataList.elements[0].value, item.dataList.elements[0].valueSize);

	/* Return actual value.
	 */
	String::InputFormat	 inputFormat("UTF-8");

	return String(buffer);
}

Error BoCA::TaggerMP4::UpdateStreamInfo(const String &fileName, const Track &track)
{
	MP4FileHandle	 mp4File = ex_MP4Modify(fileName.ConvertTo("UTF-8"), 0);

	if (mp4File == NIL) return Error();

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
	ex_MP4TagsSetGrouping(mp4Tags, NIL);
	ex_MP4TagsSetComposer(mp4Tags, NIL);
	ex_MP4TagsSetTempo(mp4Tags, NIL);
	ex_MP4TagsSetCopyright(mp4Tags, NIL);
	ex_MP4TagsSetGenre(mp4Tags, NIL);
	ex_MP4TagsSetGenreType(mp4Tags, NIL);
	ex_MP4TagsSetComments(mp4Tags, NIL);

	for (Int i = mp4Tags->artworkCount - 1; i >= 0; i--)
	{
		ex_MP4TagsRemoveArtwork(mp4Tags, i);
	}

	ex_MP4TagsStore(mp4Tags, mp4File);

	ex_MP4TagsFree(mp4Tags);

	/* Remove iTunes metadata items too.
	 */
	RemoveItmfItem(mp4File, "LABEL");
	RemoveItmfItem(mp4File, "ISRC");
	RemoveItmfItem(mp4File, "SUBTITLE");
	RemoveItmfItem(mp4File, "CONDUCTOR");
	RemoveItmfItem(mp4File, "REMIXER");
	RemoveItmfItem(mp4File, "LYRICIST");
	RemoveItmfItem(mp4File, "CATALOGNUMBER");
	RemoveItmfItem(mp4File, "BARCODE");
	RemoveItmfItem(mp4File, "DISCSUBTITLE");

	ex_MP4Close(mp4File, 0);

	/* Now render the new metadata.
	 */
	RenderStreamInfo(fileName, track);

	return Success();
}

Bool BoCA::TaggerMP4::RemoveItmfItem(MP4FileHandle mp4File, const String &id)
{
	/* Look for iTunes metadata items.
	 */
	MP4ItmfItemList	*items = ex_MP4ItmfGetItemsByMeaning(mp4File, "com.apple.iTunes", id);

	if (items == NIL) return True;

	/* Loop over items and remove them.
	 */
	for (UnsignedInt i = 0; i < items->size; i++)
	{
		ex_MP4ItmfRemoveItem(mp4File, &items->elements[i]);
	}

	ex_MP4ItmfItemListFree(items);

	return True;
}

const String &BoCA::TaggerMP4::GetID3CategoryName(UnsignedInt id)
{
	static const String	 empty;

	if (id > 191) return empty;
	else	      return genres[id];
}

ConfigLayer *BoCA::TaggerMP4::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMP4();

	return configLayer;
}
