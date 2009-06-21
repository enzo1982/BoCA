 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "chooser_tracks.h"

BoCA::ChooserTracks::ChooserTracks() : Chooser("Tracks")
{
	list_tracks	= new ListBox(Point(7, 7), Size(100, 150));
	list_tracks->onSelectEntry.Connect(&ChooserTracks::OnSelectTrack, this);
	list_tracks->AddTab(I18n::Get()->TranslateString("Title"));
	list_tracks->AddTab(I18n::Get()->TranslateString("Track"), 50, OR_RIGHT);
	list_tracks->AddTab(I18n::Get()->TranslateString("Length"), 80, OR_RIGHT);
	list_tracks->AddTab(I18n::Get()->TranslateString("Size"), 80, OR_RIGHT);

	Add(list_tracks);

	onChangeSize.Connect(&ChooserTracks::OnChangeSize, this);

	JobList::Get()->onApplicationAddTrack.Connect(&ChooserTracks::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Connect(&ChooserTracks::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Connect(&ChooserTracks::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationSelectTrack.Connect(&ChooserTracks::OnApplicationSelectTrack, this);

	JobList::Get()->onApplicationRemoveAllTracks.Connect(&ChooserTracks::OnApplicationRemoveAllTracks, this);
}

BoCA::ChooserTracks::~ChooserTracks()
{
	JobList::Get()->onApplicationAddTrack.Disconnect(&ChooserTracks::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Disconnect(&ChooserTracks::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Disconnect(&ChooserTracks::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationSelectTrack.Disconnect(&ChooserTracks::OnApplicationSelectTrack, this);

	JobList::Get()->onApplicationRemoveAllTracks.Disconnect(&ChooserTracks::OnApplicationRemoveAllTracks, this);

	DeleteObject(list_tracks);
}

/* Called when component canvas size changes.
 * ----
 */
Void BoCA::ChooserTracks::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	list_tracks->SetSize(Size(clientSize.cx - 15, clientSize.cy - 15));
}

/* Called when a list entry is selected.
 * ----
 * Finds the corresponding track and emits onSelectTrack.
 */
Void BoCA::ChooserTracks::OnSelectTrack()
{
	const Track	&track = tracks.GetNth(list_tracks->GetSelectedEntryNumber());

	onSelectTrack.Emit(track);

	JobList::Get()->onComponentSelectTrack.Emit(track);
}

/* Called when a track is added to the application joblist.
 * ----
 * Adds entries to tracks and list_tracks.
 */
Void BoCA::ChooserTracks::OnApplicationAddTrack(const Track &track)
{
	const Info	&info = track.GetInfo();
	String		 jlEntry;

	if (info.artist == NIL && info.title == NIL) jlEntry = String(track.origFilename).Append("\t");
	else					     jlEntry = String(info.artist.Length() > 0 ? info.artist : I18n::Get()->TranslateString("unknown artist")).Append(" - ").Append(info.title.Length() > 0 ? info.title : I18n::Get()->TranslateString("unknown title")).Append("\t");

	jlEntry.Append(info.track > 0 ? (info.track < 10 ? String("0").Append(String::FromInt(info.track)) : String::FromInt(info.track)) : String("")).Append("\t").Append(track.lengthString).Append("\t").Append(track.fileSizeString);

	tracks.Add(track, list_tracks->AddEntry(jlEntry)->GetHandle());
}

/* Called when a track is modified by the application.
 * ----
 * Modifies our corresponding entry accordingly.
 */
Void BoCA::ChooserTracks::OnApplicationModifyTrack(const Track &track)
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
}

/* Called when a track is removed from the application joblist.
 * ----
 * Removes our corresponding entry from tracks and list_tracks.
 */
Void BoCA::ChooserTracks::OnApplicationRemoveTrack(const Track &track)
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
}

/* Called when a track is selected in the application joblist.
 * ----
 * Finds and selects the corresponding entry in our track list.
 */
Void BoCA::ChooserTracks::OnApplicationSelectTrack(const Track &track)
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
Void BoCA::ChooserTracks::OnApplicationRemoveAllTracks()
{
	tracks.RemoveAll();

	list_tracks->RemoveAllEntries();

	onSelectNone.Emit();
}
