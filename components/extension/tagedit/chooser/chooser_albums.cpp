 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
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
	list_albums	= new ListBox(Point(7, 7), Size(100, 150));
	list_albums->onSelectEntry.Connect(&ChooserAlbums::OnSelectAlbum, this);
	list_albums->AddTab(I18n::Get()->TranslateString("Title"));

	Add(list_albums);

	onChangeSize.Connect(&ChooserAlbums::OnChangeSize, this);

	JobList::Get()->onApplicationAddTrack.Connect(&ChooserAlbums::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Connect(&ChooserAlbums::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Connect(&ChooserAlbums::OnApplicationRemoveTrack, this);

	JobList::Get()->onApplicationRemoveAllTracks.Connect(&ChooserAlbums::OnApplicationRemoveAllTracks, this);
}

BoCA::ChooserAlbums::~ChooserAlbums()
{
	JobList::Get()->onApplicationAddTrack.Disconnect(&ChooserAlbums::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Disconnect(&ChooserAlbums::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Disconnect(&ChooserAlbums::OnApplicationRemoveTrack, this);

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

	onSelectAlbum.Emit(album);
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

			break;
		}
	}

	UpdateAlbumList();
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

	if (tracks.Length() == 0) onSelectNone.Emit();

	UpdateAlbumList();
}

/* Called when all tracks are removed from the application joblist at once.
 * ----
 * Clears tracks and list_tracks.
 */
Void BoCA::ChooserAlbums::OnApplicationRemoveAllTracks()
{
	tracks.RemoveAll();

	onSelectNone.Emit();

	UpdateAlbumList();
}

Void BoCA::ChooserAlbums::UpdateAlbumList()
{
	Array<Track>	 actualAlbums;

	foreach (const Track &track, tracks)
	{
		Bool	 found = False;

		foreach (const Track &album, albums)
		{
			if (album.GetInfo().artist == track.GetInfo().artist &&
			    album.GetInfo().album  == track.GetInfo().album)
			{
				found = True;

				break;
			}
		}

		if (!found)
		{
			Track		 album;

			album.GetInfo().artist = track.GetInfo().artist;
			album.GetInfo().album  = track.GetInfo().album;

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
			if (album.GetInfo().artist == track.GetInfo().artist &&
			    album.GetInfo().album  == track.GetInfo().album)
			{
				found = True;

				break;
			}
		}

		if (!found)
		{
			albums.RemoveNth(i);
			list_albums->Remove(list_albums->GetNthEntry(i));

			i--;
		}
	}
}
