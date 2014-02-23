 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "layer.h"

BoCA::LayerLengthStatus::LayerLengthStatus()
{
	tracks.EnableLocking();
	tracks_selected.EnableLocking();
	tracks_unselected.EnableLocking();

	display_selected	= new LengthDisplay(ImageLoader::Load("freac.pci:18"));
	display_unselected	= new LengthDisplay(ImageLoader::Load("freac.pci:19"));
	display_all		= new LengthDisplay(ImageLoader::Load("freac.pci:20"));

	Add(display_selected);
	Add(display_unselected);
	Add(display_all);

	UpdateLengthDisplays();
	SetOrientation(OR_UPPERRIGHT);

	JobList::Get()->onApplicationAddTrack.Connect(&LayerLengthStatus::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Connect(&LayerLengthStatus::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationMarkTrack.Connect(&LayerLengthStatus::OnApplicationMarkTrack, this);
	JobList::Get()->onApplicationUnmarkTrack.Connect(&LayerLengthStatus::OnApplicationUnmarkTrack, this);

	JobList::Get()->onApplicationRemoveAllTracks.Connect(&LayerLengthStatus::OnApplicationRemoveAllTracks, this);
}

BoCA::LayerLengthStatus::~LayerLengthStatus()
{
	JobList::Get()->onApplicationAddTrack.Disconnect(&LayerLengthStatus::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Disconnect(&LayerLengthStatus::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationMarkTrack.Disconnect(&LayerLengthStatus::OnApplicationMarkTrack, this);
	JobList::Get()->onApplicationUnmarkTrack.Disconnect(&LayerLengthStatus::OnApplicationUnmarkTrack, this);

	JobList::Get()->onApplicationRemoveAllTracks.Disconnect(&LayerLengthStatus::OnApplicationRemoveAllTracks, this);

	DeleteObject(display_selected);
	DeleteObject(display_unselected);
	DeleteObject(display_all);
}

Void BoCA::LayerLengthStatus::UpdateLengthDisplays()
{
	Surface	*surface = GetDrawSurface();

	if (IsRegistered())
	{
		surface->StartPaint(Rect(container->GetRealPosition(), container->GetRealSize()));
	}

	display_selected->SetText(GetTotalLengthString(tracks_selected));
	display_unselected->SetText(GetTotalLengthString(tracks_unselected));
	display_all->SetText(GetTotalLengthString(tracks));

	display_selected->SetPosition(Point(0, 0));
	display_unselected->SetPosition(Point(display_selected->GetWidth() + 3, 0));
	display_all->SetPosition(Point(display_selected->GetWidth() + display_unselected->GetWidth() + 6, 0));

	SetSize(Size(display_all->GetWidth() + display_selected->GetWidth() + display_unselected->GetWidth() + 6, display_all->GetHeight()));

	if (IsRegistered())
	{
		container->Paint(SP_UPDATE);

		surface->EndPaint();
	}
}

const String &BoCA::LayerLengthStatus::GetTotalLengthString(const Array<Track> &tracks)
{
	static String	 string;

	Int		 seconds = 0;
	Bool		 approx	 = False;
	Bool		 unknown = False;

	for (Int i = 0; i < tracks.Length(); i++)
	{
		const Track	&track	= tracks.GetNth(i);
		const Format	&format = track.GetFormat();

		if (track.length >= 0 && format.rate > 0)
		{
			seconds += track.length / format.rate;
		}
		else if (track.approxLength >= 0 && format.rate > 0)
		{
			seconds += track.approxLength / format.rate;

			approx = True;
		}
		else
		{
			unknown = True;
		}
	}

	string = String(unknown ? "> " : NIL).Append(approx ? "~ " : NIL)
		.Append(seconds >= 3600 ? String(seconds / 3600	     < 10 ? "0" : NIL).Append(String::FromInt(seconds / 3600	 )).Append(":") : String())
					 .Append(seconds % 3600 / 60 < 10 ? "0" : NIL).Append(String::FromInt(seconds % 3600 / 60)).Append(":")
					 .Append(seconds % 3600 % 60 < 10 ? "0" : NIL).Append(String::FromInt(seconds % 3600 % 60));

	if (Setup::enableUnicode)
	{
		static wchar_t	 sign[2] = { 0x2248, 0 };

		string.Replace("~", sign);
	}

	return string;
}

/* Called when a track is added to the application joblist.
 * ----
 * Adds entries to tracks.
 */
Void BoCA::LayerLengthStatus::OnApplicationAddTrack(const Track &track)
{
	tracks.Add(track);
	tracks_selected.Add(track);

	UpdateLengthDisplays();
}

/* Called when a track is removed from the application joblist.
 * ----
 * Removes our corresponding entry from tracks.
 */
Void BoCA::LayerLengthStatus::OnApplicationRemoveTrack(const Track &track)
{
	for (Int i = 0; i < tracks.Length(); i++)
	{
		if (tracks.GetNth(i).GetTrackID() == track.GetTrackID())
		{
			tracks.RemoveNth(i);

			break;
		}
	}

	for (Int i = 0; i < tracks_selected.Length(); i++)
	{
		if (tracks_selected.GetNth(i).GetTrackID() == track.GetTrackID())
		{
			tracks_selected.RemoveNth(i);

			break;
		}
	}

	for (Int i = 0; i < tracks_unselected.Length(); i++)
	{
		if (tracks_unselected.GetNth(i).GetTrackID() == track.GetTrackID())
		{
			tracks_unselected.RemoveNth(i);

			break;
		}
	}

	UpdateLengthDisplays();
}

/* Called when a track is marked by the application.
 * ----
 * Adds the track to tracks_selected, removes it from tracks_unselected.
 */
Void BoCA::LayerLengthStatus::OnApplicationMarkTrack(const Track &track)
{
	tracks_selected.Add(track);

	for (Int i = 0; i < tracks_unselected.Length(); i++)
	{
		if (tracks_unselected.GetNth(i).GetTrackID() == track.GetTrackID())
		{
			tracks_unselected.RemoveNth(i);

			break;
		}
	}

	UpdateLengthDisplays();
}

/* Called when a track is unmarked in the application joblist.
 * ----
 * Adds the track to tracks_unselected, removes it from tracks_selected.
 */
Void BoCA::LayerLengthStatus::OnApplicationUnmarkTrack(const Track &track)
{
	tracks_unselected.Add(track);

	for (Int i = 0; i < tracks_selected.Length(); i++)
	{
		if (tracks_selected.GetNth(i).GetTrackID() == track.GetTrackID())
		{
			tracks_selected.RemoveNth(i);

			break;
		}
	}

	UpdateLengthDisplays();
}

/* Called when all track are removed from the application joblist at once.
 * ----
 * Clears all internal arrays.
 */
Void BoCA::LayerLengthStatus::OnApplicationRemoveAllTracks()
{
	tracks.RemoveAll();
	tracks_selected.RemoveAll();
	tracks_unselected.RemoveAll();

	UpdateLengthDisplays();
}
