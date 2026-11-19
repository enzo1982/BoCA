 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2023 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::TaggerMP4::ConfigID		= "Tags";

const String	 BoCA::TaggerMP4::ItmfMeaningiTunes	= "com.apple.iTunes";
const String	 BoCA::TaggerMP4::ItmfMeaningReplayGain	= "org.hydrogenaudio.replaygain";

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

	Bool		 albumArtWriteToTags	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToTags", True);
	Bool		 albumArtWriteToMP4	 = currentConfig->GetIntValue(ConfigID, "CoverArtWriteToMP4Metadata", True);

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

	/* Artist, genre, label and ISRC can have several values. An item
	 * with several values gets one data atom for each value. The MP4Tags
	 * functions of mp4v2 write only one, so items with several values
	 * are added after the tags are stored.
	 */
	Array<String>	 artists = Info::GetInfoList(&info, INFO_ARTISTS);
	Array<String>	 genres	 = Info::GetInfoList(&info, INFO_GENRES);
	Array<String>	 labels	 = Info::GetInfoList(&info, INFO_LABELS);
	Array<String>	 isrcs	 = Info::GetInfoList(&info, INFO_ISRCS);

	if	(artists.Length() == 1) ex_MP4TagsSetArtist(mp4Tags, artists.GetFirst().Trim());
	else if (artists.Length()  > 1) ex_MP4TagsSetArtist(mp4Tags, NIL);

	if (info.title  != NIL) ex_MP4TagsSetName(mp4Tags, info.title.Trim());
	if (info.album  != NIL) ex_MP4TagsSetAlbum(mp4Tags, info.album.Trim());
	if (info.year    >   0) ex_MP4TagsSetReleaseDate(mp4Tags, String::FromInt(info.year));

	if	(genres.Length() == 1) ex_MP4TagsSetGenre(mp4Tags, genres.GetFirst().Trim());
	else if (genres.Length()  > 1) { ex_MP4TagsSetGenre(mp4Tags, NIL); ex_MP4TagsSetGenreType(mp4Tags, NIL); }

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

	if	(info.comment != NIL && !replaceExistingComments) ex_MP4TagsSetComments(mp4Tags, info.comment);
	else if (defaultComment != NIL)				  ex_MP4TagsSetComments(mp4Tags, String(defaultComment).Trim());

	/* Save encoder version.
	 */
	Application	*app = Application::Get();

	ex_MP4TagsSetEncodingTool(mp4Tags, app->getClientName.Call().Append(" ").Append(app->getClientVersion.Call()));

	/* Save album art.
	 */
	if (albumArtWriteToTags && albumArtWriteToMP4)
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

		/* Add album art to tag.
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

	/* Save other metadata atoms.
	 */
	if (artists.Length() > 1) AddItmfItem(mp4File, L"©ART", NIL, NIL, artists);
	if (genres.Length()  > 1) AddItmfItem(mp4File, L"©gen", NIL, NIL, genres);

	if (info.rating >= 0) AddItmfItem(mp4File, "rate", NIL, NIL, String::FromInt(info.rating));

	Array<String>	 keys = Info::GetOtherInfoKeys(&info);

	foreach (const String &key, keys)
	{
		Array<String>	 values = Info::GetOtherInfoList(&info, key);
		String		 value;

		/* Use the first non-blank value; skip keys without one.
		 */
		for (Int i = 0; i < values.Length() && value == NIL; i++) value = values.GetNth(i).Trim();

		if (value == NIL) continue;

		if	(key == INFO_ALBUMARTIST)      AddItmfItem(mp4File,  "aART", NIL, NIL, values);

		else if	(key == INFO_CONTENTGROUP)     AddItmfItem(mp4File, L"©grp", NIL, NIL, values);

		else if	(key == INFO_COMPOSER)         AddItmfItem(mp4File, L"©wrt", NIL, NIL, values);

		else if	(key == INFO_MOVEMENT)	       AddItmfItem(mp4File, L"©mvi", NIL, NIL, value, MP4_ITMF_BT_INTEGER);
		else if	(key == INFO_MOVEMENTTOTAL)    AddItmfItem(mp4File, L"©mvc", NIL, NIL, value, MP4_ITMF_BT_INTEGER);
		else if	(key == INFO_MOVEMENTNAME)     AddItmfItem(mp4File, L"©mvn", NIL, NIL, values);

		else if (key == INFO_BPM)	       AddItmfItem(mp4File,  "tmpo", NIL, NIL, value, MP4_ITMF_BT_INTEGER);

		else if	(key == INFO_COPYRIGHT)        AddItmfItem(mp4File,  "cprt", NIL, NIL, values);

		else if	(key == INFO_LYRICS)	       AddItmfItem(mp4File, L"©lyr", NIL, NIL, values.GetFirst());

		else if	(key == INFO_SORT_ARTIST)      AddItmfItem(mp4File,  "soar", NIL, NIL, values);
		else if	(key == INFO_SORT_ALBUM)       AddItmfItem(mp4File,  "soal", NIL, NIL, values);
		else if	(key == INFO_SORT_ALBUMARTIST) AddItmfItem(mp4File,  "soaa", NIL, NIL, values);
		else if	(key == INFO_SORT_COMPOSER)    AddItmfItem(mp4File,  "soco", NIL, NIL, values);
		else if	(key == INFO_SORT_TITLE)       AddItmfItem(mp4File,  "sonm", NIL, NIL, values);
	}

	/* Save generic iTunes metadata.
	 */
	if (labels.Length() > 0) AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "LABEL", labels);
	if (isrcs.Length()  > 0) AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "ISRC",  isrcs, MP4_ITMF_BT_ISRC);

	foreach (const String &key, keys)
	{
		Array<String>	 values = Info::GetOtherInfoList(&info, key);
		String		 value;

		/* Use the first non-blank value; skip keys without one.
		 */
		for (Int i = 0; i < values.Length() && value == NIL; i++) value = values.GetNth(i).Trim();

		if (value == NIL) continue;

		if	(key == INFO_SUBTITLE)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "SUBTITLE",				values);

		else if	(key == INFO_CONDUCTOR)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "CONDUCTOR",				values);
		else if	(key == INFO_REMIXER)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "REMIXER",				values);
		else if	(key == INFO_LYRICIST)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "LYRICIST",				values);
		else if	(key == INFO_PRODUCER)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "PRODUCER",				values);
		else if	(key == INFO_ENGINEER)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "ENGINEER",				values);

		else if	(key == INFO_INITIALKEY)		   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "initialkey",			values);

		else if	(key == INFO_MEDIATYPE)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MEDIA",				values);
		else if	(key == INFO_CATALOGNUMBER)		   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "CATALOGNUMBER",			values);
		else if	(key == INFO_BARCODE)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "BARCODE",				values);

		else if	(key == INFO_DISCSUBTITLE)		   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "DISCSUBTITLE",			values);

		else if	(key == INFO_SCRIPT)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "SCRIPT",				values);

		else if	(key == INFO_ASIN)			   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "ASIN",				values);

		else if	(key == INFO_MUSICBRAINZ_ARTISTID)	   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Artist Id",		values);
		else if	(key == INFO_MUSICBRAINZ_ALBUMID)	   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Id",		values);
		else if	(key == INFO_MUSICBRAINZ_ALBUMARTISTID)	   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Artist Id",	values);
		else if	(key == INFO_MUSICBRAINZ_WORKID)	   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Work Id",		values);
		else if	(key == INFO_MUSICBRAINZ_DISCID)	   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Disc Id",		values);
		else if	(key == INFO_MUSICBRAINZ_TRACKID)	   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Track Id",		values);
		else if	(key == INFO_MUSICBRAINZ_ORIGINALARTISTID) AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Original Artist Id",	values);
		else if	(key == INFO_MUSICBRAINZ_ORIGINALALBUMID)  AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Original Album Id",	values);
		else if	(key == INFO_MUSICBRAINZ_RELEASEGROUPID)   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Release Group Id",	values);
		else if	(key == INFO_MUSICBRAINZ_RELEASETRACKID)   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Release Track Id",	values);
		else if	(key == INFO_MUSICBRAINZ_TRMID)		   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz TRM Id",		values);

		else if	(key == INFO_MUSICBRAINZ_RELEASETYPE)	   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Type",		values);
		else if	(key == INFO_MUSICBRAINZ_RELEASESTATUS)	   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Status",		values);

		else if	(key == INFO_RELEASECOUNTRY)		   AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Release Country",	values);
	}

	/* Save Replay Gain info.
	 */
	if (preserveReplayGain)
	{
		if (info.track_gain != NIL && info.track_peak != NIL)
		{
			AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "replaygain_track_gain", info.track_gain);
			AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "replaygain_track_peak", info.track_peak);
		}

		if (info.album_gain != NIL && info.album_peak != NIL)
		{
			AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "replaygain_album_gain", info.album_gain);
			AddItmfItem(mp4File, "----", ItmfMeaningiTunes, "replaygain_album_peak", info.album_peak);
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

Bool BoCA::TaggerMP4::AddItmfItem(MP4FileHandle mp4File, const String &code, const String &meaning, const String &id, const String &value, MP4ItmfBasicType type)
{
	/* Add iTunes metadata item.
	 */
	MP4ItmfItem	*item = ex_MP4ItmfItemAlloc(code.ConvertTo("ISO-8859-1"), 1);
	Buffer<uint8_t>	 buffer(2);

	item->mean = meaning;
	item->name = id;

	item->dataList.elements[0].typeCode  = type;

	if (type == MP4_ITMF_BT_UTF8 ||
	    type == MP4_ITMF_BT_URL ||
	    type == MP4_ITMF_BT_ISRC)
	{
		item->dataList.elements[0].value     = (uint8_t *) value.ConvertTo("UTF-8");
		item->dataList.elements[0].valueSize = strlen((char *) item->dataList.elements[0].value);
	}
	else if (type == MP4_ITMF_BT_INTEGER)
	{
		Int	 number = value.ToInt();
		Int	 size	= 2;

		for (Int i = 0; i < size; i++)
		{
			buffer[i] = (number >> 8 * (size - i - 1)) & 0xFF;
		}

		item->dataList.elements[0].value     = buffer;
		item->dataList.elements[0].valueSize = size;
	}

	ex_MP4ItmfAddItem(mp4File, item);

	item->code = NIL;
	item->mean = NIL;
	item->name = NIL;

	item->dataList.elements[0].typeCode  = MP4_ITMF_BT_IMPLICIT;
	item->dataList.elements[0].value     = NIL;
	item->dataList.elements[0].valueSize = 0;

	ex_MP4ItmfItemFree(item);

	return True;
}

/* Copy a string as UTF-8 into a new buffer that is added to a list.
 */
static char *CopyUTF8(const String &string, Array<Buffer<char> *> &buffers)
{
	const char	*utf8	  = string.ConvertTo("UTF-8");
	Int		 utf8Size = strlen(utf8);
	Buffer<char>	*buffer	  = new Buffer<char>(utf8Size + 1);

	memcpy(*buffer, utf8, utf8Size + 1);

	buffers.Add(buffer);

	return *buffer;
}

Bool BoCA::TaggerMP4::AddItmfItem(MP4FileHandle mp4File, const String &code, const String &meaning, const String &id, const Array<String> &values, MP4ItmfBasicType type)
{
	Array<String>	 strings;

	foreach (const String &value, values) if (value.Trim() != NIL) strings.Add(value.Trim());

	if (strings.Length() == 0) return False;

	/* Keep converted strings alive until the item is added. String
	 * keeps only a few recent conversions, so the value conversions
	 * would free the mean and name conversions if they were not copied.
	 */
	Array<Buffer<char> *>	 buffers;
	MP4ItmfItem		*item = ex_MP4ItmfItemAlloc(code.ConvertTo("ISO-8859-1"), strings.Length());

	if (meaning != NIL) item->mean = CopyUTF8(meaning, buffers);
	if (id	    != NIL) item->name = CopyUTF8(id, buffers);

	for (Int i = 0; i < strings.Length(); i++)
	{
		item->dataList.elements[i].typeCode  = type;
		item->dataList.elements[i].value     = (uint8_t *) CopyUTF8(strings.GetNth(i), buffers);
		item->dataList.elements[i].valueSize = strlen((char *) item->dataList.elements[i].value);
	}

	ex_MP4ItmfAddItem(mp4File, item);

	/* Detach borrowed pointers before freeing the item. The code
	 * belongs to the item, because MP4ItmfItemAlloc copies it.
	 */
	item->mean = NIL;
	item->name = NIL;

	for (Int i = 0; i < strings.Length(); i++)
	{
		item->dataList.elements[i].typeCode  = MP4_ITMF_BT_IMPLICIT;
		item->dataList.elements[i].value     = NIL;
		item->dataList.elements[i].valueSize = 0;
	}

	ex_MP4ItmfItemFree(item);

	foreach (Buffer<char> *buffer, buffers) delete buffer;

	return True;
}

Error BoCA::TaggerMP4::ParseStreamInfo(const String &fileName, Track &track)
{
	const Config	*currentConfig = GetConfiguration();

	Info		 info	 = track.GetInfo();

	/* Values from this tag replace the values that tags read before gave
	 * the same keys; several values of a key in this tag add up.
	 */
	Array<String>	 previousOther = info.other;

	info.other.RemoveAll();

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

	ex_MP4TagsFree(mp4Tags);

	/* Parse generic iTunes metadata.
	 */
	ParseItmfItems(mp4File, info, previousOther);

	/* Set artist to album artist if artist is not filled.
	 */
	if (info.artist == NIL) info.artist = info.GetOtherInfo(INFO_ALBUMARTIST);

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

				if (chapterList[i].duration > 0 && i < chapterCount - 1)
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

/* Parse the iTunes metadata items, then add the values of earlier tags
 * that this tag does not have.
 */
Bool BoCA::TaggerMP4::ParseItmfItems(MP4FileHandle mp4File, Info &info, const Array<String> &previousOther)
{
	String::InputFormat	 inputFormat("UTF-8");

	/* Artist, genre, label and ISRC can have several values in this tag.
	 */
	Array<String>	 artists;
	Array<String>	 genres;
	Array<String>	 labels;
	Array<String>	 isrcs;

	/* Look for known atoms.
	 */
	MP4ItmfItemList	*items = ex_MP4ItmfGetItems(mp4File);

	if (items != NIL)
	{
		/* Loop over items.
		 */
		for (UnsignedInt i = 0; i < items->size; i++)
		{
			MP4ItmfItem	 item = items->elements[i];

			/* Read and assign value string.
			 */
			String	 code;

			code.ImportFrom("ISO-8859-1", item.code);

			String		 value	= GetItmfItemValue(item);
			Array<String>	 values	= GetItmfItemValues(item);

			/* Fields with several values use values, which skips blank
			 * data, so an item can have values when its first data is
			 * blank. Fields with one value check value != NIL.
			 */
			if (value == NIL && values.Length() == 0) continue;

			else if	(code ==  "rate" && value != NIL) info.rating = Math::Min(100, value.ToInt());

			else if	(code ==  "aART") Info::AddOtherInfo(&info, INFO_ALBUMARTIST,	   values);

			else if	(code == L"©grp") Info::AddOtherInfo(&info, INFO_CONTENTGROUP,	   values);

			else if	(code == L"©wrt") Info::AddOtherInfo(&info, INFO_COMPOSER,	   values);

			/* MP4Tags gives only the first artist and genre.
			 */
			else if	(code == L"©ART") { foreach (const String &text, values) artists.Add(text); }
			else if	(code == L"©gen") { foreach (const String &text, values) genres.Add(text); }

			if	(code == L"©mvi" && value != NIL) info.SetOtherInfo(INFO_MOVEMENT,         value);
			else if (code == L"©mvc" && value != NIL) info.SetOtherInfo(INFO_MOVEMENTTOTAL,    value);
			else if	(code == L"©mvn") Info::AddOtherInfo(&info, INFO_MOVEMENTNAME,     values);

			else if	(code ==  "tmpo")
			{
				if (value.ToInt() > 0) info.SetOtherInfo(INFO_BPM, value);
			}

			else if	(code ==  "cprt") Info::AddOtherInfo(&info, INFO_COPYRIGHT,	   values);

			else if	(code == L"©lyr" && value != NIL) info.SetOtherInfo(INFO_LYRICS,	   value);

			else if (code ==  "soar") Info::AddOtherInfo(&info, INFO_SORT_ARTIST,	   values);
			else if (code ==  "soal") Info::AddOtherInfo(&info, INFO_SORT_ALBUM,	   values);
			else if (code ==  "soaa") Info::AddOtherInfo(&info, INFO_SORT_ALBUMARTIST, values);
			else if (code ==  "soco") Info::AddOtherInfo(&info, INFO_SORT_COMPOSER,	   values);
			else if (code ==  "sonm") Info::AddOtherInfo(&info, INFO_SORT_TITLE,	   values);
		}

		ex_MP4ItmfItemListFree(items);
	}

	/* Look for iTunes metadata items.
	 */
	items = ex_MP4ItmfGetItemsByMeaning(mp4File, ItmfMeaningiTunes, NIL);

	if (items != NIL)
	{
		/* Loop over items.
		 */
		for (UnsignedInt i = 0; i < items->size; i++)
		{
			MP4ItmfItem	 item = items->elements[i];

			/* Read and assign value string.
			 */
			String		 id	= String(item.name).ToUpper();
			String		 value	= GetItmfItemValue(item).Trim();
			Array<String>	 values	= GetItmfItemValues(item);

			/* Fields with several values use values, which skips blank
			 * data. Fields with one value check value != NIL.
			 */
			if (value == NIL && values.Length() == 0) continue;

			if	(id == "LABEL"	   ||
				 id == "PUBLISHER") { foreach (const String &text, values) labels.Add(text); }

			else if (id == "ISRC")
			{
				foreach (const String &text, values) if (Info::IsISRC(text)) isrcs.Add(text);
			}

			else if (id == "SUBTITLE")	Info::AddOtherInfo(&info, INFO_SUBTITLE,      values);

			else if (id == "CONDUCTOR")	Info::AddOtherInfo(&info, INFO_CONDUCTOR,     values);
			else if (id == "REMIXER")	Info::AddOtherInfo(&info, INFO_REMIXER,	      values);
			else if (id == "LYRICIST")	Info::AddOtherInfo(&info, INFO_LYRICIST,      values);
			else if (id == "PRODUCER")	Info::AddOtherInfo(&info, INFO_PRODUCER,      values);
			else if (id == "ENGINEER")	Info::AddOtherInfo(&info, INFO_ENGINEER,      values);

			else if (id == "INITIALKEY")	Info::AddOtherInfo(&info, INFO_INITIALKEY,    values);

			else if (id == "MEDIA")		Info::AddOtherInfo(&info, INFO_MEDIATYPE,     values);
			else if (id == "CATALOGNUMBER")	Info::AddOtherInfo(&info, INFO_CATALOGNUMBER, values);
			else if (id == "BARCODE")	Info::AddOtherInfo(&info, INFO_BARCODE,	      values);

			else if (id == "DISCSUBTITLE")	Info::AddOtherInfo(&info, INFO_DISCSUBTITLE,  values);

			else if (id == "SCRIPT")	Info::AddOtherInfo(&info, INFO_SCRIPT,	      values);

			else if (id == "ASIN")		Info::AddOtherInfo(&info, INFO_ASIN,	      values);

			else if (id.StartsWith("MUSICBRAINZ"))
			{
				if	(id == "MUSICBRAINZ ARTIST ID")		    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_ARTISTID,	 values);
				else if	(id == "MUSICBRAINZ ALBUM ID")		    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_ALBUMID,		 values);
				else if	(id == "MUSICBRAINZ ALBUM ARTIST ID")	    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_ALBUMARTISTID,	 values);
				else if	(id == "MUSICBRAINZ WORK ID")		    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_WORKID,		 values);
				else if	(id == "MUSICBRAINZ DISC ID")		    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_DISCID,		 values);
				else if	(id == "MUSICBRAINZ TRACK ID")		    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_TRACKID,		 values);
				else if (id == "MUSICBRAINZ ORIGINAL ARTIST ID")    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_ORIGINALARTISTID, values);
				else if	(id == "MUSICBRAINZ ORIGINAL ALBUM ID")	    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_ORIGINALALBUMID,	 values);
				else if	(id == "MUSICBRAINZ RELEASE GROUP ID")	    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_RELEASEGROUPID,	 values);
				else if	(id == "MUSICBRAINZ RELEASE TRACK ID")	    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_RELEASETRACKID,	 values);
				else if	(id == "MUSICBRAINZ TRM ID")		    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_TRMID,		 values);

				else if	(id == "MUSICBRAINZ ALBUM TYPE")	    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_RELEASETYPE,	 values);
				else if	(id == "MUSICBRAINZ ALBUM STATUS")	    Info::AddOtherInfo(&info, INFO_MUSICBRAINZ_RELEASESTATUS,	 values);

				else if	(id == "MUSICBRAINZ ALBUM RELEASE COUNTRY") Info::AddOtherInfo(&info, INFO_RELEASECOUNTRY,		 values);
			}

			else if (id.StartsWith("REPLAYGAIN") && value != NIL)
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
	items = ex_MP4ItmfGetItemsByMeaning(mp4File, ItmfMeaningReplayGain, NIL);

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

	Info::AddMissingOtherInfo(&info, previousOther);

	/* Set the lists of this tag after the values of earlier tags, so
	 * that one value of this tag replaces a list of an earlier tag.
	 */
	if (artists.Length() > 0) Info::SetInfoList(&info, INFO_ARTISTS, artists);
	if (genres.Length()  > 0) Info::SetInfoList(&info, INFO_GENRES,	 genres);
	if (labels.Length()  > 0) Info::SetInfoList(&info, INFO_LABELS,	 labels);
	if (isrcs.Length()   > 0) Info::SetInfoList(&info, INFO_ISRCS,	 isrcs);

	return True;
}

String BoCA::TaggerMP4::GetItmfItemValue(MP4ItmfItem &item)
{
	if (item.dataList.size == 0) return NIL;

	/* Check type code.
	 */
	if (item.dataList.elements[0].typeCode == MP4_ITMF_BT_UTF8 ||
	    item.dataList.elements[0].typeCode == MP4_ITMF_BT_URL  ||
	    item.dataList.elements[0].typeCode == MP4_ITMF_BT_ISRC)
	{
		return GetItmfDataString(item.dataList.elements[0]);
	}
	else if (item.dataList.elements[0].typeCode == MP4_ITMF_BT_INTEGER)
	{
		Int64	 value = 0;

		for (UnsignedInt i = 0; i < item.dataList.elements[0].valueSize; i++)
		{
			value |= item.dataList.elements[0].value[i] << (8 * (item.dataList.elements[0].valueSize - i - 1));
		}

		return String::FromInt(value);
	}

	return NIL;
}

String BoCA::TaggerMP4::GetItmfDataString(const MP4ItmfData &data)
{
	/* Read value into buffer.
	 */
	Buffer<char>	 buffer(data.valueSize + 1);

	memset(buffer, 0, data.valueSize + 1);
	memcpy(buffer, data.value, data.valueSize);

	/* Return actual value.
	 */
	String::InputFormat	 inputFormat("UTF-8");

	return String(buffer);
}

Array<String> BoCA::TaggerMP4::GetItmfItemValues(MP4ItmfItem &item)
{
	Array<String>	 values;

	for (UnsignedInt i = 0; i < item.dataList.size; i++)
	{
		MP4ItmfData	&data = item.dataList.elements[i];

		if (data.typeCode != MP4_ITMF_BT_UTF8 &&
		    data.typeCode != MP4_ITMF_BT_URL  &&
		    data.typeCode != MP4_ITMF_BT_ISRC) continue;

		String	 value = GetItmfDataString(data);

		if (value.Trim() != NIL) values.Add(value.Trim());
	}

	return values;
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
	ex_MP4TagsSetGenre(mp4Tags, NIL);
	ex_MP4TagsSetGenreType(mp4Tags, NIL);
	ex_MP4TagsSetComments(mp4Tags, NIL);

	for (Int i = mp4Tags->artworkCount - 1; i >= 0; i--)
	{
		ex_MP4TagsRemoveArtwork(mp4Tags, i);
	}

	ex_MP4TagsStore(mp4Tags, mp4File);

	ex_MP4TagsFree(mp4Tags);

	/* Remove known atoms. The artist and genre items are removed here
	 * too, because items with several values are not written by MP4Tags.
	 */
	RemoveItmfItem(mp4File, L"©ART");
	RemoveItmfItem(mp4File, L"©gen");

	RemoveItmfItem(mp4File,  "aART");

	RemoveItmfItem(mp4File, L"©grp");

	RemoveItmfItem(mp4File, L"©wrt");

	RemoveItmfItem(mp4File, L"©mvi");
	RemoveItmfItem(mp4File, L"©mvc");
	RemoveItmfItem(mp4File, L"©mvn");

	RemoveItmfItem(mp4File,  "tmpo");

	RemoveItmfItem(mp4File,  "cprt");

	RemoveItmfItem(mp4File, L"©lyr");

	RemoveItmfItem(mp4File,  "soar");
	RemoveItmfItem(mp4File,  "soal");
	RemoveItmfItem(mp4File,  "soaa");
	RemoveItmfItem(mp4File,  "soco");
	RemoveItmfItem(mp4File,  "sonm");

	/* Remove iTunes metadata items too.
	 */
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "LABEL");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "PUBLISHER");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "ISRC");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "SUBTITLE");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "CONDUCTOR");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "REMIXER");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "LYRICIST");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "PRODUCER");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "ENGINEER");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "initialkey");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MEDIA");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "CATALOGNUMBER");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "BARCODE");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "DISCSUBTITLE");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "SCRIPT");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "ASIN");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Artist Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Artist Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Work Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Disc Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Track Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Original Artist Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Original Album Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Release Group Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Release Track Id");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz TRM Id");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Type");
	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Status");

	RemoveItmfItem(mp4File, "----", ItmfMeaningiTunes, "MusicBrainz Album Release Country");

	ex_MP4Close(mp4File, 0);

	/* Now render the new metadata.
	 */
	RenderStreamInfo(fileName, track);

	return Success();
}

Bool BoCA::TaggerMP4::RemoveItmfItem(MP4FileHandle mp4File, const String &code, const String &meaning, const String &id)
{
	/* Look for iTunes metadata items.
	 */
	MP4ItmfItemList	*items = NIL;

	if (code == "----") items = ex_MP4ItmfGetItemsByMeaning(mp4File, meaning, NIL);
	else		    items = ex_MP4ItmfGetItemsByCode(mp4File, code.ConvertTo("ISO-8859-1"));

	if (items == NIL) return True;

	/* Loop over items and remove them. Freeform items are found by
	 * name without regard to case, because the reader does the same.
	 */
	for (UnsignedInt i = 0; i < items->size; i++)
	{
		const MP4ItmfItem	&item = items->elements[i];

		if (code == "----" && (item.name == NIL || String(item.name).ToUpper() != id.ToUpper())) continue;

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
