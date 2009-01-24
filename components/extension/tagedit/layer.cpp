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
	text_tracks	= new Text("List of tracks:", Point(7, 5));
	list_tracks	= new ListBox(Point(7, 24), Size(100, 150));
	list_tracks->onSelectEntry.Connect(&LayerTags::OnSelectTrack, this);
	list_tracks->AddTab(I18n::Get()->TranslateString("Title"));
	list_tracks->AddTab(I18n::Get()->TranslateString("Track"), 50, OR_RIGHT);
	list_tracks->AddTab(I18n::Get()->TranslateString("Length"), 80, OR_RIGHT);
	list_tracks->AddTab(I18n::Get()->TranslateString("Size"), 80, OR_RIGHT);

	tab_mode	= new TabWidget(Point(7, 288), Size(300, 280));
	tab_mode->SetOrientation(OR_LOWERLEFT);

	layer_basic	= new LayerTagBasic();
	layer_basic->onModifyTrack.Connect(&LayerTags::OnModifyTrack, this);

	layer_details	= new LayerTagDetails();
	layer_details->onModifyTrack.Connect(&LayerTags::OnModifyTrack, this);

	tab_mode->Add(layer_basic);
	tab_mode->Add(layer_details);

	layer_advanced	= new LayerTagAdvanced();

	tab_mode->Add(layer_advanced);

	Add(text_tracks);
	Add(list_tracks);

	Add(tab_mode);

	onChangeSize.Connect(&LayerTags::OnChangeSize, this);

	onSelectTrack.Connect(&LayerTagBasic::OnSelectTrack, layer_basic);
	onSelectNone.Connect(&LayerTagBasic::OnSelectNone, layer_basic);

	onSelectTrack.Connect(&LayerTagDetails::OnSelectTrack, layer_details);
	onSelectNone.Connect(&LayerTagDetails::OnSelectNone, layer_details);

	onSelectTrack.Connect(&LayerTagAdvanced::OnSelectTrack, layer_advanced);
	onSelectNone.Connect(&LayerTagAdvanced::OnSelectNone, layer_advanced);

	JobList::Get()->onApplicationAddTrack.Connect(&LayerTags::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Connect(&LayerTags::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Connect(&LayerTags::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationSelectTrack.Connect(&LayerTags::OnApplicationSelectTrack, this);
}

BoCA::LayerTags::~LayerTags()
{
	JobList::Get()->onApplicationAddTrack.Disconnect(&LayerTags::OnApplicationAddTrack, this);
	JobList::Get()->onApplicationModifyTrack.Disconnect(&LayerTags::OnApplicationModifyTrack, this);
	JobList::Get()->onApplicationRemoveTrack.Disconnect(&LayerTags::OnApplicationRemoveTrack, this);
	JobList::Get()->onApplicationSelectTrack.Disconnect(&LayerTags::OnApplicationSelectTrack, this);

	DeleteObject(text_tracks);
	DeleteObject(list_tracks);

	DeleteObject(tab_mode);

	DeleteObject(layer_basic);
	DeleteObject(layer_details);

	DeleteObject(layer_advanced);
}

/* Called when component canvas size changes.
 * ----
 */
Void BoCA::LayerTags::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	list_tracks->SetSize(Size(clientSize.cx - 15, clientSize.cy - 320));
	tab_mode->SetWidth(clientSize.cx - 15);
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

	if (list_tracks->Length() == 0) onSelectNone.Emit();
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
