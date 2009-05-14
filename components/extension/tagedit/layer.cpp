 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "layer.h"

BoCA::LayerTags::LayerTags() : Layer("Tags")
{
	tab_mode	= new TabWidget(Point(7, 7), Size(100, 150));

	layer_tracks	= new Layer("Tracks");
	layer_albums	= new Layer("Albums");
	layer_files	= new Layer("Files");

	list_tracks	= new ListBox(Point(7, 7), Size(100, 150));
	list_tracks->onSelectEntry.Connect(&LayerTags::OnSelectTrack, this);
	list_tracks->AddTab(I18n::Get()->TranslateString("Title"));
	list_tracks->AddTab(I18n::Get()->TranslateString("Track"), 50, OR_RIGHT);
	list_tracks->AddTab(I18n::Get()->TranslateString("Length"), 80, OR_RIGHT);
	list_tracks->AddTab(I18n::Get()->TranslateString("Size"), 80, OR_RIGHT);

	list_albums	= new ListBox(Point(7, 7), Size(100, 150));
	list_albums->onSelectEntry.Connect(&LayerTags::OnSelectAlbum, this);
	list_albums->AddTab(I18n::Get()->TranslateString("Title"));

	list_files	= new ListBox(Point(7, 7), Size(100, 150));
	list_files->AddTab(I18n::Get()->TranslateString("File"));

	layer_tracks->Add(list_tracks);
	layer_albums->Add(list_albums);
	layer_files->Add(list_files);

	tab_mode->Add(layer_tracks);
	tab_mode->Add(layer_albums);
	tab_mode->Add(layer_files);

	tab_editor	= new TabWidget(Point(7, 226), Size(300, 218));
	tab_editor->SetOrientation(OR_LOWERLEFT);

	layer_basic	= new LayerTagBasic();
	layer_basic->onModifyTrack.Connect(&LayerTags::OnModifyTrack, this);

	layer_details	= new LayerTagDetails();
	layer_details->onModifyTrack.Connect(&LayerTags::OnModifyTrack, this);

	layer_other	= new LayerTagOther();
	layer_other->onModifyTrack.Connect(&LayerTags::OnModifyTrack, this);

	tab_editor->Add(layer_basic);
	tab_editor->Add(layer_details);
	tab_editor->Add(layer_other);

	layer_advanced	= new LayerTagAdvanced();

//	tab_editor->Add(layer_advanced);

	Add(tab_mode);
	Add(tab_editor);

	onChangeSize.Connect(&LayerTags::OnChangeSize, this);

	onSelectTrack.Connect(&LayerTagBasic::OnSelectTrack, layer_basic);
	onSelectNone.Connect(&LayerTagBasic::OnSelectNone, layer_basic);

	onSelectTrack.Connect(&LayerTagDetails::OnSelectTrack, layer_details);
	onSelectNone.Connect(&LayerTagDetails::OnSelectNone, layer_details);

	onSelectTrack.Connect(&LayerTagOther::OnSelectTrack, layer_other);
	onSelectNone.Connect(&LayerTagOther::OnSelectNone, layer_other);

//	onSelectTrack.Connect(&LayerTagAdvanced::OnSelectTrack, layer_advanced);
//	onSelectNone.Connect(&LayerTagAdvanced::OnSelectNone, layer_advanced);

	JobList::Get()->onApplicationAddTrack.Connect(&LayerTags::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Connect(&LayerTags::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Connect(&LayerTags::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationSelectTrack.Connect(&LayerTags::OnApplicationSelectTrack, this);

	JobList::Get()->onApplicationRemoveAllTracks.Connect(&LayerTags::OnApplicationRemoveAllTracks, this);
}

BoCA::LayerTags::~LayerTags()
{
	JobList::Get()->onApplicationAddTrack.Disconnect(&LayerTags::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Disconnect(&LayerTags::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Disconnect(&LayerTags::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationSelectTrack.Disconnect(&LayerTags::OnApplicationSelectTrack, this);

	JobList::Get()->onApplicationRemoveAllTracks.Disconnect(&LayerTags::OnApplicationRemoveAllTracks, this);

	DeleteObject(tab_mode);

	DeleteObject(layer_tracks);
	DeleteObject(layer_albums);
	DeleteObject(layer_files);

	DeleteObject(list_tracks);
	DeleteObject(list_albums);
	DeleteObject(list_files);

	DeleteObject(tab_editor);

	DeleteObject(layer_basic);
	DeleteObject(layer_details);
	DeleteObject(layer_other);

	DeleteObject(layer_advanced);
}

/* Called when component canvas size changes.
 * ----
 */
Void BoCA::LayerTags::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	tab_mode->SetSize(Size(clientSize.cx - 15, clientSize.cy - 241));

	list_tracks->SetSize(Size(clientSize.cx - 33, clientSize.cy - 278));
	list_albums->SetSize(Size(clientSize.cx - 33, clientSize.cy - 278));
	list_files->SetSize(Size(clientSize.cx - 33, clientSize.cy - 278));

	tab_editor->SetWidth(clientSize.cx - 15);
}

/* Called when a list entry is selected.
 * ----
 * Finds the corresponding track and emits onSelectTrack.
 */
Void BoCA::LayerTags::OnSelectTrack()
{
	const Track	&track = tracks.GetNth(list_tracks->GetSelectedEntryNumber());

	onSelectTrack.Emit(track);

	JobList::Get()->onComponentSelectTrack.Emit(track);
}

/* Called when a list entry is modified.
 * ----
 * Finds the corresponding track and updates it accordingly.
 */
Void BoCA::LayerTags::OnModifyTrack(const Track &track)
{
	JobList::Get()->onComponentModifyTrack.Emit(track);
}

/* Called when an album entry is selected.
 * ----
 * Finds the corresponding album and emits onSelectAlbum.
 */
Void BoCA::LayerTags::OnSelectAlbum()
{
	const Track	&album = albums.GetNth(list_albums->GetSelectedEntryNumber());

	onSelectAlbum.Emit(album);
}

/* Called when a track is added to the application joblist.
 * ----
 * Adds entries to tracks and list_tracks.
 */
Void BoCA::LayerTags::OnApplicationAddTrack(const Track &track)
{
	const Info	&info = track.GetInfo();
	String		 jlEntry;

	if (info.artist == NIL && info.title == NIL) jlEntry = String(track.origFilename).Append("\t");
	else					     jlEntry = String(info.artist.Length() > 0 ? info.artist : I18n::Get()->TranslateString("unknown artist")).Append(" - ").Append(info.title.Length() > 0 ? info.title : I18n::Get()->TranslateString("unknown title")).Append("\t");

	jlEntry.Append(info.track > 0 ? (info.track < 10 ? String("0").Append(String::FromInt(info.track)) : String::FromInt(info.track)) : String("")).Append("\t").Append(track.lengthString).Append("\t").Append(track.fileSizeString);

	tracks.Add(track, list_tracks->AddEntry(jlEntry)->GetHandle());

	UpdateAlbumList();
}

/* Called when a track is modified by the application.
 * ----
 * Modifies our corresponding entry accordingly.
 */
Void BoCA::LayerTags::OnApplicationModifyTrack(const Track &track)
{
	const Info	&info = track.GetInfo();
	String		 jlEntry;

	if (info.artist == NIL && info.title == NIL) jlEntry = String(track.origFilename).Append("\t");
	else					     jlEntry = String(info.artist.Length() > 0 ? info.artist : I18n::Get()->TranslateString("unknown artist")).Append(" - ").Append(info.title.Length() > 0 ? info.title : I18n::Get()->TranslateString("unknown title")).Append("\t");

	jlEntry.Append(info.track > 0 ? (info.track < 10 ? String("0").Append(String::FromInt(info.track)) : String::FromInt(info.track)) : String("")).Append("\t").Append(track.lengthString).Append("\t").Append(track.fileSizeString);

	for (Int i = 0; i < list_tracks->Length(); i++)
	{
		if (tracks.Get(list_tracks->GetNthEntry(i)->GetHandle()).GetTrackID() == track.GetTrackID())
		{
			list_tracks->GetNthEntry(i)->SetText(jlEntry);

			tracks.GetReference(list_tracks->GetNthEntry(i)->GetHandle()) = track;

			/* Emit onSelectTrack to let edit layer update its input fields.
			 */
			if (list_tracks->GetSelectedEntryNumber() == i) onSelectTrack.Emit(track);

			break;
		}
	}

	UpdateAlbumList();
}

/* Called when a track is removed from the application joblist.
 * ----
 * Removes our corresponding entry from tracks and list_tracks.
 */
Void BoCA::LayerTags::OnApplicationRemoveTrack(const Track &track)
{
	for (Int i = 0; i < list_tracks->Length(); i++)
	{
		if (tracks.Get(list_tracks->GetNthEntry(i)->GetHandle()).GetTrackID() == track.GetTrackID())
		{
			tracks.Remove(list_tracks->GetNthEntry(i)->GetHandle());

			list_tracks->Remove(list_tracks->GetNthEntry(i));

			break;
		}
	}

	if (list_tracks->GetSelectedEntry() == NIL || list_tracks->Length() == 0) onSelectNone.Emit();

	UpdateAlbumList();
}

/* Called when a track is selected in the application joblist.
 * ----
 * Finds and selects the corresponding entry in our track list.
 */
Void BoCA::LayerTags::OnApplicationSelectTrack(const Track &track)
{
	for (Int i = 0; i < list_tracks->Length(); i++)
	{
		if (tracks.Get(list_tracks->GetNthEntry(i)->GetHandle()).GetTrackID() == track.GetTrackID())
		{
			if (list_tracks->GetSelectedEntryNumber() != i) list_tracks->SelectNthEntry(i);

			break;
		}
	}
}

/* Called when all tracks are removed from the application joblist at once.
 * ----
 * Clears tracks and list_tracks.
 */
Void BoCA::LayerTags::OnApplicationRemoveAllTracks()
{
	tracks.RemoveAll();

	list_tracks->RemoveAllEntries();

	onSelectNone.Emit();

	UpdateAlbumList();
}

Void BoCA::LayerTags::UpdateAlbumList()
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
