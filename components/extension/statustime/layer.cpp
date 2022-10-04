 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "layer.h"

BoCA::LayerLengthStatus::LayerLengthStatus()
{
	String	 resourcesPath;

#ifndef __WIN32__
	if (Directory(S::System::System::GetResourcesDirectory().Append("freac")).Exists()) resourcesPath = S::System::System::GetResourcesDirectory().Append("freac").Append(Directory::GetDirectoryDelimiter());
#endif

	tracks.EnableLocking();
	tracks_selected.EnableLocking();
	tracks_unselected.EnableLocking();

	seconds			= 0;
	approx			= 0;
	unknown			= 0;

	seconds_selected	= 0;
	approx_selected		= 0;
	unknown_selected	= 0;

	seconds_unselected	= 0;
	approx_unselected	= 0;
	unknown_unselected	= 0;

	display_selected	= new LengthDisplay(ImageLoader::Load(String(resourcesPath).Append("icons/select/select-all.png")));
	display_unselected	= new LengthDisplay(ImageLoader::Load(String(resourcesPath).Append("icons/select/select-none.png")));
	display_all		= new LengthDisplay(ImageLoader::Load(String(resourcesPath).Append("icons/select/select-toggle.png")));

	Add(display_selected);
	Add(display_unselected);
	Add(display_all);

	UpdateLengthDisplays();
	SetOrientation(OR_UPPERRIGHT);

	/* Connect slots.
	 */
	JobList	*joblist = JobList::Get();

	joblist->onApplicationAddTrack.Connect(&LayerLengthStatus::OnApplicationAddTrack, this);
	joblist->onApplicationModifyTrack.Connect(&LayerLengthStatus::OnApplicationModifyTrack, this);
	joblist->onApplicationRemoveTrack.Connect(&LayerLengthStatus::OnApplicationRemoveTrack, this);
	joblist->onApplicationMarkTrack.Connect(&LayerLengthStatus::OnApplicationMarkTrack, this);
	joblist->onApplicationUnmarkTrack.Connect(&LayerLengthStatus::OnApplicationUnmarkTrack, this);

	joblist->onApplicationRemoveAllTracks.Connect(&LayerLengthStatus::OnApplicationRemoveAllTracks, this);
}

BoCA::LayerLengthStatus::~LayerLengthStatus()
{
	/* Disconnect slots.
	 */
	JobList	*joblist = JobList::Get();

	joblist->onApplicationAddTrack.Disconnect(&LayerLengthStatus::OnApplicationAddTrack, this);
	joblist->onApplicationModifyTrack.Disconnect(&LayerLengthStatus::OnApplicationModifyTrack, this);
	joblist->onApplicationRemoveTrack.Disconnect(&LayerLengthStatus::OnApplicationRemoveTrack, this);
	joblist->onApplicationMarkTrack.Disconnect(&LayerLengthStatus::OnApplicationMarkTrack, this);
	joblist->onApplicationUnmarkTrack.Disconnect(&LayerLengthStatus::OnApplicationUnmarkTrack, this);

	joblist->onApplicationRemoveAllTracks.Disconnect(&LayerLengthStatus::OnApplicationRemoveAllTracks, this);

	DeleteObject(display_selected);
	DeleteObject(display_unselected);
	DeleteObject(display_all);
}

Void BoCA::LayerLengthStatus::UpdateLengthDisplays()
{
	/* Update displayed values.
	 */
	Surface	*surface = (IsVisible() ? GetDrawSurface() : NIL);

	if (surface) surface->StartPaint(container->GetVisibleArea());

	display_selected->SetText(GetLengthString(seconds_selected, approx_selected, unknown_selected));
	display_unselected->SetText(GetLengthString(seconds_unselected, approx_unselected, unknown_unselected));
	display_all->SetText(GetLengthString(seconds, approx, unknown));

	display_selected->SetPosition(Point(0, 0));
	display_unselected->SetPosition(Point(display_selected->GetWidth() + 3, 0));
	display_all->SetPosition(Point(display_selected->GetWidth() + display_unselected->GetWidth() + 6, 0));

	SetSize(Size(display_all->GetWidth() + display_selected->GetWidth() + display_unselected->GetWidth() + 6, display_all->GetHeight()));

	if (surface)
	{
		container->Paint(SP_UPDATE);

		surface->EndPaint();
	}
}

Void BoCA::LayerLengthStatus::AddTrack(const Track &track, Int64 &seconds, Int &approx, Int &unknown)
{
	const Format	&format = track.GetFormat();

	if (track.length >= 0 && format.rate > 0)
	{
		seconds += Math::Round(Float(track.length) / format.rate);
	}
	else if (track.approxLength >= 0 && format.rate > 0)
	{
		seconds += Math::Round(Float(track.approxLength) / format.rate);

		approx++;
	}
	else
	{
		unknown++;
	}
}

Void BoCA::LayerLengthStatus::RemoveTrack(const Track &track, Int64 &seconds, Int &approx, Int &unknown)
{
	const Format	&format = track.GetFormat();

	if (track.length >= 0 && format.rate > 0)
	{
		seconds -= Math::Round(Float(track.length) / format.rate);
	}
	else if (track.approxLength >= 0 && format.rate > 0)
	{
		seconds -= Math::Round(Float(track.approxLength) / format.rate);

		approx--;
	}
	else
	{
		unknown--;
	}
}

const String &BoCA::LayerLengthStatus::GetLengthString(Int64 seconds, Int approx, Int unknown)
{
	static String	 string;
	static wchar_t	 sign[2] = { 0x2248, 0 };

	string = String(unknown ? "> " : NIL).Append(approx ? String(sign).Append(" ") : String())
		.Append(seconds >= 3600 ? String(seconds / 3600	     < 10 ? "0" : NIL).Append(String::FromInt(seconds / 3600	 )).Append(":") : String())
					 .Append(seconds % 3600 / 60 < 10 ? "0" : NIL).Append(String::FromInt(seconds % 3600 / 60)).Append(":")
					 .Append(seconds % 3600 % 60 < 10 ? "0" : NIL).Append(String::FromInt(seconds % 3600 % 60));

	return string;
}

/* Called when a track is added to the application joblist.
 * ----
 * Adds entries to tracks.
 */
Void BoCA::LayerLengthStatus::OnApplicationAddTrack(const Track &track)
{
	tracks.Add(track, track.GetTrackID());
	tracks_selected.Add(track, track.GetTrackID());

	AddTrack(track, seconds, approx, unknown);
	AddTrack(track, seconds_selected, approx_selected, unknown_selected);

	UpdateLengthDisplays();
}

/* Called when a track is modified in the application joblist.
 * ----
 * Updates track entries.
 */
Void BoCA::LayerLengthStatus::OnApplicationModifyTrack(const Track &track)
{
	const Track	&knownTrack = tracks.Get(track.GetTrackID());

	RemoveTrack(knownTrack, seconds, approx, unknown);
	AddTrack(track, seconds, approx, unknown);

	tracks.Remove(track.GetTrackID());
	tracks.Add(track, track.GetTrackID());

	if (tracks_selected.Get(track.GetTrackID()) != NIL)
	{
		const Track	&knownTrack = tracks_selected.Get(track.GetTrackID());

		RemoveTrack(knownTrack, seconds_selected, approx_selected, unknown_selected);
		AddTrack(track, seconds_selected, approx_selected, unknown_selected);

		tracks_selected.Remove(track.GetTrackID());
		tracks_selected.Add(track, track.GetTrackID());
	}
	else if (tracks_unselected.Remove(track.GetTrackID()) != NIL)
	{
		const Track	&knownTrack = tracks_unselected.Get(track.GetTrackID());

		RemoveTrack(knownTrack, seconds_unselected, approx_unselected, unknown_unselected);
		AddTrack(track, seconds_unselected, approx_unselected, unknown_unselected);

		tracks_unselected.Remove(track.GetTrackID());
		tracks_unselected.Add(track, track.GetTrackID());
	}

	UpdateLengthDisplays();
}

/* Called when a track is removed from the application joblist.
 * ----
 * Removes our corresponding entry from tracks.
 */
Void BoCA::LayerLengthStatus::OnApplicationRemoveTrack(const Track &track)
{
	tracks.Remove(track.GetTrackID());

	RemoveTrack(track, seconds, approx, unknown);

	if	(tracks_selected.Remove(track.GetTrackID()))   RemoveTrack(track, seconds_selected, approx_selected, unknown_selected);
	else if (tracks_unselected.Remove(track.GetTrackID())) RemoveTrack(track, seconds_unselected, approx_unselected, unknown_unselected);

	UpdateLengthDisplays();
}

/* Called when a track is marked by the application.
 * ----
 * Adds the track to tracks_selected, removes it from tracks_unselected.
 */
Void BoCA::LayerLengthStatus::OnApplicationMarkTrack(const Track &track)
{
	tracks_selected.Add(track, track.GetTrackID());
	tracks_unselected.Remove(track.GetTrackID());

	AddTrack(track, seconds_selected, approx_selected, unknown_selected);
	RemoveTrack(track, seconds_unselected, approx_unselected, unknown_unselected);

	UpdateLengthDisplays();
}

/* Called when a track is unmarked in the application joblist.
 * ----
 * Adds the track to tracks_unselected, removes it from tracks_selected.
 */
Void BoCA::LayerLengthStatus::OnApplicationUnmarkTrack(const Track &track)
{
	tracks_unselected.Add(track, track.GetTrackID());
	tracks_selected.Remove(track.GetTrackID());

	AddTrack(track, seconds_unselected, approx_unselected, unknown_unselected);
	RemoveTrack(track, seconds_selected, approx_selected, unknown_selected);

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

	seconds		   = 0;
	approx		   = 0;
	unknown		   = 0;

	seconds_selected   = 0;
	approx_selected	   = 0;
	unknown_selected   = 0;

	seconds_unselected = 0;
	approx_unselected  = 0;
	unknown_unselected = 0;

	UpdateLengthDisplays();
}
