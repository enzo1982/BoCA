 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2025 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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

			static Void	 AddAlbumArt(Track &, const Picture &);

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

			S::Byte		 drive;
			S::Byte		 cdTrack;

			/* Attached pictures:
			 */
			Array<Picture>	 pictures;

			/* Tracks in album:
			 */
			Array<Track>	 tracks;

			/* CDDB information:
			 */
			UnsignedInt32	 discid;

			/* Lossless format information:
			 */
			Bool		 lossless;
			String		 md5;

			/* Filename information:
			 */
			String		 fileName;
			String		 outputFile;

			/* Decoder information:
			 */
			String		 decoderID;

					 Track(const int = NIL);
					 Track(const Track &);
					~Track();

			Track &operator	 =(const int);
			Track &operator	 =(const Track &);

			Bool operator	 ==(const int) const;
			Bool operator	 !=(const int) const;

			Void		 AdjustSampleCounts(const Format &);

			Bool		 LoadCoverArtFiles();
			Bool		 LoadCoverArtFile(const String &);

			Bool		 SaveCoverArtFiles(const String &);
		accessors:
			Int		 GetTrackID() const				{ return trackID; }

			Void		 SetFormat(const Format &nFormat)		{ format = nFormat; foreach (Track &track, tracks) track.SetFormat(format); }
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
