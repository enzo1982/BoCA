 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_INFO
#define H_BOCA_INFO

#include <smooth.h>
#include "../../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	const char * const INFO_BAND		= (char *) "Band/orchestra/accompaniment";
	const char * const INFO_PERFORMER	= (char *) "Conductor/performer refinement";
	const char * const INFO_REMIX		= (char *) "Interpreted, remixed or otherwise modified by";
	const char * const INFO_COMPOSER	= (char *) "Composer";
	const char * const INFO_LYRICIST	= (char *) "Lyricist/Text writer";

	const char * const INFO_ORIG_ARTIST	= (char *) "Original artist(s)/performer(s)";
	const char * const INFO_ORIG_ALBUM	= (char *) "Original album/movie/show title";
	const char * const INFO_ORIG_LYRICIST	= (char *) "Original lyricist(s)/text writer(s)";
	const char * const INFO_ORIG_YEAR	= (char *) "Original release year";

	const char * const INFO_WEB_ARTIST	= (char *) "Official artist webpage";
	const char * const INFO_WEB_PUBLISHER	= (char *) "Official publisher webpage";
	const char * const INFO_WEB_RADIO	= (char *) "Official radio webpage";
	const char * const INFO_WEB_SOURCE	= (char *) "Official audio source webpage";
	const char * const INFO_WEB_COPYRIGHT	= (char *) "Copyright information webpage";
	const char * const INFO_WEB_COMMERCIAL	= (char *) "Commercial information  webpage";

	class BOCA_DLL_EXPORT Info
	{
		public:
			/* General information:
			 */
			String			 artist;
			String			 title;
			String			 album;
			String			 genre;
			Int			 year;
			String			 comment;

			/* Disc information:
			 */
			Int			 track;
			Int			 numTracks;

			Int			 disc;
			Int			 numDiscs;

			/* Publisher information:
			 */
			String			 label;
			String			 isrc;

			/* Replay Gain information:
			 */
			String			 track_gain;
			String			 track_peak;
			String			 album_gain;
			String			 album_peak;

			/* CD-ROM table of contents:
			 */
			Buffer<UnsignedByte>	 mcdi;
			String			 offsets;

			/* Array of additional fields:
			 */
			Array<String>		 other;

			/* Class constructors / destructor:
			 */
						 Info();
						 Info(const Info &);
			virtual			~Info();

			Info &operator		 =(const Info &);
	};
};

#endif
