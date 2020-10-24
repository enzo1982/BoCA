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

#include "cdinfo.h"

BoCA::CDInfo::CDInfo()
{
}

BoCA::CDInfo::~CDInfo()
{
}

Int BoCA::CDInfo::Clear()
{
	artist	   = NIL;
	title	   = NIL;
	songwriter = NIL;
	composer   = NIL;
	arranger   = NIL;
	comment	   = NIL;
	genre	   = NIL;
	catalog	   = NIL;
	barcode	   = NIL;

	trackArtists.RemoveAll();
	trackTitles.RemoveAll();
	trackSongwriters.RemoveAll();
	trackComposers.RemoveAll();
	trackArrangers.RemoveAll();
	trackComments.RemoveAll();
	trackISRCs.RemoveAll();

	return Success();
}

Void BoCA::CDInfo::ReserveTracks(Int n)
{
	while (trackArtists.Length() < n)
	{
		trackArtists.Add(NIL);
		trackTitles.Add(NIL);
		trackSongwriters.Add(NIL);
		trackComposers.Add(NIL);
		trackArrangers.Add(NIL);
		trackComments.Add(NIL);
		trackISRCs.Add(NIL);
	}
}

Void BoCA::CDInfo::SetTrackArtist(Int n, const String &nArtist)
{
	ReserveTracks(n);

	if (nArtist.Trim() != NIL) trackArtists.SetNth(n - 1, nArtist);
}

Void BoCA::CDInfo::SetTrackTitle(Int n, const String &nTitle)
{
	ReserveTracks(n);

	if (nTitle.Trim() != NIL) trackTitles.SetNth(n - 1, nTitle);
}

Void BoCA::CDInfo::SetTrackSongwriter(Int n, const String &nSongwriter)
{
	ReserveTracks(n);

	if (nSongwriter.Trim() != NIL) trackSongwriters.SetNth(n - 1, nSongwriter);
}

Void BoCA::CDInfo::SetTrackComposer(Int n, const String &nComposer)
{
	ReserveTracks(n);

	if (nComposer.Trim() != NIL) trackComposers.SetNth(n - 1, nComposer);
}

Void BoCA::CDInfo::SetTrackArranger(Int n, const String &nArranger)
{
	ReserveTracks(n);

	if (nArranger.Trim() != NIL) trackArrangers.SetNth(n - 1, nArranger);
}

Void BoCA::CDInfo::SetTrackComment(Int n, const String &nComment)
{
	ReserveTracks(n);

	if (nComment.Trim() != NIL) trackComments.SetNth(n - 1, nComment);
}

Void BoCA::CDInfo::SetTrackISRC(Int n, const String &nISRC)
{
	ReserveTracks(n);

	if (nISRC.Trim() != NIL) trackISRCs.SetNth(n - 1, nISRC.Replace(" ", NIL).Replace("-", NIL));
}
