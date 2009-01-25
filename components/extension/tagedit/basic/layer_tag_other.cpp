 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "layer_tag_other.h"
#include "../utilities.h"

using namespace smooth::IO;
using namespace smooth::GUI::Dialogs;

const char *INFO_ORIG_ARTIST	= (char *) "Original artist(s)/performer(s)";
const char *INFO_ORIG_ALBUM	= (char *) "Original album/movie/show title";
const char *INFO_ORIG_LYRICIST	= (char *) "Original lyricist(s)/text writer(s)";
const char *INFO_ORIG_YEAR	= (char *) "Original release year";

const char *INFO_WEB_ARTIST	= (char *) "Official artist webpage";
const char *INFO_WEB_PUBLISHER	= (char *) "Official publisher webpage";
const char *INFO_WEB_RADIO	= (char *) "Official radio webpage";
const char *INFO_WEB_SOURCE	= (char *) "Official audio source webpage";
const char *INFO_WEB_COPYRIGHT	= (char *) "Copyright information webpage";
const char *INFO_WEB_COMMERCIAL	= (char *) "Commercial information  webpage";

BoCA::LayerTagOther::LayerTagOther() : Layer("Other")
{
	I18n	*i18n = I18n::Get();

	group_original		= new GroupBox("Original information", Point(7, 10), Size(400, 120));

	text_oartist		= new Text(String(i18n->TranslateString("Original artist")).Append(":"), Point(9, 13));
	text_oalbum		= new Text(String(i18n->TranslateString("Original album")).Append(":"), text_oartist->GetPosition() + Point(0, 27));
	text_otextwriter	= new Text(String(i18n->TranslateString("Original lyrics writer")).Append(":"), text_oalbum->GetPosition() + Point(0, 27));
	text_oyear		= new Text(String(i18n->TranslateString("Original release year")).Append(":"), text_otextwriter->GetPosition() + Point(0, 27));

	Int	 maxTextSize = Math::Max(Math::Max(text_oartist->textSize.cx, text_oalbum->textSize.cx), Math::Max(text_otextwriter->textSize.cx, text_oyear->textSize.cx));

	edit_oartist		= new EditBox(NIL, text_oartist->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_oartist->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	edit_oalbum		= new EditBox(NIL, text_oalbum->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_oalbum->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	edit_otextwriter	= new EditBox(NIL, text_otextwriter->GetPosition() + Point(maxTextSize + 7, -3), Size(300, 0));
	edit_otextwriter->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	edit_oyear		= new EditBox(NIL, text_oyear->GetPosition() + Point(maxTextSize + 7, -3), Size(50, 0), 4);
	edit_oyear->SetFlags(EDB_NUMERIC);
	edit_oyear->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	group_original->Add(text_oartist);
	group_original->Add(edit_oartist);
	group_original->Add(text_oalbum);
	group_original->Add(edit_oalbum);
	group_original->Add(text_otextwriter);
	group_original->Add(edit_otextwriter);
	group_original->Add(text_oyear);
	group_original->Add(edit_oyear);

	Add(group_original);

	group_web		= new GroupBox("Webpage URLs", Point(7, 10), Size(400, 174));

	text_wartist		= new Text(String(i18n->TranslateString("Artist webpage")).Append(":"), Point(9, 13));
	text_wpublisher		= new Text(String(i18n->TranslateString("Publisher webpage")).Append(":"), text_wartist->GetPosition() + Point(0, 27));
	text_wradio		= new Text(String(i18n->TranslateString("Internet radio station")).Append(":"), text_wpublisher->GetPosition() + Point(0, 27));
	text_wsource		= new Text(String(i18n->TranslateString("Audio source webpage")).Append(":"), text_wradio->GetPosition() + Point(0, 27));
	text_wcopyright		= new Text(String(i18n->TranslateString("Copyright information")).Append(":"), text_wsource->GetPosition() + Point(0, 27));
	text_wcommercial	= new Text(String(i18n->TranslateString("Commercial information")).Append(":"), text_wcopyright->GetPosition() + Point(0, 27));

	Int	 maxTextSize2 = Math::Max(Math::Max(Math::Max(text_wartist->textSize.cx, text_wcopyright->textSize.cx), Math::Max(text_wpublisher->textSize.cx, text_wcommercial->textSize.cx)), Math::Max(text_wradio->textSize.cx, text_wsource->textSize.cx));

	edit_wartist		= new EditBox(NIL, text_wartist->GetPosition() + Point(maxTextSize2 + 7, -3), Size(300, 0));
	edit_wartist->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	edit_wpublisher		= new EditBox(NIL, text_wpublisher->GetPosition() + Point(maxTextSize2 + 7, -3), Size(300, 0));
	edit_wpublisher->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	edit_wradio		= new EditBox(NIL, text_wradio->GetPosition() + Point(maxTextSize2 + 7, -3), Size(300, 0));
	edit_wradio->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	edit_wsource		= new EditBox(NIL, text_wsource->GetPosition() + Point(maxTextSize2 + 7, -3), Size(300, 0));
	edit_wsource->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	edit_wcopyright		= new EditBox(NIL, text_wcopyright->GetPosition() + Point(maxTextSize2 + 7, -3), Size(300, 0));
	edit_wcopyright->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	edit_wcommercial	= new EditBox(NIL, text_wcommercial->GetPosition() + Point(maxTextSize2 + 7, -3), Size(300, 0));
	edit_wcommercial->onInput.Connect(&LayerTagOther::OnModifyTrack, this);

	group_web->Add(text_wartist);
	group_web->Add(edit_wartist);
	group_web->Add(text_wpublisher);
	group_web->Add(edit_wpublisher);
	group_web->Add(text_wradio);
	group_web->Add(edit_wradio);
	group_web->Add(text_wsource);
	group_web->Add(edit_wsource);
	group_web->Add(text_wcopyright);
	group_web->Add(edit_wcopyright);
	group_web->Add(text_wcommercial);
	group_web->Add(edit_wcommercial);

	Add(group_web);

	onChangeSize.Connect(&LayerTagOther::OnChangeSize, this);

	/* Initially deactivate all input fields.
	 */
	OnSelectNone();
}

BoCA::LayerTagOther::~LayerTagOther()
{
	DeleteObject(group_original);
	DeleteObject(text_oartist);
	DeleteObject(edit_oartist);
	DeleteObject(text_oalbum);
	DeleteObject(edit_oalbum);
	DeleteObject(text_otextwriter);
	DeleteObject(edit_otextwriter);
	DeleteObject(text_oyear);
	DeleteObject(edit_oyear);

	DeleteObject(group_web);
	DeleteObject(text_wartist);
	DeleteObject(edit_wartist);
	DeleteObject(text_wpublisher);
	DeleteObject(edit_wpublisher);
	DeleteObject(text_wradio);
	DeleteObject(edit_wradio);
	DeleteObject(text_wsource);
	DeleteObject(edit_wsource);
	DeleteObject(text_wcopyright);
	DeleteObject(edit_wcopyright);
	DeleteObject(text_wcommercial);
	DeleteObject(edit_wcommercial);
}

/* Called when layer size changes.
 * ----
 */
Void BoCA::LayerTagOther::OnChangeSize(const Size &nSize)
{
	Rect	 clientRect = Rect(GetPosition(), GetSize());
	Size	 clientSize = Size(clientRect.right - clientRect.left, clientRect.bottom - clientRect.top);

	group_original->SetWidth((clientSize.cx - 23) / 2);

	Int	 maxTextSize = Math::Max(Math::Max(text_oartist->textSize.cx, text_oalbum->textSize.cx), Math::Max(text_otextwriter->textSize.cx, text_oyear->textSize.cx));
	Int	 maxTextSize2 = Math::Max(Math::Max(Math::Max(text_wartist->textSize.cx, text_wcopyright->textSize.cx), Math::Max(text_wpublisher->textSize.cx, text_wcommercial->textSize.cx)), Math::Max(text_wradio->textSize.cx, text_wsource->textSize.cx));

	edit_oartist->SetWidth(group_original->GetWidth() - 26 - maxTextSize);
	edit_oalbum->SetWidth(group_original->GetWidth() - 26 - maxTextSize);
	edit_otextwriter->SetWidth(group_original->GetWidth() - 26 - maxTextSize);

	group_web->SetX((clientSize.cx / 2) + 4);
	group_web->SetWidth((clientSize.cx - 24) / 2 + (clientSize.cx % 2));

	edit_wartist->SetWidth(group_web->GetWidth() - 26 - maxTextSize2);
	edit_wpublisher->SetWidth(group_web->GetWidth() - 26 - maxTextSize2);
	edit_wradio->SetWidth(group_web->GetWidth() - 26 - maxTextSize2);
	edit_wsource->SetWidth(group_web->GetWidth() - 26 - maxTextSize2);
	edit_wcopyright->SetWidth(group_web->GetWidth() - 26 - maxTextSize2);
	edit_wcommercial->SetWidth(group_web->GetWidth() - 26 - maxTextSize2);
}

/* Called when a track is selected from the list.
 * ----
 * Copy new info to track and update input fields.
 */
Void BoCA::LayerTagOther::OnSelectTrack(const Track &nTrack)
{
	if (&nTrack == &track) return;

	Surface	*surface = GetDrawSurface();

	surface->StartPaint(GetVisibleArea());

	OnSelectNone();

	track = nTrack;

	group_original->Activate();
	group_web->Activate();

	const Info	&info = track.GetInfo();

	for (Int i = 0; i < info.other.Length(); i++)
	{
		String	 value = info.other.GetNth(i);

		if	(value.StartsWith(String(INFO_ORIG_ARTIST).Append(":")))    { edit_oartist->SetText(value.Tail(value.Length() - value.Find(":") - 1));	   }
		else if	(value.StartsWith(String(INFO_ORIG_ALBUM).Append(":")))     { edit_oalbum->SetText(value.Tail(value.Length() - value.Find(":") - 1));	   }
		else if	(value.StartsWith(String(INFO_ORIG_LYRICIST).Append(":")))  { edit_otextwriter->SetText(value.Tail(value.Length() - value.Find(":") - 1)); }
		else if	(value.StartsWith(String(INFO_ORIG_YEAR).Append(":")))	    { edit_oyear->SetText(value.Tail(value.Length() - value.Find(":") - 1));	   }

		else if	(value.StartsWith(String(INFO_WEB_ARTIST).Append(":")))	    { edit_wartist->SetText(value.Tail(value.Length() - value.Find(":") - 1));	   }
		else if	(value.StartsWith(String(INFO_WEB_PUBLISHER).Append(":")))  { edit_wpublisher->SetText(value.Tail(value.Length() - value.Find(":") - 1));  }
		else if	(value.StartsWith(String(INFO_WEB_RADIO).Append(":")))	    { edit_wradio->SetText(value.Tail(value.Length() - value.Find(":") - 1));	   }
		else if	(value.StartsWith(String(INFO_WEB_SOURCE).Append(":")))	    { edit_wsource->SetText(value.Tail(value.Length() - value.Find(":") - 1));	   }
		else if	(value.StartsWith(String(INFO_WEB_COPYRIGHT).Append(":")))  { edit_wcopyright->SetText(value.Tail(value.Length() - value.Find(":") - 1));  }
		else if	(value.StartsWith(String(INFO_WEB_COMMERCIAL).Append(":"))) { edit_wcommercial->SetText(value.Tail(value.Length() - value.Find(":") - 1)); }
	}


	surface->EndPaint();
}

/* Called when the last track is removed from the list.
 * ----
 * Clear and deactivate all input fields.
 */
Void BoCA::LayerTagOther::OnSelectNone()
{
	edit_oartist->SetText(NIL);
	edit_oalbum->SetText(NIL);
	edit_otextwriter->SetText(NIL);
	edit_oyear->SetText(NIL);

	edit_wartist->SetText(NIL);
	edit_wpublisher->SetText(NIL);
	edit_wradio->SetText(NIL);
	edit_wsource->SetText(NIL);
	edit_wcopyright->SetText(NIL);
	edit_wcommercial->SetText(NIL);

	group_original->Deactivate();
	group_web->Deactivate();
}

/* Called when a track is modified.
 * ----
 * Write updated info back to track and emit onModifyTrack.
 */
Void BoCA::LayerTagOther::OnModifyTrack()
{
	Info	&info = track.GetInfo();

	Bool	 modified_oartist	= False;
	Bool	 modified_oalbum	= False;
	Bool	 modified_otextwriter	= False;
	Bool	 modified_oyear		= False;

	Bool	 modified_wartist	= False;
	Bool	 modified_wpublisher	= False;
	Bool	 modified_wradio	= False;
	Bool	 modified_wsource	= False;
	Bool	 modified_wcopyright	= False;
	Bool	 modified_wcommercial	= False;

	for (Int i = 0; i < info.other.Length(); i++)
	{
		String	 value = info.other.GetNth(i);

		if	(value.StartsWith(String(INFO_ORIG_ARTIST).Append(":")))    { if (edit_oartist->GetText()     != NIL) { info.other.SetNth(i, String(INFO_ORIG_ARTIST).Append(":").Append(edit_oartist->GetText()));	   modified_oartist	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_ORIG_ALBUM).Append(":")))     { if (edit_oalbum->GetText()      != NIL) { info.other.SetNth(i, String(INFO_ORIG_ALBUM).Append(":").Append(edit_oalbum->GetText()));	   modified_oalbum	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_ORIG_LYRICIST).Append(":")))  { if (edit_otextwriter->GetText() != NIL) { info.other.SetNth(i, String(INFO_ORIG_LYRICIST).Append(":").Append(edit_otextwriter->GetText()));  modified_otextwriter	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_ORIG_YEAR).Append(":")))	    { if (edit_oyear->GetText()	      != NIL) { info.other.SetNth(i, String(INFO_ORIG_YEAR).Append(":").Append(edit_oyear->GetText()));		   modified_oyear	= True; } else { info.other.RemoveNth(i); } }

		else if	(value.StartsWith(String(INFO_WEB_ARTIST).Append(":")))	    { if (edit_wartist->GetText()     != NIL) { info.other.SetNth(i, String(INFO_WEB_ARTIST).Append(":").Append(edit_wartist->GetText()));	   modified_wartist	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_WEB_PUBLISHER).Append(":")))  { if (edit_wpublisher->GetText()  != NIL) { info.other.SetNth(i, String(INFO_WEB_PUBLISHER).Append(":").Append(edit_wpublisher->GetText()));   modified_wpublisher	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_WEB_RADIO).Append(":")))	    { if (edit_wradio->GetText()      != NIL) { info.other.SetNth(i, String(INFO_WEB_RADIO).Append(":").Append(edit_wradio->GetText()));	   modified_wradio	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_WEB_SOURCE).Append(":")))	    { if (edit_wsource->GetText()     != NIL) { info.other.SetNth(i, String(INFO_WEB_SOURCE).Append(":").Append(edit_wsource->GetText()));	   modified_wsource	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_WEB_COPYRIGHT).Append(":")))  { if (edit_wcopyright->GetText()  != NIL) { info.other.SetNth(i, String(INFO_WEB_COPYRIGHT).Append(":").Append(edit_wcopyright->GetText()));   modified_wcopyright	= True; } else { info.other.RemoveNth(i); } }
		else if	(value.StartsWith(String(INFO_WEB_COMMERCIAL).Append(":"))) { if (edit_wcommercial->GetText() != NIL) { info.other.SetNth(i, String(INFO_WEB_COMMERCIAL).Append(":").Append(edit_wcommercial->GetText())); modified_wcommercial	= True; } else { info.other.RemoveNth(i); } }
	}

	if	(!modified_oartist     && edit_oartist->GetText()     != NIL) info.other.Add(String(INFO_ORIG_ARTIST).Append(":").Append(edit_oartist->GetText()));
	else if	(!modified_oalbum      && edit_oalbum->GetText()      != NIL) info.other.Add(String(INFO_ORIG_ALBUM).Append(":").Append(edit_oalbum->GetText()));
	else if	(!modified_otextwriter && edit_otextwriter->GetText() != NIL) info.other.Add(String(INFO_ORIG_LYRICIST).Append(":").Append(edit_otextwriter->GetText()));
	else if	(!modified_oyear       && edit_oyear->GetText()	      != NIL) info.other.Add(String(INFO_ORIG_YEAR).Append(":").Append(edit_oyear->GetText()));

	else if	(!modified_wartist     && edit_wartist->GetText()     != NIL) info.other.Add(String(INFO_WEB_ARTIST).Append(":").Append(edit_wartist->GetText()));
	else if	(!modified_wpublisher  && edit_wpublisher->GetText()  != NIL) info.other.Add(String(INFO_WEB_PUBLISHER).Append(":").Append(edit_wpublisher->GetText()));
	else if	(!modified_wradio      && edit_wradio->GetText()      != NIL) info.other.Add(String(INFO_WEB_RADIO).Append(":").Append(edit_wradio->GetText()));
	else if	(!modified_wsource     && edit_wsource->GetText()     != NIL) info.other.Add(String(INFO_WEB_SOURCE).Append(":").Append(edit_wsource->GetText()));
	else if	(!modified_wcopyright  && edit_wcopyright->GetText()  != NIL) info.other.Add(String(INFO_WEB_COPYRIGHT).Append(":").Append(edit_wcopyright->GetText()));
	else if	(!modified_wcommercial && edit_wcommercial->GetText() != NIL) info.other.Add(String(INFO_WEB_COMMERCIAL).Append(":").Append(edit_wcommercial->GetText()));

	onModifyTrack.Emit(track);
}
