 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"

BoCA::ConfigureMAC::ConfigureMAC()
{
	Point	 pos;
	Size	 size;

	Config	*config = Config::Get();
	I18n	*i18n = I18n::Get();

	enable_tags = config->GetIntValue("MAC", "EnableTags", 1);

	pos.x = 7;
	pos.y = 11;
	size.cx = 203;
	size.cy = 39;

	group_compression	= new GroupBox(i18n->TranslateString("Compression"), pos, size);

	pos.x += 9;
	pos.y += 13;

	text_compression	= new Text(i18n->TranslateString("Compression mode:"), pos);

	pos.x += (text_compression->textSize.cx + 8);
	pos.y -= 3;
	size.cx = 176 - text_compression->textSize.cx;
	size.cy = 0;

	combo_compression		= new ComboBox(pos, size);
	combo_compression->AddEntry("Fast");
	combo_compression->AddEntry("Normal");
	combo_compression->AddEntry("High");
	combo_compression->AddEntry("Extra high");
	combo_compression->AddEntry("Insane");
	combo_compression->SelectNthEntry(config->GetIntValue("MAC", "CompressionMode", 2));

	pos.x = 7;
	pos.y += 38;
	size.cx = 203;
	size.cy = 39;

	group_tag		= new GroupBox(i18n->TranslateString("Tags"), pos, size);

	pos.x += 10;
	pos.y += 12;
	size.cx = 183;
	size.cy = 0;

	check_tag		= new CheckBox(i18n->TranslateString("Write APE tags"), pos, size, &enable_tags);

	Add(group_compression);
	Add(text_compression);
	Add(combo_compression);
	Add(group_tag);
	Add(check_tag);

	SetSize(Size(217, 108));
}

BoCA::ConfigureMAC::~ConfigureMAC()
{
	DeleteObject(group_compression);
	DeleteObject(text_compression);
	DeleteObject(combo_compression);
	DeleteObject(group_tag);
	DeleteObject(check_tag);
}

Int BoCA::ConfigureMAC::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("MAC", "CompressionMode", combo_compression->GetSelectedEntryNumber());
	config->SetIntValue("MAC", "EnableTags", enable_tags);

	return Success();
}
