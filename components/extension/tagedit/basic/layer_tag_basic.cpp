 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "layer_tag_basic.h"

using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

BoCA::LayerTagBasic::LayerTagBasic() : Layer("Basic")
{
	I18n	*i18n = I18n::Get();

	group_info		= new GroupBox("Title info", Point(7, 10), Size(400, 240));

	text_artist		= new Text(String(i18n->TranslateString("Artist")).Append(":"), Point(7, 13));
	text_title		= new Text(String(i18n->TranslateString("Title")).Append(":"), text_artist->GetPosition() + Point(0, 25));
	text_album		= new Text(String(i18n->TranslateString("Album")).Append(":"), text_title->GetPosition() + Point(0, 25));
	text_comment		= new Text(String(i18n->TranslateString("Comment")).Append(":"), text_album->GetPosition() + Point(0, 25));

	Int	 maxTextSize = Math::Max(Math::Max(text_artist->textSize.cx, text_title->textSize.cx), Math::Max(text_album->textSize.cx, text_comment->textSize.cx));

	edit_artist		= new EditBox("", text_artist->GetPosition() + Point(maxTextSize + 8, -3), Size(300, 0));
	edit_artist->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_title		= new EditBox("", text_title->GetPosition() + Point(maxTextSize + 8, -3), Size(300, 0));
	edit_title->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_album		= new EditBox("", text_album->GetPosition() + Point(maxTextSize + 8, -3), Size(300, 0));
	edit_album->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_comment		= new MultiEdit("", text_comment->GetPosition() + Point(maxTextSize + 8, -3), Size(300, 50));
	edit_comment->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	group_info->Add(text_artist);
	group_info->Add(edit_artist);
	group_info->Add(text_title);
	group_info->Add(edit_title);
	group_info->Add(text_album);
	group_info->Add(edit_album);
	group_info->Add(text_comment);
	group_info->Add(edit_comment);

	group_cover		= new GroupBox("Cover art", Point(415, 10), Size(400, 240));

	image_covers		= new ImageBox(Point(10, 10), Size(300, 80));
	image_covers->onSelectEntry.Connect(&LayerTagBasic::SelectCover, this);

	button_cover_load	= new Button("Load", NIL, Point(89, 9), Size(0, 0));
	button_cover_load->SetOrientation(OR_UPPERRIGHT);
	button_cover_load->onAction.Connect(&LayerTagBasic::LoadCover, this);

	button_cover_remove	= new Button("Remove", NIL, button_cover_load->GetPosition() + Point(0, 30), Size(0, 0));
	button_cover_remove->SetOrientation(OR_UPPERRIGHT);
	button_cover_remove->onAction.Connect(&LayerTagBasic::RemoveCover, this);

	group_cover->Add(image_covers);
	group_cover->Add(button_cover_load);
	group_cover->Add(button_cover_remove);

	Add(group_info);
	Add(group_cover);

	onChangeSize.Connect(&LayerTagBasic::OnChangeSize, this);

	/* Initially deactivate all input fields.
	 */
	OnSelectNone();
}

BoCA::LayerTagBasic::~LayerTagBasic()
{
	DeleteObject(group_info);
	DeleteObject(text_artist);
	DeleteObject(edit_artist);
	DeleteObject(text_title);
	DeleteObject(edit_title);
	DeleteObject(text_album);
	DeleteObject(edit_album);
	DeleteObject(text_comment);
	DeleteObject(edit_comment);

	DeleteObject(group_cover);
	DeleteObject(image_covers);
	DeleteObject(button_cover_load);
	DeleteObject(button_cover_remove);
}

/* Called when layer size changes.
 * ----
 */
Void BoCA::LayerTagBasic::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	group_info->SetWidth((clientSize.cx - 23) / 2);

	edit_artist->SetWidth(group_info->GetWidth() - 81);
	edit_title->SetWidth(group_info->GetWidth() - 81);
	edit_album->SetWidth(group_info->GetWidth() - 81);
	edit_comment->SetWidth(group_info->GetWidth() - 81);

	group_cover->SetX((clientSize.cx / 2) + 4);
	group_cover->SetWidth((clientSize.cx - 24) / 2 + (clientSize.cx % 2));

	image_covers->SetWidth(group_cover->GetWidth() - 107);
}

Void BoCA::LayerTagBasic::LoadCoverImages()
{
	foreach (const Picture &cover, track.pictures)
	{
		ImageEntry	*entry = new ImageEntry(cover.GetBitmap(), Size(70, 70));

		image_covers->Add(entry);
	}
}

Void BoCA::LayerTagBasic::FreeCoverImages()
{
	while (image_covers->Length() > 0)
	{
		ListEntry	*entry = image_covers->GetNthEntry(image_covers->Length() - 1);

		image_covers->Remove(entry);

		delete entry;
	}
}

Void BoCA::LayerTagBasic::LoadCover()
{
	FileSelection	*dialog = new FileSelection();

	dialog->SetParentWindow(GetContainerWindow());

	dialog->AddFilter(I18n::Get()->TranslateString("Image files"), "*.jpg; *.jpeg; *.png");

	dialog->AddFilter(I18n::Get()->TranslateString("JPEG images"), "*.jpg; *.jpeg");
	dialog->AddFilter(I18n::Get()->TranslateString("PNG images"), "*.png");

	dialog->AddFilter(I18n::Get()->TranslateString("All Files"), "*.*");

	if (dialog->ShowDialog() == Success())
	{
		String		 file = dialog->GetFileName();
		ImageEntry	*entry = new ImageEntry(ImageLoader::Load(file), Size(70, 70));

		image_covers->Add(entry);

		Picture		 picture;

		InStream	 in(STREAM_FILE, file, IS_READONLY);

		picture.data.Resize(in.Size());

		in.InputData(picture.data, picture.data.Size());

		if	(picture.data[0] == 0xFF && picture.data[1] == 0xD8) picture.mime = "image/jpeg";
		else if (picture.data[0] == 0x89 && picture.data[1] == 0x50 &&
			 picture.data[2] == 0x4E && picture.data[3] == 0x47 &&
			 picture.data[4] == 0x0D && picture.data[5] == 0x0A &&
			 picture.data[6] == 0x1A && picture.data[7] == 0x0A) picture.mime = "image/png";

		picture.type = 0;

		track.pictures.Add(picture);

		onModifyTrack.Emit(track);
	}

	delete dialog;
}

Void BoCA::LayerTagBasic::RemoveCover()
{
	Int		 index = image_covers->GetSelectedEntryNumber();
	ListEntry	*entry = image_covers->GetNthEntry(index);

	image_covers->Remove(entry);

	delete entry;

	track.pictures.RemoveNth(index);

	onModifyTrack.Emit(track);
}

Void BoCA::LayerTagBasic::SelectCover(ListEntry *entry)
{
	Int	 index = 0;

	for (Int i = 0; i < image_covers->Length(); i++)
	{
		if (image_covers->GetNthEntry(i) == entry) { index = i; break; }
	}
}

/* Called when a track is selected from the list.
 * ----
 * Copy new info to track and update input fields.
 */
Void BoCA::LayerTagBasic::OnSelectTrack(const Track &nTrack)
{
	if (&nTrack == &track) return;

	FreeCoverImages();

	track = nTrack;

	group_info->Activate();
	group_cover->Activate();

	edit_artist->SetText(track.artist);
	edit_title->SetText(track.title);
	edit_album->SetText(track.album);
	edit_comment->SetText(track.comment);

	LoadCoverImages();
}

/* Called when the last track is removed from the list.
 * ----
 * Clear and deactivate all input fields.
 */
Void BoCA::LayerTagBasic::OnSelectNone()
{
	edit_artist->SetText("");
	edit_title->SetText("");
	edit_album->SetText("");
	edit_comment->SetText("");

	group_info->Deactivate();
	group_cover->Deactivate();

	FreeCoverImages();
}

/* Called when a track is modified.
 * ----
 * Write updated info back to track and emit onModifyTrack.
 */
Void BoCA::LayerTagBasic::OnModifyTrack()
{
	track.artist = edit_artist->GetText();
	track.title = edit_title->GetText();
	track.album = edit_album->GetText();
	track.comment = edit_comment->GetText();

	onModifyTrack.Emit(track);
}
