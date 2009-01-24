 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "layer_tag_details.h"
#include "../utilities.h"

using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

const char *INFO_BAND		= (char *) "Band/orchestra/accompaniment";
const char *INFO_PERFORMER	= (char *) "Conductor/performer refinement";
const char *INFO_REMIX		= (char *) "Interpreted, remixed or otherwise modified by";
const char *INFO_COMPOSER	= (char *) "Composer";
const char *INFO_LYRICIST	= (char *) "Lyricist/Text writer";

const char *INFO_ORIG_ARTIST	= (char *) "Original artist(s)/performer(s)";
const char *INFO_ORIG_ALBUM	= (char *) "Original album/movie/show title";
const char *INFO_ORIG_LYRICIST	= (char *) "Original lyricist(s)/text writer(s)";
const char *INFO_ORIG_YEAR	= (char *) "Original release year";

BoCA::LayerTagDetails::LayerTagDetails() : Layer("Details")
{
	I18n	*i18n = I18n::Get();

	group_details		= new GroupBox("Detailed info", Point(7, 10), Size(400, 147));

	text_band		= new Text(String(i18n->TranslateString("Band / orchestra")).Append(":"), Point(9, 13));
	text_conductor		= new Text(String(i18n->TranslateString("Performer refinement")).Append(":"), text_band->GetPosition() + Point(0, 27));
	text_remix		= new Text(String(i18n->TranslateString("Modified / remixed by")).Append(":"), text_conductor->GetPosition() + Point(0, 27));
	text_composer		= new Text(String(i18n->TranslateString("Composer")).Append(":"), text_remix->GetPosition() + Point(0, 27));
	text_textwriter		= new Text(String(i18n->TranslateString("Lyrics writer")).Append(":"), text_composer->GetPosition() + Point(0, 27));

	text_publisher		= new Text(String(i18n->TranslateString("Publisher / label")).Append(":"), Point(9, 13));
	text_isrc		= new Text(String(i18n->TranslateString("ISRC")).Append(":"), text_publisher->GetPosition() + Point(0, 27));

	Int	 maxTextSize = Math::Max(Math::Max(Math::Max(text_band->textSize.cx, text_conductor->textSize.cx), Math::Max(text_remix->textSize.cx, text_publisher->textSize.cx)), Math::Max(Math::Max(text_composer->textSize.cx, text_isrc->textSize.cx), text_textwriter->textSize.cx));

	edit_band		= new EditBox(NIL, text_band->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_band->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_conductor		= new EditBox(NIL, text_conductor->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_conductor->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_remix		= new EditBox(NIL, text_remix->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_remix->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_composer		= new EditBox(NIL, text_composer->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_composer->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_textwriter		= new EditBox(NIL, text_textwriter->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
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

	group_publisher		= new GroupBox("Publisher info", Point(7, 167), Size(400, 66));

	edit_publisher		= new EditBox(NIL, text_publisher->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_publisher->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_isrc		= new EditBox(NIL, text_isrc->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0), 12);
	edit_isrc->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	group_publisher->Add(text_publisher);
	group_publisher->Add(edit_publisher);
	group_publisher->Add(text_isrc);
	group_publisher->Add(edit_isrc);

	Add(group_publisher);

	group_original		= new GroupBox("Original info", Point(7, 10), Size(400, 120));

	text_oartist		= new Text(String(i18n->TranslateString("Original artist")).Append(":"), Point(9, 13));
	text_oalbum		= new Text(String(i18n->TranslateString("Original album")).Append(":"), text_oartist->GetPosition() + Point(0, 27));
	text_otextwriter	= new Text(String(i18n->TranslateString("Original lyrics writer")).Append(":"), text_oalbum->GetPosition() + Point(0, 27));
	text_oyear		= new Text(String(i18n->TranslateString("Original release year")).Append(":"), text_otextwriter->GetPosition() + Point(0, 27));

	Int	 maxTextSize2 = Math::Max(Math::Max(text_oartist->textSize.cx, text_oalbum->textSize.cx), Math::Max(text_otextwriter->textSize.cx, text_oyear->textSize.cx));

	edit_oartist		= new EditBox(NIL, text_oartist->GetPosition() + Point(maxTextSize2 + 7, -3), Size(300, 0));
	edit_oartist->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_oalbum		= new EditBox(NIL, text_oalbum->GetPosition() + Point(maxTextSize2 + 7, -3), Size(300, 0));
	edit_oalbum->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_otextwriter	= new EditBox(NIL, text_otextwriter->GetPosition() + Point(maxTextSize2 + 7, -3), Size(300, 0));
	edit_otextwriter->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	edit_oyear		= new EditBox(NIL, text_oyear->GetPosition() + Point(maxTextSize2 + 7, -3), Size(50, 0), 4);
	edit_oyear->SetFlags(EDB_NUMERIC);
	edit_oyear->onInput.Connect(&LayerTagDetails::OnModifyTrack, this);

	group_original->Add(text_oartist);
	group_original->Add(edit_oartist);
	group_original->Add(text_oalbum);
	group_original->Add(edit_oalbum);
	group_original->Add(text_otextwriter);
	group_original->Add(edit_otextwriter);
	group_original->Add(text_oyear);
	group_original->Add(edit_oyear);

	Add(group_original);

	onChangeSize.Connect(&LayerTagDetails::OnChangeSize, this);

	/* Initially deactivate all input fields.
	 */
	OnSelectNone();
}

BoCA::LayerTagDetails::~LayerTagDetails()
{
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

	DeleteObject(group_original);
	DeleteObject(text_oartist);
	DeleteObject(edit_oartist);
	DeleteObject(text_oalbum);
	DeleteObject(edit_oalbum);
	DeleteObject(text_otextwriter);
	DeleteObject(edit_otextwriter);
	DeleteObject(text_oyear);
	DeleteObject(edit_oyear);
}

/* Called when layer size changes.
 * ----
 */
Void BoCA::LayerTagDetails::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	group_details->SetWidth((clientSize.cx - 23) / 2);
	group_publisher->SetWidth((clientSize.cx - 23) / 2);

	Int	 maxTextSize = Math::Max(Math::Max(Math::Max(text_band->textSize.cx, text_conductor->textSize.cx), Math::Max(text_remix->textSize.cx, text_publisher->textSize.cx)), Math::Max(Math::Max(text_composer->textSize.cx, text_isrc->textSize.cx), text_textwriter->textSize.cx));
	Int	 maxTextSize2 = Math::Max(Math::Max(text_oartist->textSize.cx, text_oalbum->textSize.cx), Math::Max(text_otextwriter->textSize.cx, text_oyear->textSize.cx));

	edit_band->SetWidth(group_details->GetWidth() - 26 - maxTextSize);
	edit_conductor->SetWidth(group_details->GetWidth() - 26 - maxTextSize);
	edit_remix->SetWidth(group_details->GetWidth() - 26 - maxTextSize);
	edit_composer->SetWidth(group_details->GetWidth() - 26 - maxTextSize);
	edit_textwriter->SetWidth(group_details->GetWidth() - 26 - maxTextSize);

	edit_publisher->SetWidth(group_details->GetWidth() - 26 - maxTextSize);
	edit_isrc->SetWidth(group_details->GetWidth() - 26 - maxTextSize);

	group_original->SetX((clientSize.cx / 2) + 4);
	group_original->SetWidth((clientSize.cx - 24) / 2 + (clientSize.cx % 2));

	edit_oartist->SetWidth(group_original->GetWidth() - 26 - maxTextSize2);
	edit_oalbum->SetWidth(group_original->GetWidth() - 26 - maxTextSize2);
	edit_otextwriter->SetWidth(group_original->GetWidth() - 26 - maxTextSize2);
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
	group_original->Activate();

	const Info	&info = track.GetInfo();

	edit_publisher->SetText(info.label);
	edit_isrc->SetText(info.isrc);

	for (Int i = 0; i < info.other.Length(); i++)
	{
		String	 value = info.other.GetNth(i);

		if	(value.StartsWith(String(INFO_BAND).Append(":")))	   { edit_band->SetText(value.Tail(value.Length() - value.Find(":") - 1));	  }
		else if	(value.StartsWith(String(INFO_PERFORMER).Append(":")))	   { edit_conductor->SetText(value.Tail(value.Length() - value.Find(":") - 1));   }
		else if	(value.StartsWith(String(INFO_REMIX).Append(":")))	   { edit_remix->SetText(value.Tail(value.Length() - value.Find(":") - 1));	  }
		else if	(value.StartsWith(String(INFO_COMPOSER).Append(":")))	   { edit_composer->SetText(value.Tail(value.Length() - value.Find(":") - 1));	  }
		else if	(value.StartsWith(String(INFO_LYRICIST).Append(":")))	   { edit_textwriter->SetText(value.Tail(value.Length() - value.Find(":") - 1));  }

		else if	(value.StartsWith(String(INFO_ORIG_ARTIST).Append(":")))   { edit_oartist->SetText(value.Tail(value.Length() - value.Find(":") - 1));	  }
		else if	(value.StartsWith(String(INFO_ORIG_ALBUM).Append(":")))    { edit_oalbum->SetText(value.Tail(value.Length() - value.Find(":") - 1));	  }
		else if	(value.StartsWith(String(INFO_ORIG_LYRICIST).Append(":"))) { edit_otextwriter->SetText(value.Tail(value.Length() - value.Find(":") - 1)); }
		else if	(value.StartsWith(String(INFO_ORIG_YEAR).Append(":")))	   { edit_oyear->SetText(value.Tail(value.Length() - value.Find(":") - 1));	  }
	}


	surface->EndPaint();
}

/* Called when the last track is removed from the list.
 * ----
 * Clear and deactivate all input fields.
 */
Void BoCA::LayerTagDetails::OnSelectNone()
{
	edit_band->SetText(NIL);
	edit_conductor->SetText(NIL);
	edit_remix->SetText(NIL);
	edit_composer->SetText(NIL);
	edit_textwriter->SetText(NIL);

	edit_publisher->SetText(NIL);
	edit_isrc->SetText(NIL);

	edit_oartist->SetText(NIL);
	edit_oalbum->SetText(NIL);
	edit_otextwriter->SetText(NIL);
	edit_oyear->SetText(NIL);

	group_details->Deactivate();
	group_publisher->Deactivate();
	group_original->Deactivate();
}

/* Called when a track is modified.
 * ----
 * Write updated info back to track and emit onModifyTrack.
 */
Void BoCA::LayerTagDetails::OnModifyTrack()
{
	Info	&info = track.GetInfo();

	info.label	= edit_publisher->GetText();
	info.isrc	= edit_isrc->GetText();

	Bool	 modified_band		= False;
	Bool	 modified_conductor	= False;
	Bool	 modified_remix		= False;
	Bool	 modified_composer	= False;
	Bool	 modified_textwriter	= False;

	Bool	 modified_oartist	= False;
	Bool	 modified_oalbum	= False;
	Bool	 modified_otextwriter	= False;
	Bool	 modified_oyear		= False;

	for (Int i = 0; i < info.other.Length(); i++)
	{
		String	 value = info.other.GetNth(i);

		if	(value.StartsWith(String(INFO_BAND).Append(":")))	   { if (edit_band->GetText()	     != NIL) { info.other.SetNth(i, String(INFO_BAND).Append(":").Append(edit_band->GetText()));		 modified_band		= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_PERFORMER).Append(":")))	   { if (edit_conductor->GetText()   != NIL) { info.other.SetNth(i, String(INFO_PERFORMER).Append(":").Append(edit_conductor->GetText()));	 modified_conductor	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_REMIX).Append(":")))	   { if (edit_remix->GetText()	     != NIL) { info.other.SetNth(i, String(INFO_REMIX).Append(":").Append(edit_remix->GetText()));		 modified_remix		= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_COMPOSER).Append(":")))	   { if (edit_composer->GetText()    != NIL) { info.other.SetNth(i, String(INFO_COMPOSER).Append(":").Append(edit_composer->GetText()));	 modified_composer	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_LYRICIST).Append(":")))	   { if (edit_textwriter->GetText()  != NIL) { info.other.SetNth(i, String(INFO_LYRICIST).Append(":").Append(edit_textwriter->GetText()));	 modified_textwriter	= True; } else { info.other.RemoveNth(i); } }

		else if	(value.StartsWith(String(INFO_ORIG_ARTIST).Append(":")))   { if (edit_oartist->GetText()     != NIL) { info.other.SetNth(i, String(INFO_ORIG_ARTIST).Append(":").Append(edit_oartist->GetText()));	 modified_oartist	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_ORIG_ALBUM).Append(":")))    { if (edit_oalbum->GetText()      != NIL) { info.other.SetNth(i, String(INFO_ORIG_ALBUM).Append(":").Append(edit_oalbum->GetText()));	 modified_oalbum	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_ORIG_LYRICIST).Append(":"))) { if (edit_otextwriter->GetText() != NIL) { info.other.SetNth(i, String(INFO_ORIG_LYRICIST).Append(":").Append(edit_otextwriter->GetText())); modified_otextwriter	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_ORIG_YEAR).Append(":")))	   { if (edit_oyear->GetText()	     != NIL) { info.other.SetNth(i, String(INFO_ORIG_YEAR).Append(":").Append(edit_oyear->GetText()));		 modified_oyear		= True; } else { info.other.RemoveNth(i); } }
	}

	if	(!modified_band	       && edit_band->GetText()	      != NIL) info.other.Add(String(INFO_BAND).Append(":").Append(edit_band->GetText()));
	else if	(!modified_conductor   && edit_conductor->GetText()   != NIL) info.other.Add(String(INFO_PERFORMER).Append(":").Append(edit_conductor->GetText()));
	else if	(!modified_remix       && edit_remix->GetText()	      != NIL) info.other.Add(String(INFO_REMIX).Append(":").Append(edit_remix->GetText()));
	else if	(!modified_composer    && edit_composer->GetText()    != NIL) info.other.Add(String(INFO_COMPOSER).Append(":").Append(edit_composer->GetText()));
	else if	(!modified_textwriter  && edit_textwriter->GetText()  != NIL) info.other.Add(String(INFO_LYRICIST).Append(":").Append(edit_textwriter->GetText()));

	else if	(!modified_oartist     && edit_oartist->GetText()     != NIL) info.other.Add(String(INFO_ORIG_ARTIST).Append(":").Append(edit_oartist->GetText()));
	else if	(!modified_oalbum      && edit_oalbum->GetText()      != NIL) info.other.Add(String(INFO_ORIG_ALBUM).Append(":").Append(edit_oalbum->GetText()));
	else if	(!modified_otextwriter && edit_otextwriter->GetText() != NIL) info.other.Add(String(INFO_ORIG_LYRICIST).Append(":").Append(edit_otextwriter->GetText()));
	else if	(!modified_oyear       && edit_oyear->GetText()	      != NIL) info.other.Add(String(INFO_ORIG_YEAR).Append(":").Append(edit_oyear->GetText()));

	onModifyTrack.Emit(track);
}
