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

#include "chooser/chooser_tracks.h"
#include "chooser/chooser_albums.h"
#include "chooser/chooser_files.h"

BoCA::LayerTags::LayerTags() : Layer("Tags")
{
	tab_mode	= new TabWidget(Point(7, 7), Size(100, 150));

	choosers.Add(new ChooserTracks());
	choosers.Add(new ChooserAlbums());
	choosers.Add(new ChooserFiles());

	foreach (Chooser *chooser, choosers)
	{
		chooser->onSelectTrack.Connect(&onSelectTrack);
		chooser->onSelectAlbum.Connect(&onSelectAlbum);
		chooser->onSelectNone.Connect(&onSelectNone);

		tab_mode->Add(chooser);
	}

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

	tab_editor->Add(layer_advanced);

	Add(tab_mode);
	Add(tab_editor);

	onChangeSize.Connect(&LayerTags::OnChangeSize, this);

	onSelectTrack.Connect(&LayerTagBasic::OnSelectTrack, layer_basic);
	onSelectNone.Connect(&LayerTagBasic::OnSelectNone, layer_basic);

	onSelectTrack.Connect(&LayerTagDetails::OnSelectTrack, layer_details);
	onSelectNone.Connect(&LayerTagDetails::OnSelectNone, layer_details);

	onSelectTrack.Connect(&LayerTagOther::OnSelectTrack, layer_other);
	onSelectNone.Connect(&LayerTagOther::OnSelectNone, layer_other);

	onSelectTrack.Connect(&LayerTagAdvanced::OnSelectTrack, layer_advanced);
	onSelectNone.Connect(&LayerTagAdvanced::OnSelectNone, layer_advanced);
}

BoCA::LayerTags::~LayerTags()
{
	DeleteObject(tab_mode);

	foreach (Chooser *chooser, choosers)
	{
		DeleteObject(chooser);
	}

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

	tab_editor->SetWidth(clientSize.cx - 15);
}

/* Called when a list entry is modified.
 * ----
 * Finds the corresponding track and updates it accordingly.
 */
Void BoCA::LayerTags::OnModifyTrack(const Track &track)
{
	JobList::Get()->onComponentModifyTrack.Emit(track);

	foreach (Chooser *chooser, choosers)
	{
		chooser->OnModifyTrack(track);
	}
}
