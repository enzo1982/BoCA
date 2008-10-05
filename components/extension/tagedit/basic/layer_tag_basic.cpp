 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "layer_tag_basic.h"

BoCA::LayerTagBasic::LayerTagBasic() : Layer("Basic")
{
	I18n	*i18n = I18n::Get();

	text_artist	= new Text(String(i18n->TranslateString("Artist")).Append(":"), Point(7, 10));
	text_title	= new Text(String(i18n->TranslateString("Title")).Append(":"), text_artist->GetPosition() + Point(0, 25));
	text_album	= new Text(String(i18n->TranslateString("Album")).Append(":"), text_title->GetPosition() + Point(0, 25));
	text_comment	= new Text(String(i18n->TranslateString("Comment")).Append(":"), text_album->GetPosition() + Point(0, 25));

	Int	 maxTextSize = Math::Max(Math::Max(text_artist->textSize.cx, text_title->textSize.cx), Math::Max(text_album->textSize.cx, text_comment->textSize.cx));

	edit_artist	= new EditBox("", text_artist->GetPosition() + Point(maxTextSize + 8, -3), Size(300, 0));
	edit_artist->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_title	= new EditBox("", text_title->GetPosition() + Point(maxTextSize + 8, -3), Size(300, 0));
	edit_title->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_album	= new EditBox("", text_album->GetPosition() + Point(maxTextSize + 8, -3), Size(300, 0));
	edit_album->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	edit_comment	= new MultiEdit("", text_comment->GetPosition() + Point(maxTextSize + 8, -3), Size(300, 50));
	edit_comment->onInput.Connect(&LayerTagBasic::OnModifyTrack, this);

	Add(text_artist);
	Add(edit_artist);
	Add(text_title);
	Add(edit_title);
	Add(text_album);
	Add(edit_album);
	Add(text_comment);
	Add(edit_comment);

	/* Initially deactivate all input fields.
	 */
	OnSelectNone();
}

BoCA::LayerTagBasic::~LayerTagBasic()
{
	DeleteObject(text_artist);
	DeleteObject(edit_artist);
	DeleteObject(text_title);
	DeleteObject(edit_title);
	DeleteObject(text_album);
	DeleteObject(edit_album);
	DeleteObject(text_comment);
	DeleteObject(edit_comment);
}

/* Called when a track is selected from the list.
 * ----
 * Copy new info to track and update input fields.
 */
Void BoCA::LayerTagBasic::OnSelectTrack(const Track &nTrack)
{
	track = nTrack;

	edit_artist->Activate();
	edit_title->Activate();
	edit_album->Activate();
	edit_comment->Activate();

	edit_artist->SetText(track.artist);
	edit_title->SetText(track.title);
	edit_album->SetText(track.album);
	edit_comment->SetText(track.comment);
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

	edit_artist->Deactivate();
	edit_title->Deactivate();
	edit_album->Deactivate();
	edit_comment->Deactivate();
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
