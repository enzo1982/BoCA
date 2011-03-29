 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"

BoCA::ConfigureMAD::ConfigureMAD()
{
	Config	*config = Config::Get();
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Decoders::MAD");

	enable24bit = config->GetIntValue("MAD", "Enable24Bit", False);

	group_decoding		= new GroupBox(i18n->TranslateString("Decoding"), Point(7, 11), Size(200, 41));

	check_enable24bit	= new CheckBox(i18n->TranslateString("Enable 24 bit decoding"), Point(10, 14), Size(180, 0), &enable24bit);

	group_decoding->Add(check_enable24bit);

	Add(group_decoding);

	SetSize(Size(214, 169));
}

BoCA::ConfigureMAD::~ConfigureMAD()
{
	DeleteObject(group_decoding);
	DeleteObject(check_enable24bit);
}

Int BoCA::ConfigureMAD::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("MAD", "Enable24Bit", enable24bit);

	return Success();
}
