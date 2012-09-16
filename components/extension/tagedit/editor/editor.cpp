 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "editor.h"

BoCA::Editor::Editor(const String &title) : Layer(title)
{
	allowTrackChangeByArrowKey.Connect(True);
}

BoCA::Editor::~Editor()
{
}

Void BoCA::Editor::OnSelectTrack(const Track &track)
{
}

Void BoCA::Editor::OnSelectAlbum(const Track &track)
{
}

Void BoCA::Editor::OnSelectNone()
{
}
