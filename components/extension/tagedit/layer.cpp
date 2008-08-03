 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "layer.h"
#include "basic/layer_tag_basic.h"
#include "advanced/layer_tag_advanced.h"

BoCA::LayerTags::LayerTags() : Layer("Tags")
{
	text_tracks	= new Text("List of tracks:", Point(7, 5));
	list_tracks	= new ListBox(Point(7, 24), Size(100, 150));
	list_tracks->AddTab(I18n::Get()->TranslateString("Title"));
	list_tracks->AddTab(I18n::Get()->TranslateString("Track"), 50, OR_RIGHT);
	list_tracks->AddTab(I18n::Get()->TranslateString("Length"), 80, OR_RIGHT);
	list_tracks->AddTab(I18n::Get()->TranslateString("Size"), 80, OR_RIGHT);

	tab_mode	= new TabWidget(Point(7, 182), Size(300, 200));

	layer_basic	= new LayerTagBasic();
	layer_advanced	= new LayerTagAdvanced();

	tab_mode->Add(layer_basic);
	tab_mode->Add(layer_advanced);

	Add(text_tracks);
	Add(list_tracks);

	Add(tab_mode);

	onChangeSize.Connect(&LayerTags::OnChangeSize, this);

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
	DeleteObject(layer_advanced);
}

Void BoCA::LayerTags::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	list_tracks->SetWidth(clientSize.cx - 15);
	tab_mode->SetSize(Size(clientSize.cx - 15, clientSize.cy - 190));
}

Void BoCA::LayerTags::OnApplicationAddTrack(const Track &track)
{
	String	 jlEntry;

	if (track.artist == NIL && track.title == NIL)	jlEntry = String(track.origFilename).Append("\t");
	else						jlEntry = String(track.artist.Length() > 0 ? track.artist : I18n::Get()->TranslateString("unknown artist")).Append(" - ").Append(track.title.Length() > 0 ? track.title : I18n::Get()->TranslateString("unknown title")).Append("\t");

	jlEntry.Append(track.track > 0 ? (track.track < 10 ? String("0").Append(String::FromInt(track.track)) : String::FromInt(track.track)) : String("")).Append("\t").Append(track.lengthString).Append("\t").Append(track.fileSizeString);

	tracks.Add(&track, list_tracks->AddEntry(jlEntry)->GetHandle());
}

Void BoCA::LayerTags::OnApplicationModifyTrack(const Track &track)
{
	String	 jlEntry;

	if (track.artist == NIL && track.title == NIL)	jlEntry = String(track.origFilename).Append("\t");
	else						jlEntry = String(track.artist.Length() > 0 ? track.artist : I18n::Get()->TranslateString("unknown artist")).Append(" - ").Append(track.title.Length() > 0 ? track.title : I18n::Get()->TranslateString("unknown title")).Append("\t");

	jlEntry.Append(track.track > 0 ? (track.track < 10 ? String("0").Append(String::FromInt(track.track)) : String::FromInt(track.track)) : String("")).Append("\t").Append(track.lengthString).Append("\t").Append(track.fileSizeString);

	for (Int i = 0; i < list_tracks->Length(); i++)
	{
		if (tracks.Get(list_tracks->GetNthEntry(i)->GetHandle())->GetTrackID() == track.GetTrackID())
		{
			list_tracks->GetNthEntry(i)->SetText(jlEntry);

			break;
		}
	}
}

Void BoCA::LayerTags::OnApplicationRemoveTrack(const Track &track)
{
	for (Int i = 0; i < list_tracks->Length(); i++)
	{
		if (tracks.Get(list_tracks->GetNthEntry(i)->GetHandle())->GetTrackID() == track.GetTrackID())
		{
			tracks.Remove(list_tracks->GetNthEntry(i)->GetHandle());

			list_tracks->Remove(list_tracks->GetNthEntry(i));

			break;
		}
	}
}

Void BoCA::LayerTags::OnApplicationSelectTrack(const Track &track)
{
	for (Int i = 0; i < list_tracks->Length(); i++)
	{
		if (tracks.Get(list_tracks->GetNthEntry(i)->GetHandle())->GetTrackID() == track.GetTrackID())
		{
			list_tracks->SelectNthEntry(i);

			break;
		}
	}
}
