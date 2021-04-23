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

#include "cdtext.h"

#include <cdio/cdio.h>

BoCA::CDText::CDText()
{
}

BoCA::CDText::~CDText()
{
}

Int BoCA::CDText::ReadCDText(const String &path)
{
	CdIo_t	*cd = cdio_open(path, DRIVER_UNKNOWN);

	if (cd == NIL) return Error();

	cdInfo.Clear();

	Int	 firstTrack = cdio_get_first_track_num(cd);
	Int	 lastTrack  = cdio_get_last_track_num(cd);

#if LIBCDIO_VERSION_NUM >= 90
	cdtext_t	*cdtext = cdio_get_cdtext(cd);

	if (cdtext != NIL)
	{
		cdInfo.SetArtist(cdtext_get_const(cdtext, CDTEXT_FIELD_PERFORMER, 0));
		cdInfo.SetTitle(cdtext_get_const(cdtext, CDTEXT_FIELD_TITLE, 0));
		cdInfo.SetSongwriter(cdtext_get_const(cdtext, CDTEXT_FIELD_SONGWRITER, 0));
		cdInfo.SetComposer(cdtext_get_const(cdtext, CDTEXT_FIELD_COMPOSER, 0));
		cdInfo.SetArranger(cdtext_get_const(cdtext, CDTEXT_FIELD_ARRANGER, 0));
		cdInfo.SetComment(cdtext_get_const(cdtext, CDTEXT_FIELD_MESSAGE, 0));
		cdInfo.SetGenre(cdtext_get_const(cdtext, CDTEXT_FIELD_GENRE, 0));
		cdInfo.SetCatalog(cdtext_get_const(cdtext, CDTEXT_FIELD_DISCID, 0));
		cdInfo.SetBarcode(cdtext_get_const(cdtext, CDTEXT_FIELD_UPC_EAN, 0));

		if (cdInfo.GetGenre() == NIL)
		{
			cdtext_genre_t	 genre	  = cdtext_get_genre(cdtext);
			cdtext_genre_t	 genre_le = cdtext_genre_t((genre & 0xFF) << 8 | genre >> 8);

			if	(genre	  < 28) cdInfo.SetGenre(cdtext_genre2str(genre));
			else if (genre_le < 28) cdInfo.SetGenre(cdtext_genre2str(genre_le));
		}

		for (Int i = firstTrack; i <= lastTrack; i++)
		{
			cdInfo.SetTrackArtist(i, cdtext_get_const(cdtext, CDTEXT_FIELD_PERFORMER, i));
			cdInfo.SetTrackTitle(i, cdtext_get_const(cdtext, CDTEXT_FIELD_TITLE, i));
			cdInfo.SetTrackSongwriter(i, cdtext_get_const(cdtext, CDTEXT_FIELD_SONGWRITER, i));
			cdInfo.SetTrackComposer(i, cdtext_get_const(cdtext, CDTEXT_FIELD_COMPOSER, i));
			cdInfo.SetTrackArranger(i, cdtext_get_const(cdtext, CDTEXT_FIELD_ARRANGER, i));
			cdInfo.SetTrackComment(i, cdtext_get_const(cdtext, CDTEXT_FIELD_MESSAGE, i));
			cdInfo.SetTrackISRC(i, cdtext_get_const(cdtext, CDTEXT_FIELD_ISRC, i));
		}
	}
#else
	cdtext_t	*cdtext = cdio_get_cdtext(cd, 0);

	if (cdtext != NIL)
	{
		cdInfo.SetArtist(cdtext_get_const(CDTEXT_PERFORMER, cdtext));
		cdInfo.SetTitle(cdtext_get_const(CDTEXT_TITLE, cdtext));
		cdInfo.SetSongwriter(cdtext_get_const(CDTEXT_SONGWRITER, cdtext));
		cdInfo.SetComposer(cdtext_get_const(CDTEXT_COMPOSER, cdtext));
		cdInfo.SetArranger(cdtext_get_const(CDTEXT_ARRANGER, cdtext));
		cdInfo.SetComment(cdtext_get_const(CDTEXT_MESSAGE, cdtext));
		cdInfo.SetGenre(cdtext_get_const(CDTEXT_GENRE, cdtext));
		cdInfo.SetCatalog(cdtext_get_const(CDTEXT_DISCID, cdtext));
		cdInfo.SetBarcode(cdtext_get_const(CDTEXT_UPC_EAN, cdtext));

		for (Int i = firstTrack; i <= lastTrack; i++)
		{
			cdtext_t	*cdtext = cdio_get_cdtext(cd, i);

			cdInfo.SetTrackArtist(i, cdtext_get_const(CDTEXT_PERFORMER, cdtext));
			cdInfo.SetTrackTitle(i, cdtext_get_const(CDTEXT_TITLE, cdtext));
			cdInfo.SetTrackSongwriter(i, cdtext_get_const(CDTEXT_SONGWRITER, cdtext));
			cdInfo.SetTrackComposer(i, cdtext_get_const(CDTEXT_COMPOSER, cdtext));
			cdInfo.SetTrackArranger(i, cdtext_get_const(CDTEXT_ARRANGER, cdtext));
			cdInfo.SetTrackComment(i, cdtext_get_const(CDTEXT_MESSAGE, cdtext));
			cdInfo.SetTrackISRC(i, cdtext_get_const(CDTEXT_ISRC, cdtext));
		}
	}
#endif

	cdio_destroy(cd);

	return Success();
}

const BoCA::CDInfo &BoCA::CDText::GetCDInfo() const
{
	return cdInfo;
}
