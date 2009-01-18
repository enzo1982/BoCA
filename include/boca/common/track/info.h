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

			/* CD-ROM table of contents
			 */
			Buffer<UnsignedByte>	 mcdi;
			String			 offsets;

			/* Class constructors / destructor:
			 */
						 Info();
						 Info(const Info &);
			virtual			~Info();

			Info &operator		 =(const Info &);
	};
};

#endif
