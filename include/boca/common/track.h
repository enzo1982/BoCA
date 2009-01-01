 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
		public:
			/* Length and file size information:
			 */
			Int64		 length;
			Int64		 approxLength;
			Int64		 fileSize;

			/* CD track information:
			 */
			Bool		 isCDTrack;
			Int		 drive;
			Int		 cdTrack;

			/* Title information:
			 */
			String		 artist;
			String		 title;
			String		 album;
			Int		 track;
			String		 genre;
			Int		 year;
			String		 comment;

			String		 label;
			String		 isrc;

			String		 oArtist;
			String		 oTitle;
			String		 oAlbum;
			String		 oGenre;

			/* Attached pictures:
			 */
			Array<Picture>	 pictures;

			/* CDDB information:
			 */
			Int		 offset;
			String		 discid;
			String		 category;
			Int		 revision;
			Int		 disclength;
			String		 discComment;
			String		 playorder;

			/* Other information:
			 */
			String		 fileSizeString;
			String		 lengthString;

			String		 outfile;
			String		 origFilename;

					 Track();
					 Track(int);
					 Track(const Track &);
			virtual		~Track();

			Track &operator	 =(const int);
			Track &operator	 =(const Track &);

			Bool operator	 ==(const int) const;
			Bool operator	 !=(const int) const;

			Int		 RenderID3Tag(Buffer<UnsignedByte> &, Int = 2);
			Bool		 ParseID3Tag(const Buffer<UnsignedByte> &);
			Bool		 ParseID3Tag(const String &);

			Int		 RenderAPETag(Buffer<UnsignedByte> &);
			Bool		 ParseAPETag(const Buffer<UnsignedByte> &);
			Bool		 ParseAPETag(const String &);

			Bool		 RenderMP4Meta(const String &);
			Bool		 ParseMP4Meta(const String &);

			Int		 RenderVorbisComment(Buffer<UnsignedByte> &, const String & = "BoCA Tagging API");
			Bool		 ParseVorbisComment(const Buffer<UnsignedByte> &);
			Bool		 ParseVorbisComment(const String &);
		accessors:
			Int		 GetTrackID() const				{ return trackID; }

			Void		 SetFormat(const Format &nFormat)		{ format = nFormat; }
			Format		&GetFormat()					{ return format; }
			const Format	&GetFormat() const				{ return format; }
	};
};

#endif
