 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_INFO
#define H_BOCA_INFO

#include <smooth.h>
#include "mcdi.h"

using namespace smooth;

namespace BoCA
{
	const char * const INFO_ALBUMARTIST			= "Album artist";

	const char * const INFO_CONTENTGROUP			= "Content group description";
	const char * const INFO_SUBTITLE			= "Subtitle/Description refinement";

	const char * const INFO_BAND				= "Band/orchestra/accompaniment";
	const char * const INFO_PERFORMER			= "Performer";
	const char * const INFO_CONDUCTOR			= "Conductor/performer refinement";
	const char * const INFO_REMIXER				= "Interpreted, remixed or otherwise modified by";
	const char * const INFO_COMPOSER			= "Composer";
	const char * const INFO_LYRICIST			= "Lyricist/Text writer";
	const char * const INFO_ARRANGER			= "Arranger";
	const char * const INFO_PRODUCER			= "Producer";
	const char * const INFO_ENGINEER			= "Audio engineer";

	const char * const INFO_MOVEMENT			= "Movement";
	const char * const INFO_MOVEMENTTOTAL			= "Total movements";
	const char * const INFO_MOVEMENTNAME			= "Movement name";

	const char * const INFO_ORIG_ARTIST			= "Original artist(s)/performer(s)";
	const char * const INFO_ORIG_ALBUM			= "Original album/movie/show title";
	const char * const INFO_ORIG_LYRICIST			= "Original lyricist(s)/text writer(s)";
	const char * const INFO_ORIG_YEAR			= "Original release year";

	const char * const INFO_BPM				= "BPM (beats per minute)";
	const char * const INFO_INITIALKEY			= "Initial key";

	const char * const INFO_COPYRIGHT			= "Copyright";

	const char * const INFO_MEDIATYPE			= "Source media type";
	const char * const INFO_CATALOGNUMBER			= "Source media catalog number";
	const char * const INFO_BARCODE				= "Source media EAN/UPC barcode";

	const char * const INFO_RELEASECOUNTRY			= "Album release country";

	const char * const INFO_DISCSUBTITLE			= "Disc/set subtitle";

	const char * const INFO_LYRICS				= "Unsynchronized lyrics";

	const char * const INFO_RADIOSTATION			= "Internet radio station name";
	const char * const INFO_RADIOOWNER			= "Internet radio station owner";

	const char * const INFO_USERTEXT			= "User defined text";

	const char * const INFO_WEB_ARTIST			= "Official artist webpage";
	const char * const INFO_WEB_PUBLISHER			= "Official publisher webpage";
	const char * const INFO_WEB_RADIO			= "Official radio webpage";
	const char * const INFO_WEB_SOURCE			= "Official audio source webpage";
	const char * const INFO_WEB_COPYRIGHT			= "Copyright information webpage";
	const char * const INFO_WEB_COMMERCIAL			= "Commercial information  webpage";

	const char * const INFO_WEB_USERURL			= "User defined URL";

	const char * const INFO_MUSICBRAINZ_ARTISTID		= "MusicBrainz Artist ID";
	const char * const INFO_MUSICBRAINZ_ALBUMID		= "MusicBrainz Album ID";
	const char * const INFO_MUSICBRAINZ_ALBUMARTISTID	= "MusicBrainz Album Artist ID";
	const char * const INFO_MUSICBRAINZ_WORKID		= "MusicBrainz Work ID";
	const char * const INFO_MUSICBRAINZ_DISCID		= "MusicBrainz Disc ID";
	const char * const INFO_MUSICBRAINZ_TRACKID		= "MusicBrainz Track ID";
	const char * const INFO_MUSICBRAINZ_ORIGINALARTISTID	= "MusicBrainz Original Artist ID";
	const char * const INFO_MUSICBRAINZ_ORIGINALALBUMID	= "MusicBrainz Original Album ID";
	const char * const INFO_MUSICBRAINZ_RELEASEGROUPID	= "MusicBrainz Release Group ID";
	const char * const INFO_MUSICBRAINZ_RELEASETRACKID	= "MusicBrainz Release Track ID";
	const char * const INFO_MUSICBRAINZ_TRMID		= "MusicBrainz TRM ID";

	const char * const INFO_MUSICBRAINZ_RELEASETYPE		= "MusicBrainz Release Type";
	const char * const INFO_MUSICBRAINZ_RELEASESTATUS	= "MusicBrainz Release Status";

	const char * const INFO_PRE_EMPHASIS			= "Pre-emphasis";

	class BOCA_DLL_EXPORT Info
	{
		public:
			/* General information:
			 */
			String			 artist;
			String			 title;
			String			 album;
			String			 genre;
			String			 comment;
			Short			 year;

			/* Disc information:
			 */
			Short			 track;
			Short			 numTracks;

			S::Byte			 disc;
			S::Byte			 numDiscs;

			/* Publisher information:
			 */
			String			 label;
			String			 isrc;

			/* Rating information:
			 */
			Int			 rating;

			/* Replay Gain information:
			 */
			String			 track_gain;
			String			 track_peak;
			String			 album_gain;
			String			 album_peak;

			/* CD-ROM table of contents:
			 */
			MCDI			 mcdi;
			String			 offsets;

			/* Array of additional fields:
			 */
			Array<String>		 other;

			/* Helper functions:
			 */
			Bool			 HasBasicInfo() const;
			Bool			 HasOtherInfo(const String &) const;

			String			 GetOtherInfo(const String &) const;
			Bool			 SetOtherInfo(const String &, const String &);

			static Bool		 IsISRC(const String &);

			/* Class constructors / destructor:
			 */
						 Info();
						 Info(const Info &);
						~Info();

			Info &operator		 =(const Info &);
	};
};

#endif
