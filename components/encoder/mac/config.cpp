 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"

BoCA::ConfigureMAC::ConfigureMAC()
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Encoders::Monkey's Audio");

	group_compression	= new GroupBox(i18n->TranslateString("Compression"), Point(7, 11), Size(216, 39));

	text_compression	= new Text(i18n->AddColon(i18n->TranslateString("Compression mode")), Point(9, 13));

	combo_compression	= new ComboBox(Point(text_compression->GetUnscaledTextWidth() + 17, 10), Size(189 - text_compression->GetUnscaledTextWidth(), 0));
	combo_compression->AddEntry(i18n->TranslateString("Fast"));
	combo_compression->AddEntry(i18n->TranslateString("Normal"));
	combo_compression->AddEntry(i18n->TranslateString("High"));
	combo_compression->AddEntry(i18n->TranslateString("Extra high"));
	combo_compression->AddEntry(i18n->TranslateString("Insane"));
	combo_compression->SelectNthEntry(config->GetIntValue("MAC", "CompressionMode", 2));

	group_compression->Add(text_compression);
	group_compression->Add(combo_compression);

	Add(group_compression);

	SetSize(Size(230, 57));
}

BoCA::ConfigureMAC::~ConfigureMAC()
{
	DeleteObject(group_compression);
	DeleteObject(text_compression);
	DeleteObject(combo_compression);
}

Int BoCA::ConfigureMAC::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("MAC", "CompressionMode", combo_compression->GetSelectedEntryNumber());

	return Success();
}
