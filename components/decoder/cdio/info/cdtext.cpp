 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

		for (Int i = firstTrack; i <= lastTrack; i++)
		{
			cdInfo.SetTrackArtist(i, cdtext_get_const(cdtext, CDTEXT_FIELD_PERFORMER, i));
			cdInfo.SetTrackTitle(i, cdtext_get_const(cdtext, CDTEXT_FIELD_TITLE, i));
		}
	}
#else
	cdtext_t	*cdtext = cdio_get_cdtext(cd, 0);

	if (cdtext != NIL)
	{
		cdInfo.SetArtist(cdtext_get_const(CDTEXT_PERFORMER, cdtext));
		cdInfo.SetTitle(cdtext_get_const(CDTEXT_TITLE, cdtext));

		for (Int i = firstTrack; i <= lastTrack; i++)
		{
			cdtext_t	*cdtext = cdio_get_cdtext(cd, i);

			cdInfo.SetTrackArtist(i, cdtext_get_const(CDTEXT_PERFORMER, cdtext));
			cdInfo.SetTrackTitle(i, cdtext_get_const(CDTEXT_TITLE, cdtext));
		}
	}
#endif

	cdio_destroy(cd);

	return Success();
}

Int BoCA::CDText::ClearCDInfo()
{
	cdInfo.Clear();

	return Success();
}

const BoCA::CDInfo &BoCA::CDText::GetCDInfo() const
{
	return cdInfo;
}
