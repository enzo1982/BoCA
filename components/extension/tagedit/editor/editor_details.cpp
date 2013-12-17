 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "editor_details.h"
#include "../utilities.h"

using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

BoCA::LayerTagDetails::LayerTagDetails() : Editor("Details")
{
	group_details		= new GroupBox(NIL, Point(7, 10), Size(400, 147));

	text_band		= new Text(NIL, Point(9, 13));
	text_conductor		= new Text(NIL, text_band->GetPosition() + Point(0, 27));
	text_remix		= new Text(NIL, text_conductor->GetPosition() + Point(0, 27));
	text_composer		= new Text(NIL, text_remix->GetPosition() + Point(0, 27));
	text_textwriter		= new Text(NIL, text_composer->GetPosition() + Point(0, 27));

	edit_band		= new EditBox(NIL, text_band->GetPosition() + Point(7, -3), Size(300, 0));
	edit_band->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_conductor		= new EditBox(NIL, text_conductor->GetPosition() + Point(7, -3), Size(300, 0));
	edit_conductor->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_remix		= new EditBox(NIL, text_remix->GetPosition() + Point(7, -3), Size(300, 0));
	edit_remix->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_composer		= new EditBox(NIL, text_composer->GetPosition() + Point(7, -3), Size(300, 0));
	edit_composer->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_textwriter		= new EditBox(NIL, text_textwriter->GetPosition() + Point(7, -3), Size(300, 0));
	edit_textwriter->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	group_details->Add(text_band);
	group_details->Add(edit_band);
	group_details->Add(text_conductor);
	group_details->Add(edit_conductor);
	group_details->Add(text_remix);
	group_details->Add(edit_remix);
	group_details->Add(text_composer);
	group_details->Add(edit_composer);
	group_details->Add(text_textwriter);
	group_details->Add(edit_textwriter);

	Add(group_details);

	group_publisher		= new GroupBox(NIL, Point(7, 10), Size(400, 66));

	text_publisher		= new Text(NIL, Point(9, 13));
	text_isrc		= new Text(NIL, text_publisher->GetPosition() + Point(0, 27));

	edit_publisher		= new EditBox(NIL, text_publisher->GetPosition() + Point(7, -3), Size(300, 0));
	edit_publisher->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_isrc		= new EditBox(NIL, text_isrc->GetPosition() + Point(7, -3), Size(300, 0), 12);
	edit_isrc->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	group_publisher->Add(text_publisher);
	group_publisher->Add(edit_publisher);
	group_publisher->Add(text_isrc);
	group_publisher->Add(edit_isrc);

	Add(group_publisher);

	onChangeSize.Connect(&LayerTagDetails::OnChangeSize, this);

	Settings::Get()->onChangeLanguageSettings.Connect(&LayerTagDetails::OnChangeLanguageSettings, this);

	/* Initially deactivate all input fields.
	 */
	OnSelectNone();
}

BoCA::LayerTagDetails::~LayerTagDetails()
{
	Settings::Get()->onChangeLanguageSettings.Disconnect(&LayerTagDetails::OnChangeLanguageSettings, this);

	DeleteObject(group_details);
	DeleteObject(text_band);
	DeleteObject(edit_band);
	DeleteObject(text_conductor);
	DeleteObject(edit_conductor);
	DeleteObject(text_remix);
	DeleteObject(edit_remix);
	DeleteObject(text_composer);
	DeleteObject(edit_composer);
	DeleteObject(text_textwriter);
	DeleteObject(edit_textwriter);

	DeleteObject(group_publisher);
	DeleteObject(text_publisher);
	DeleteObject(edit_publisher);
	DeleteObject(text_isrc);
	DeleteObject(edit_isrc);
}

/* Called when layer size changes.
 * ----
 */
Void BoCA::LayerTagDetails::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	group_details->SetWidth((clientSize.cx - 23) / 2);

	Int	 maxTextSize = Math::Max(Math::Max(Math::Max(text_band->GetUnscaledTextWidth(), text_conductor->GetUnscaledTextWidth()), text_remix->GetUnscaledTextWidth()), Math::Max(text_composer->GetUnscaledTextWidth(), text_textwriter->GetUnscaledTextWidth()));
	Int	 maxTextSize2 = Math::Max(text_publisher->GetUnscaledTextWidth(), text_isrc->GetUnscaledTextWidth());

	edit_band->SetWidth(group_details->GetWidth() - 26 - maxTextSize);
	edit_conductor->SetWidth(group_details->GetWidth() - 26 - maxTextSize);
	edit_remix->SetWidth(group_details->GetWidth() - 26 - maxTextSize);
	edit_composer->SetWidth(group_details->GetWidth() - 26 - maxTextSize);
	edit_textwriter->SetWidth(group_details->GetWidth() - 26 - maxTextSize);

	group_publisher->SetX((clientSize.cx / 2) + 4);
	group_publisher->SetWidth((clientSize.cx - 24) / 2 + (clientSize.cx % 2));

	edit_publisher->SetWidth(group_details->GetWidth() - 26 - maxTextSize2);
	edit_isrc->SetWidth(group_details->GetWidth() - 26 - maxTextSize2);
}

/* Called when application language is changed.
 * ----
 */
Void BoCA::LayerTagDetails::OnChangeLanguageSettings()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Extensions::Tag Editor::Details");

	SetText(i18n->TranslateString("Details"));

	/* Hide all affected widgets prior to changing
	 * labels to avoid flickering.
	 */
	Bool	 prevVisible = IsVisible();

	if (prevVisible) Hide();

	group_details->SetText(i18n->TranslateString("Detailed information"));

	text_band->SetText(String(i18n->TranslateString("Band / orchestra")).Append(":"));
	text_conductor->SetText(String(i18n->TranslateString("Performer refinement")).Append(":"));
	text_remix->SetText(String(i18n->TranslateString("Modified / remixed by")).Append(":"));
	text_composer->SetText(String(i18n->TranslateString("Composer")).Append(":"));
	text_textwriter->SetText(String(i18n->TranslateString("Lyrics writer")).Append(":"));

	Int	 maxTextSize = Math::Max(Math::Max(Math::Max(text_band->GetUnscaledTextWidth(), text_conductor->GetUnscaledTextWidth()), text_remix->GetUnscaledTextWidth()), Math::Max(text_composer->GetUnscaledTextWidth(), text_textwriter->GetUnscaledTextWidth()));

	edit_band->SetX(text_band->GetX() + maxTextSize + 7);
	edit_conductor->SetX(text_conductor->GetX() + maxTextSize + 7);
	edit_remix->SetX(text_remix->GetX() + maxTextSize + 7);
	edit_composer->SetX(text_composer->GetX() + maxTextSize + 7);
	edit_textwriter->SetX(text_textwriter->GetX() + maxTextSize + 7);

	group_publisher->SetText(i18n->TranslateString("Publisher information"));

	text_publisher->SetText(String(i18n->TranslateString("Publisher / label")).Append(":"));
	text_isrc->SetText(String(i18n->TranslateString("ISRC")).Append(":"));

	Int	 maxTextSize2 = Math::Max(text_publisher->GetUnscaledTextWidth(), text_isrc->GetUnscaledTextWidth());

	edit_publisher->SetX(text_publisher->GetX() + maxTextSize2 + 7);
	edit_isrc->SetX(text_isrc->GetX() + maxTextSize2 + 7);

	/* OnChangeSize will correct sizes of any other widgets.
	 */
	OnChangeSize(GetSize());

	/* Show all widgets again.
	 */
	if (prevVisible) Show();
}

EditBox *BoCA::LayerTagDetails::GetActiveEditBox()
{
	if	(edit_band->IsFocussed())	return edit_band;
	else if	(edit_conductor->IsFocussed())	return edit_conductor;
	else if	(edit_remix->IsFocussed())	return edit_remix;
	else if	(edit_composer->IsFocussed())	return edit_composer;
	else if	(edit_textwriter->IsFocussed())	return edit_textwriter;
	else if	(edit_publisher->IsFocussed())	return edit_publisher;
	else if	(edit_isrc->IsFocussed())	return edit_isrc;

	return NIL;
}

/* Called when a track is selected from the list.
 * ----
 * Copy new info to track and update input fields.
 */
Void BoCA::LayerTagDetails::OnSelectTrack(const Track &nTrack)
{
	if (&nTrack == &track) return;

	Surface	*surface = GetDrawSurface();

	surface->StartPaint(GetVisibleArea());

	OnSelectNone();

	track = nTrack;

	group_details->Activate();
	group_publisher->Activate();

	text_isrc->Activate();
	edit_isrc->Activate();

	const Info	&info = track.GetInfo();

	edit_publisher->SetText(info.label);
	edit_isrc->SetText(info.isrc);

	for (Int i = 0; i < info.other.Length(); i++)
	{
		const String	&value = info.other.GetNth(i);

		if	(value.StartsWith(String(INFO_BAND).Append(":")))	   { edit_band->SetText(value.Tail(value.Length() - value.Find(":") - 1));	  }
		else if	(value.StartsWith(String(INFO_CONDUCTOR).Append(":")))	   { edit_conductor->SetText(value.Tail(value.Length() - value.Find(":") - 1));   }
		else if	(value.StartsWith(String(INFO_REMIX).Append(":")))	   { edit_remix->SetText(value.Tail(value.Length() - value.Find(":") - 1));	  }
		else if	(value.StartsWith(String(INFO_COMPOSER).Append(":")))	   { edit_composer->SetText(value.Tail(value.Length() - value.Find(":") - 1));	  }
		else if	(value.StartsWith(String(INFO_LYRICIST).Append(":")))	   { edit_textwriter->SetText(value.Tail(value.Length() - value.Find(":") - 1));  }
	}

	EditBox	*activeEditBox = GetActiveEditBox();

	if (activeEditBox != NIL)
	{
		activeEditBox->SetFocus();
		activeEditBox->MarkAll();
	}

	surface->EndPaint();
}

/* Called when an album is selected from the list.
 * ----
 * Copy new info to track and update input fields.
 */
Void BoCA::LayerTagDetails::OnSelectAlbum(const Track &nTrack)
{
	if (&nTrack == &track) return;

	Surface	*surface = GetDrawSurface();

	surface->StartPaint(GetVisibleArea());

	OnSelectNone();

	track = nTrack;

	group_publisher->Activate();

	text_isrc->Deactivate();
	edit_isrc->Deactivate();

	const Info	&info = track.GetInfo();

	edit_publisher->SetText(info.label);

	EditBox	*activeEditBox = GetActiveEditBox();

	if (activeEditBox != NIL)
	{
		activeEditBox->SetFocus();
		activeEditBox->MarkAll();
	}

	surface->EndPaint();
}

/* Called when the last track is removed from the list.
 * ----
 * Clear and deactivate all input fields.
 */
Void BoCA::LayerTagDetails::OnSelectNone()
{
	Surface	*surface = GetDrawSurface();

	surface->StartPaint(GetVisibleArea());

	edit_band->SetText(NIL);
	edit_conductor->SetText(NIL);
	edit_remix->SetText(NIL);
	edit_composer->SetText(NIL);
	edit_textwriter->SetText(NIL);

	edit_publisher->SetText(NIL);
	edit_isrc->SetText(NIL);

	group_details->Deactivate();
	group_publisher->Deactivate();

	surface->EndPaint();

	track = NIL;
}

/* Called when a track is modified.
 * ----
 * Write updated info back to track and emit onModifyTrack.
 */
Void BoCA::LayerTagDetails::OnModifyTrack()
{
	Info	 info = track.GetInfo();

	info.label	= edit_publisher->GetText();
	info.isrc	= edit_isrc->GetText();

	Bool	 modified_band		= False;
	Bool	 modified_conductor	= False;
	Bool	 modified_remix		= False;
	Bool	 modified_composer	= False;
	Bool	 modified_textwriter	= False;

	for (Int i = 0; i < info.other.Length(); i++)
	{
		const String	&value = info.other.GetNth(i);

		if	(value.StartsWith(String(INFO_BAND).Append(":")))	   { if (edit_band->GetText()	     != NIL) { info.other.SetNth(i, String(INFO_BAND).Append(":").Append(edit_band->GetText()));		 modified_band		= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_CONDUCTOR).Append(":")))	   { if (edit_conductor->GetText()   != NIL) { info.other.SetNth(i, String(INFO_CONDUCTOR).Append(":").Append(edit_conductor->GetText()));	 modified_conductor	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_REMIX).Append(":")))	   { if (edit_remix->GetText()	     != NIL) { info.other.SetNth(i, String(INFO_REMIX).Append(":").Append(edit_remix->GetText()));		 modified_remix		= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_COMPOSER).Append(":")))	   { if (edit_composer->GetText()    != NIL) { info.other.SetNth(i, String(INFO_COMPOSER).Append(":").Append(edit_composer->GetText()));	 modified_composer	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_LYRICIST).Append(":")))	   { if (edit_textwriter->GetText()  != NIL) { info.other.SetNth(i, String(INFO_LYRICIST).Append(":").Append(edit_textwriter->GetText()));	 modified_textwriter	= True; } else { info.other.RemoveNth(i); } }
	}

	if	(!modified_band	       && edit_band->GetText()	      != NIL) info.other.Add(String(INFO_BAND).Append(":").Append(edit_band->GetText()));
	else if	(!modified_conductor   && edit_conductor->GetText()   != NIL) info.other.Add(String(INFO_CONDUCTOR).Append(":").Append(edit_conductor->GetText()));
	else if	(!modified_remix       && edit_remix->GetText()	      != NIL) info.other.Add(String(INFO_REMIX).Append(":").Append(edit_remix->GetText()));
	else if	(!modified_composer    && edit_composer->GetText()    != NIL) info.other.Add(String(INFO_COMPOSER).Append(":").Append(edit_composer->GetText()));
	else if	(!modified_textwriter  && edit_textwriter->GetText()  != NIL) info.other.Add(String(INFO_LYRICIST).Append(":").Append(edit_textwriter->GetText()));

	track.SetInfo(info);

	onModifyTrack.Emit(track);
}
