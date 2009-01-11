 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
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
	Config	*config = Config::Get();
	I18n	*i18n = I18n::Get();

	enable_tags = config->GetIntValue("MAC", "EnableTags", 1);

	group_compression	= new GroupBox(i18n->TranslateString("Compression"), Point(7, 11), Size(203, 39));

	text_compression	= new Text(i18n->TranslateString("Compression mode:"), Point(9, 13));

	combo_compression	= new ComboBox(Point(text_compression->textSize.cx + 17, 10), Size(176 - text_compression->textSize.cx, 0));
	combo_compression->AddEntry("Fast");
	combo_compression->AddEntry("Normal");
	combo_compression->AddEntry("High");
	combo_compression->AddEntry("Extra high");
	combo_compression->AddEntry("Insane");
	combo_compression->SelectNthEntry(config->GetIntValue("MAC", "CompressionMode", 2));

	group_compression->Add(text_compression);
	group_compression->Add(combo_compression);

	group_tag		= new GroupBox(i18n->TranslateString("Tags"), Point(7, 59), Size(203, 39));

	check_tag		= new CheckBox(i18n->TranslateString("Write APE tags"), Point(10, 12), Size(183, 0), &enable_tags);

	group_tag->Add(check_tag);

	Add(group_compression);
	Add(group_tag);

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
