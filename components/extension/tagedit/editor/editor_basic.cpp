 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "editor_basic.h"
#include "editor_basic_cover.h"

#include "../utilities.h"

using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

BoCA::LayerTagBasic::LayerTagBasic() : Editor("Info")
{
	I18n	*i18n = I18n::Get();

	group_info		= new GroupBox("Title information", Point(7, 10), Size(400, 178));

	text_artist		= new Text(String(i18n->TranslateString("Artist")).Append(":"), Point(9, 13));
	text_title		= new Text(String(i18n->TranslateString("Title")).Append(":"), text_artist->GetPosition() + Point(0, 27));
	text_album		= new Text(String(i18n->TranslateString("Album")).Append(":"), text_title->GetPosition() + Point(0, 27));
	text_genre		= new Text(String(i18n->TranslateString("Genre")).Append(":"), text_album->GetPosition() + Point(0, 27));
	text_comment		= new Text(String(i18n->TranslateString("Comment")).Append(":"), text_genre->GetPosition() + Point(0, 27));

	Int	 maxTextSize = Math::Max(Math::Max(Math::Max(text_artist->textSize.cx, text_genre->textSize.cx), text_title->textSize.cx), Math::Max(text_album->textSize.cx, text_comment->textSize.cx));

	edit_artist		= new EditBox(NIL, text_artist->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_artist->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_title		= new EditBox(NIL, text_title->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_title->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_album		= new EditBox(NIL, text_album->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_album->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	list_genre		= new ListBox(Point(), Size());
	TagUtilities::FillGenreList(list_genre);

	edit_genre		= new EditBox(NIL, text_genre->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_genre->SetDropDownList(list_genre);
	edit_genre->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_comment		= new MultiEdit(NIL, text_comment->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 50));
	edit_comment->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_ntracks		= new EditBox(NIL, Point(28, text_title->GetY() - 3), Size(18, 0), 2);
	edit_ntracks->SetOrientation(OR_UPPERRIGHT);
	edit_ntracks->SetFlags(EDB_NUMERIC);
	edit_ntracks->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	text_ntracks		= new Text("/", edit_ntracks->GetPosition() + Point(10, 3));
	text_ntracks->SetOrientation(OR_UPPERRIGHT);

	edit_track		= new EditBox(NIL, edit_ntracks->GetPosition() + Point(32, 0), Size(18, 0), 2);
	edit_track->SetOrientation(OR_UPPERRIGHT);
	edit_track->SetFlags(EDB_NUMERIC);
	edit_track->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	text_track		= new Text(String(i18n->TranslateString("Track")).Append(":"), edit_track->GetPosition() + Point(35, 3));
	text_track->SetOrientation(OR_UPPERRIGHT);

	edit_ndiscs		= new EditBox(NIL, Point(28, text_album->GetY() - 3), Size(18, 0), 2);
	edit_ndiscs->SetOrientation(OR_UPPERRIGHT);
	edit_ndiscs->SetFlags(EDB_NUMERIC);
	edit_ndiscs->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	text_ndiscs		= new Text("/", edit_ndiscs->GetPosition() + Point(10, 3));
	text_ndiscs->SetOrientation(OR_UPPERRIGHT);

	edit_disc		= new EditBox(NIL, edit_ndiscs->GetPosition() + Point(32, 0), Size(18, 0), 2);
	edit_disc->SetOrientation(OR_UPPERRIGHT);
	edit_disc->SetFlags(EDB_NUMERIC);
	edit_disc->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	text_disc		= new Text(String(i18n->TranslateString("Disc")).Append(":"), edit_disc->GetPosition() + Point(30, 3));
	text_disc->SetOrientation(OR_UPPERRIGHT);

	edit_year		= new EditBox(NIL, Point(60, text_genre->GetY() - 3), Size(50, 0), 4);
	edit_year->SetOrientation(OR_UPPERRIGHT);
	edit_year->SetFlags(EDB_NUMERIC);
	edit_year->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	text_year		= new Text(String(i18n->TranslateString("Year")).Append(":"), edit_year->GetPosition() + Point(30, 3));
	text_year->SetOrientation(OR_UPPERRIGHT);

	Int	 maxTextSize2 = Math::Max(Math::Max(text_track->textSize.cx, text_disc->textSize.cx), text_year->textSize.cx);

	text_track->SetX(maxTextSize2 + 68);
	text_disc->SetX(maxTextSize2 + 68);
	text_year->SetX(maxTextSize2 + 68);

	group_info->Add(text_artist);
	group_info->Add(edit_artist);
	group_info->Add(text_title);
	group_info->Add(edit_title);
	group_info->Add(text_album);
	group_info->Add(edit_album);
	group_info->Add(text_genre);
	group_info->Add(edit_genre);
	group_info->Add(text_comment);
	group_info->Add(edit_comment);
	group_info->Add(text_track);
	group_info->Add(edit_track);
	group_info->Add(text_ntracks);
	group_info->Add(edit_ntracks);
	group_info->Add(text_disc);
	group_info->Add(edit_disc);
	group_info->Add(text_ndiscs);
	group_info->Add(edit_ndiscs);
	group_info->Add(text_year);
	group_info->Add(edit_year);

	group_cover		= new GroupBox("Cover art", Point(415, 10), Size(400, 178));

	text_covers		= new Text(String(i18n->TranslateString("Covers")).Append(":"), Point(9, 13));
	text_cover_type		= new Text(String(i18n->TranslateString("Type")).Append(":"), Point(9, 94));
	text_cover_desc		= new Text(String(i18n->TranslateString("Description")).Append(":"), text_cover_type->GetPosition() + Point(0, 27));

	Int	 maxTextSize3 = Math::Max(Math::Max(text_cover_type->textSize.cx, text_covers->textSize.cx), text_cover_desc->textSize.cx);

	image_covers		= new ImageBox(text_covers->GetPosition() + Point(maxTextSize3 + 7, -3), Size(300, 73));
	image_covers->onSelectEntry.Connect(&LayerTagBasic::SelectCover, this);

	combo_cover_type	= new ComboBox(text_cover_type->GetPosition() + Point(maxTextSize3 + 7, -3), Size(300, 0));
	combo_cover_type->AddEntry("Other");
	combo_cover_type->AddEntry("32x32 pixels 'file icon' (PNG only)");
	combo_cover_type->AddEntry("Other file icon");
	combo_cover_type->AddEntry("Cover (front)");
	combo_cover_type->AddEntry("Cover (back)");
	combo_cover_type->AddEntry("Leaflet page");
	combo_cover_type->AddEntry("Media (e.g. label side of CD)");
	combo_cover_type->AddEntry("Lead artist/lead performer/soloist");
	combo_cover_type->AddEntry("Artist/performer");
	combo_cover_type->AddEntry("Conductor");
	combo_cover_type->AddEntry("Band/Orchestra");
	combo_cover_type->AddEntry("Composer");
	combo_cover_type->AddEntry("Lyricist/text writer");
	combo_cover_type->AddEntry("Recording Location");
	combo_cover_type->AddEntry("During recording");
	combo_cover_type->AddEntry("During performance");
	combo_cover_type->AddEntry("Movie/video screen capture");
	combo_cover_type->AddEntry("A bright coloured fish");
	combo_cover_type->AddEntry("Illustration");
	combo_cover_type->AddEntry("Band/artist logotype");
	combo_cover_type->AddEntry("Publisher/Studio logotype");
	combo_cover_type->onSelectEntry.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_cover_desc		= new MultiEdit(NIL, text_cover_desc->GetPosition() + Point(maxTextSize3 + 7, -3), Size(300, 50));
	edit_cover_desc->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	button_cover_add	= new Button("Add", NIL, Point(89, 9), Size(0, 0));
	button_cover_add->SetOrientation(OR_UPPERRIGHT);
	button_cover_add->onAction.Connect(&LayerTagBasic::AddCover, this);

	button_cover_remove	= new Button("Remove", NIL, button_cover_add->GetPosition() + Point(0, 28), Size(0, 0));
	button_cover_remove->SetOrientation(OR_UPPERRIGHT);
	button_cover_remove->Deactivate();
	button_cover_remove->onAction.Connect(&LayerTagBasic::RemoveCover, this);

	group_cover->Add(text_covers);
	group_cover->Add(image_covers);
	group_cover->Add(text_cover_type);
	group_cover->Add(combo_cover_type);
	group_cover->Add(text_cover_desc);
	group_cover->Add(edit_cover_desc);
	group_cover->Add(button_cover_add);
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
	DeleteObject(text_genre);
	DeleteObject(edit_genre);
	DeleteObject(list_genre);
	DeleteObject(text_comment);
	DeleteObject(edit_comment);
	DeleteObject(text_track);
	DeleteObject(edit_track);
	DeleteObject(text_ntracks);
	DeleteObject(edit_ntracks);
	DeleteObject(text_disc);
	DeleteObject(edit_disc);
	DeleteObject(text_ndiscs);
	DeleteObject(edit_ndiscs);
	DeleteObject(text_year);
	DeleteObject(edit_year);

	DeleteObject(group_cover);
	DeleteObject(text_covers);
	DeleteObject(image_covers);
	DeleteObject(text_cover_type);
	DeleteObject(combo_cover_type);
	DeleteObject(text_cover_desc);
	DeleteObject(edit_cover_desc);
	DeleteObject(button_cover_add);
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

	Int	 maxTextSize = Math::Max(Math::Max(Math::Max(text_artist->textSize.cx, text_genre->textSize.cx), text_title->textSize.cx), Math::Max(text_album->textSize.cx, text_comment->textSize.cx));
	Int	 maxTextSize2 = Math::Max(Math::Max(text_track->textSize.cx, text_disc->textSize.cx), text_year->textSize.cx);
	Int	 maxTextSize3 = Math::Max(text_cover_type->textSize.cx, text_cover_desc->textSize.cx);

	edit_artist->SetWidth(group_info->GetWidth() - 26 - maxTextSize);
	edit_title->SetWidth(group_info->GetWidth() - 91 - maxTextSize - maxTextSize2);
	edit_album->SetWidth(group_info->GetWidth() - 91 - maxTextSize - maxTextSize2);
	edit_genre->SetWidth(group_info->GetWidth() - 91 - maxTextSize - maxTextSize2);
	edit_comment->SetWidth(group_info->GetWidth() - 26 - maxTextSize);

	group_cover->SetX((clientSize.cx / 2) + 4);
	group_cover->SetWidth((clientSize.cx - 24) / 2 + (clientSize.cx % 2));

	image_covers->SetWidth(group_cover->GetWidth() - 113 - maxTextSize3);
	combo_cover_type->SetWidth(group_cover->GetWidth() - 26 - maxTextSize3);
	edit_cover_desc->SetWidth(group_cover->GetWidth() - 26 - maxTextSize3);
}

Void BoCA::LayerTagBasic::LoadCoverImages()
{
	foreach (const Picture &cover, track.pictures)
	{
		ImageEntry	*entry = new ImageEntry(cover.GetBitmap(), Size(70, 70));

		entry->onLeftButtonDoubleClick.Connect(&LayerTagBasic::DisplayCover, this);

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

Void BoCA::LayerTagBasic::AddCover()
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

		entry->onLeftButtonDoubleClick.Connect(&LayerTagBasic::DisplayCover, this);

		image_covers->Add(entry);

		Picture		 picture;
		InStream	 in(STREAM_FILE, file, IS_READ);

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

	combo_cover_type->SelectNthEntry(0);
	edit_cover_desc->SetText(NIL);

	button_cover_remove->Deactivate();
	text_cover_type->Deactivate();
	combo_cover_type->Deactivate();
	text_cover_desc->Deactivate();
	edit_cover_desc->Deactivate();
}

Void BoCA::LayerTagBasic::SelectCover(ListEntry *entry)
{
	Int		 index = image_covers->GetSelectedEntryNumber();
	const Picture	&picture = track.pictures.GetNth(index);

	button_cover_remove->Activate();
	text_cover_type->Activate();
	combo_cover_type->Activate();
	text_cover_desc->Activate();
	edit_cover_desc->Activate();

	combo_cover_type->onSelectEntry.Disconnect(&LayerTagBasic::OnModifyTrack, this);

	combo_cover_type->SelectNthEntry(picture.type);
	edit_cover_desc->SetText(picture.description);

	combo_cover_type->onSelectEntry.Connect(&LayerTagBasic::OnModifyTrack, this);
}

Void BoCA::LayerTagBasic::DisplayCover()
{
	Int		 index = image_covers->GetSelectedEntryNumber();
	const Picture	&picture = track.pictures.GetNth(index);

	CoverDisplay	 coverDisplay(picture.GetBitmap());

	coverDisplay.ShowDialog();
}

/* Called when a track is selected from the list.
 * ----
 * Copy new info to track and update input fields.
 */
Void BoCA::LayerTagBasic::OnSelectTrack(const Track &nTrack)
{
	if (&nTrack == &track) return;

	Surface	*surface = GetDrawSurface();

	surface->StartPaint(GetVisibleArea());

	OnSelectNone();

	track = nTrack;

	group_info->Activate();
	group_cover->Activate();

	text_title->Activate();
	edit_title->Activate();
	text_track->Activate();
	edit_track->Activate();

	const Info	&info = track.GetInfo();

	edit_artist->SetText(info.artist);
	edit_title->SetText(info.title);
	edit_album->SetText(info.album);
	edit_genre->SetText(info.genre);
	edit_comment->SetText(info.comment);

	if (info.year	   > 0) edit_year->SetText(String::FromInt(info.year));

	if (info.track	   > 0) edit_track->SetText(String(info.track < 10 ? "0" : "").Append(String::FromInt(info.track)));
	if (info.numTracks > 0) edit_ntracks->SetText(String(info.numTracks < 10 ? "0" : "").Append(String::FromInt(info.numTracks)));

	if (info.disc	   > 0) edit_disc->SetText(String(info.disc < 10 ? "0" : "").Append(String::FromInt(info.disc)));
	if (info.numDiscs  > 0) edit_ndiscs->SetText(String(info.numDiscs < 10 ? "0" : "").Append(String::FromInt(info.numDiscs)));

	LoadCoverImages();

	surface->EndPaint();
}

/* Called when an album is selected from the list.
 * ----
 * Copy new info to track and update input fields.
 */
Void BoCA::LayerTagBasic::OnSelectAlbum(const Track &nTrack)
{
	if (&nTrack == &track) return;

	Surface	*surface = GetDrawSurface();

	surface->StartPaint(GetVisibleArea());

	OnSelectNone();

	track = nTrack;

	group_info->Activate();
	group_cover->Activate();

	text_title->Deactivate();
	edit_title->Deactivate();
	text_track->Deactivate();
	edit_track->Deactivate();

	const Info	&info = track.GetInfo();

	edit_artist->SetText(info.artist);
	edit_album->SetText(info.album);
	edit_genre->SetText(info.genre);
	edit_comment->SetText(info.comment);

	if (info.year	   > 0) edit_year->SetText(String::FromInt(info.year));

	if (info.numTracks > 0) edit_ntracks->SetText(String(info.numTracks < 10 ? "0" : "").Append(String::FromInt(info.numTracks)));

	if (info.disc	   > 0) edit_disc->SetText(String(info.disc < 10 ? "0" : "").Append(String::FromInt(info.disc)));
	if (info.numDiscs  > 0) edit_ndiscs->SetText(String(info.numDiscs < 10 ? "0" : "").Append(String::FromInt(info.numDiscs)));

	LoadCoverImages();

	surface->EndPaint();
}

/* Called when the last track is removed from the list.
 * ----
 * Clear and deactivate all input fields.
 */
Void BoCA::LayerTagBasic::OnSelectNone()
{
	FreeCoverImages();

	combo_cover_type->onSelectEntry.Disconnect(&LayerTagBasic::OnModifyTrack, this);

	edit_artist->SetText(NIL);
	edit_title->SetText(NIL);
	edit_album->SetText(NIL);
	edit_genre->SetText(NIL);
	edit_year->SetText(NIL);
	edit_comment->SetText(NIL);

	edit_track->SetText(NIL);
	edit_ntracks->SetText(NIL);

	edit_disc->SetText(NIL);
	edit_ndiscs->SetText(NIL);

	combo_cover_type->SelectNthEntry(0);
	edit_cover_desc->SetText(NIL);

	group_info->Deactivate();
	group_cover->Deactivate();

	button_cover_remove->Deactivate();
	text_cover_type->Deactivate();
	combo_cover_type->Deactivate();
	text_cover_desc->Deactivate();
	edit_cover_desc->Deactivate();
}

/* Called when a track is modified.
 * ----
 * Write updated info back to track and emit onModifyTrack.
 */
Void BoCA::LayerTagBasic::OnModifyTrack()
{
	Info	&info = track.GetInfo();

	info.artist	= edit_artist->GetText();
	info.title	= edit_title->GetText();
	info.album	= edit_album->GetText();
	info.genre	= edit_genre->GetText();
	info.year	= edit_year->GetText().ToInt();
	info.comment	= edit_comment->GetText();

	info.track	= edit_track->GetText().ToInt();
	info.numTracks	= edit_ntracks->GetText().ToInt();

	info.disc	= edit_disc->GetText().ToInt();
	info.numDiscs	= edit_ndiscs->GetText().ToInt();

	Int	 index = image_covers->GetSelectedEntryNumber();

	if (index >= 0)
	{
		Picture	&picture = track.pictures.GetNthReference(index);

		picture.type	    = combo_cover_type->GetSelectedEntryNumber();
		picture.description = edit_cover_desc->GetText();
	}

	onModifyTrack.Emit(track);
}
