 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "chooser_albums.h"

BoCA::ChooserAlbums::ChooserAlbums() : Chooser("Albums")
{
	dontUpdateAlbumList = False;

	list_albums	= new ListBox(Point(7, 7), Size(100, 150));
	list_albums->onSelectEntry.Connect(&ChooserAlbums::OnSelectAlbum, this);
	list_albums->AddTab(I18n::Get()->TranslateString("Title"));

	Add(list_albums);

	onChangeSize.Connect(&ChooserAlbums::OnChangeSize, this);

	JobList::Get()->onApplicationAddTrack.Connect(&ChooserAlbums::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Connect(&ChooserAlbums::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Connect(&ChooserAlbums::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationSelectTrack.Connect(&ChooserAlbums::OnApplicationSelectTrack, this);

	JobList::Get()->onApplicationRemoveAllTracks.Connect(&ChooserAlbums::OnApplicationRemoveAllTracks, this);
}

BoCA::ChooserAlbums::~ChooserAlbums()
{
	JobList::Get()->onApplicationAddTrack.Disconnect(&ChooserAlbums::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Disconnect(&ChooserAlbums::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Disconnect(&ChooserAlbums::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationSelectTrack.Disconnect(&ChooserAlbums::OnApplicationSelectTrack, this);

	JobList::Get()->onApplicationRemoveAllTracks.Disconnect(&ChooserAlbums::OnApplicationRemoveAllTracks, this);

	DeleteObject(list_albums);
}

/* Called when component canvas size changes.
 * ----
 */
Void BoCA::ChooserAlbums::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	list_albums->SetSize(Size(clientSize.cx - 15, clientSize.cy - 15));
}

/* Called when an album entry is selected.
 * ----
 * Finds the corresponding album and emits onSelectAlbum.
 */
Void BoCA::ChooserAlbums::OnSelectAlbum()
{
	const Track	&album = albums.GetNth(list_albums->GetSelectedEntryNumber());

	if (IsActiveChooser()) onSelectAlbum.Emit(album);
}

/* Called when an album is modified.
 * ----
 * Modifies our corresponding entry accordingly.
 */
Void BoCA::ChooserAlbums::OnModifyTrack(const Track &track)
{
	for (Int i = 0; i < list_albums->Length(); i++)
	{
		if (albums.Get(list_albums->GetNthEntry(i)->GetHandle()).GetTrackID() == track.GetTrackID())
		{
			const Info	&info = track.GetInfo();
			String		 jlEntry = String(info.artist.Length() > 0 ? info.artist : I18n::Get()->TranslateString("unknown artist")).Append(" - ").Append(info.album.Length() > 0 ? info.album : I18n::Get()->TranslateString("unknown album")).Append("\t");

			list_albums->GetNthEntry(i)->SetText(jlEntry);

			dontUpdateAlbumList = True;

			Track		&origAlbum = albums.GetReference(list_albums->GetNthEntry(i)->GetHandle());
	
			/* Modify tracks and emit onComponentModifyTrack for each affected track.
			 */
			for (Int j = 0; j < tracks.Length(); j++)
			{
				Track	&track = tracks.GetNthReference(j);
				Info	&trackInfo = track.GetInfo();

				if (!IsAlbumIdentical(track, origAlbum)) continue;

				trackInfo.artist	= info.artist;
				trackInfo.album		= info.album;
				trackInfo.genre		= info.genre;
				trackInfo.year		= info.year;
				trackInfo.comment	= info.comment;

				trackInfo.numTracks	= info.numTracks;

				trackInfo.disc		= info.disc;
				trackInfo.numDiscs	= info.numDiscs;

				JobList::Get()->onComponentModifyTrack.Emit(track);
			}

			origAlbum = track;

			dontUpdateAlbumList = False;

			break;
		}
	}
}

/* Called when a track is added to the application joblist.
 * ----
 * Adds entries to tracks and list_tracks.
 */
Void BoCA::ChooserAlbums::OnApplicationAddTrack(const Track &track)
{
	tracks.Add(track);

	UpdateAlbumList();
}

/* Called when a track is modified by the application.
 * ----
 * Modifies our corresponding entry accordingly.
 */
Void BoCA::ChooserAlbums::OnApplicationModifyTrack(const Track &track)
{
	for (Int i = 0; i < tracks.Length(); i++)
	{
		if (tracks.GetNthReference(i).GetTrackID() == track.GetTrackID())
		{
			tracks.GetNthReference(i) = track;

			UpdateAlbumList();

			break;
		}
	}
}

/* Called when a track is removed from the application joblist.
 * ----
 * Removes our corresponding entry from tracks and list_tracks.
 */
Void BoCA::ChooserAlbums::OnApplicationRemoveTrack(const Track &track)
{
	for (Int i = 0; i < tracks.Length(); i++)
	{
		if (tracks.GetNthReference(i).GetTrackID() == track.GetTrackID())
		{
			tracks.RemoveNth(i);

			break;
		}
	}

	UpdateAlbumList();

	if (IsActiveChooser() && (list_albums->GetSelectedEntry() == NIL || list_albums->Length() == 0)) onSelectNone.Emit();
}

/* Called when a track is selected in the application joblist.
 * ----
 * Finds and selects the corresponding entry in our album list.
 */
Void BoCA::ChooserAlbums::OnApplicationSelectTrack(const Track &track)
{
	for (Int i = 0; i < list_albums->Length(); i++)
	{
		if (IsAlbumIdentical(albums.Get(list_albums->GetNthEntry(i)->GetHandle()), track))
		{
			if (list_albums->GetSelectedEntryNumber() != i) list_albums->SelectNthEntry(i);

			break;
		}
	}
}

/* Called when all tracks are removed from the application joblist at once.
 * ----
 * Clears tracks and list_tracks.
 */
Void BoCA::ChooserAlbums::OnApplicationRemoveAllTracks()
{
	tracks.RemoveAll();

	if (IsActiveChooser()) onSelectNone.Emit();

	UpdateAlbumList();
}

/* Called when the currently selected entry needs to be selected again.
 * ----
 */
Void BoCA::ChooserAlbums::ReselectEntry()
{
	if (list_albums->GetSelectedEntry() == NIL) return;

	OnSelectAlbum();
}

/* Checks if the album of two tracks is the same.
 * ----
 */
Bool BoCA::ChooserAlbums::IsAlbumIdentical(const Track &track1, const Track &track2)
{
	const Info	&info1 = track1.GetInfo();
	const Info	&info2 = track2.GetInfo();

	if (info1.artist  == info2.artist &&
	    info1.album   == info2.album) return True;

	return False;
}

Void BoCA::ChooserAlbums::UpdateAlbumList()
{
	if (dontUpdateAlbumList) return;

	Array<Track>	 actualAlbums;

	foreach (const Track &track, tracks)
	{
		Bool	 found = False;

		foreach (const Track &album, albums)
		{
			if (IsAlbumIdentical(album, track)) { found = True; break; }
		}

		if (!found)
		{
			Track		 album;
			Info		&albumInfo = album.GetInfo();
			const Info	&trackInfo = track.GetInfo();

			albumInfo.artist	= trackInfo.artist;
			albumInfo.album		= trackInfo.album;
			albumInfo.genre		= trackInfo.genre;
			albumInfo.year		= trackInfo.year;
			albumInfo.comment	= trackInfo.comment;

			albumInfo.numTracks	= trackInfo.numTracks;

			albumInfo.disc		= trackInfo.disc;
			albumInfo.numDiscs	= trackInfo.numDiscs;

			const Info	&info = album.GetInfo();
			String		 jlEntry = String(info.artist.Length() > 0 ? info.artist : I18n::Get()->TranslateString("unknown artist")).Append(" - ").Append(info.album.Length() > 0 ? info.album : I18n::Get()->TranslateString("unknown album")).Append("\t");

			albums.Add(album, list_albums->AddEntry(jlEntry)->GetHandle());
		}
	}

	for (Int i = 0; i < albums.Length(); i++)
	{
		const Track	&album = albums.GetNth(i);
		Bool		 found = False;

		foreach (const Track &track, tracks)
		{
			if (IsAlbumIdentical(album, track)) { found = True; break; }
		}

		if (!found)
		{
			albums.RemoveNth(i);
			list_albums->Remove(list_albums->GetNthEntry(i));

			i--;
		}
	}
}
