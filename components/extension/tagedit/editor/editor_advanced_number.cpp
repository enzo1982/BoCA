 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "editor_advanced_number.h"

BoCA::LayerEditNumber::LayerEditNumber() : Layer()
{
	text_label	= new Text("Value:", Point(7, 10));
	edit_value	= new EditBox(NIL, Point(text_label->textSize.cx + 14, 7), Size(55, 0), 8);
	edit_value->SetFlags(EDB_NUMERIC);

	Add(text_label);
	Add(edit_value);

	SetSize(Size(300, 30));
}

BoCA::LayerEditNumber::~LayerEditNumber()
{
	DeleteObject(text_label);
	DeleteObject(edit_value);
}
