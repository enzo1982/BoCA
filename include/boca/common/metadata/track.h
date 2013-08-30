 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_TRACK
#define H_BOCA_TRACK

#include <smooth.h>
#include "picture.h"
#include "format.h"
#include "info.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT Track
	{
		private:
			static Int	 nextTrackID;

			Int		 trackID;
		protected:
			/* Audio format information:
			 */
			Format		 format;

			/* Title information:
			 */
			Info		 info;
			Info		 originalInfo;
		public:
			/* Length and file size information:
			 */
			Int64		 sampleOffset;

			Int64		 length;
			Int64		 approxLength;

			Int64		 fileSize;

			/* CD track information:
			 */
			Bool		 isCDTrack;
			Byte		 drive;
			Byte		 cdTrack;

			/* Attached pictures:
			 */
			Array<Picture>	 pictures;

			/* Tracks in album:
			 */
			Array<Track>	 tracks;

			/* CDDB information:
			 */
			Int		 discid;

			/* Other information:
			 */
			String		 outfile;
			String		 origFilename;

					 Track();
					 Track(int);
					 Track(const Track &);
					~Track();

			Track &operator	 =(const int);
			Track &operator	 =(const Track &);

			Bool operator	 ==(const int) const;
			Bool operator	 !=(const int) const;

			Bool		 LoadCoverArtFiles();
			Bool		 LoadCoverArtFile(const String &);

			Bool		 SaveCoverArtFiles(const String &);
		accessors:
			Int		 GetTrackID() const				{ return trackID; }

			Void		 SetFormat(const Format &nFormat)		{ format = nFormat; }
			const Format	&GetFormat() const				{ return format; }

			Void		 SetInfo(const Info &nInfo)			{ info = nInfo; }
			const Info	&GetInfo() const				{ return info; }

			Void		 SetOriginalInfo(const Info &nOriginalInfo)	{ originalInfo = nOriginalInfo; }
			const Info	&GetOriginalInfo() const			{ return originalInfo; }

			String		 GetLengthString() const;
			String		 GetFileSizeString() const;
	};
};

#endif
