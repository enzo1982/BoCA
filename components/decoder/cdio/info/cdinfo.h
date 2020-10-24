 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_CDIO_CDINFO
#define H_CDIO_CDINFO

#include <smooth.h>

using namespace smooth;

namespace BoCA
{
	class CDInfo
	{
		private:
			String		 artist;
			String		 title;
			String		 songwriter;
			String		 composer;
			String		 arranger;
			String		 comment;
			String		 genre;
			String		 catalog;
			String		 barcode;

			Array<String>	 trackArtists;
			Array<String>	 trackTitles;
			Array<String>	 trackSongwriters;
			Array<String>	 trackComposers;
			Array<String>	 trackArrangers;
			Array<String>	 trackComments;
			Array<String>	 trackISRCs;

			Void		 ReserveTracks(Int);
		public:
					 CDInfo();
					~CDInfo();

			Int		 Clear();
		accessors:
			/* Per-disc information.
			 */
			Void		 SetArtist(const String &nArtist)		{ artist = nArtist; }
			const String	&GetArtist() const				{ return artist; }

			Void		 SetTitle(const String &nTitle)			{ title = nTitle; }
			const String	&GetTitle() const				{ return title; }

			Void		 SetSongwriter(const String &nSongwriter)	{ songwriter = nSongwriter; }
			const String	&GetSongwriter() const				{ return songwriter; }

			Void		 SetComposer(const String &nComposer)		{ composer = nComposer; }
			const String	&GetComposer() const				{ return composer; }

			Void		 SetArranger(const String &nArranger)		{ arranger = nArranger; }
			const String	&GetArranger() const				{ return arranger; }

			Void		 SetComment(const String &nComment)		{ comment = nComment; }
			const String	&GetComment() const				{ return comment; }

			Void		 SetGenre(const String &nGenre)			{ genre = nGenre; }
			const String	&GetGenre() const				{ return genre; }

			Void		 SetCatalog(const String &nCatalog)		{ catalog = nCatalog; }
			const String	&GetCatalog() const				{ return catalog; }

			Void		 SetBarcode(const String &nBarcode)		{ barcode = nBarcode; }
			const String	&GetBarcode() const				{ return barcode; }

			/* Optional per-track information.
			 */
			Void		 SetTrackArtist(Int n, const String &);
			const String	&GetTrackArtist(Int n) const			{ return trackArtists.GetNth(n - 1); }

			Void		 SetTrackTitle(Int n, const String &);
			const String	&GetTrackTitle(Int n) const			{ return trackTitles.GetNth(n - 1); }

			Void		 SetTrackSongwriter(Int n, const String &);
			const String	&GetTrackSongwriter(Int n) const		{ return trackSongwriters.GetNth(n - 1); }

			Void		 SetTrackComposer(Int n, const String &);
			const String	&GetTrackComposer(Int n) const			{ return trackComposers.GetNth(n - 1); }

			Void		 SetTrackArranger(Int n, const String &);
			const String	&GetTrackArranger(Int n) const			{ return trackArrangers.GetNth(n - 1); }

			Void		 SetTrackComment(Int n, const String &);
			const String	&GetTrackComment(Int n) const			{ return trackComments.GetNth(n - 1); }

			Void		 SetTrackISRC(Int n, const String &);
			const String	&GetTrackISRC(Int n) const			{ return trackISRCs.GetNth(n - 1); }
	};
};

#endif
