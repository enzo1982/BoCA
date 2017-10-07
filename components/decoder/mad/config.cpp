 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureMAD::ConfigID = "MAD";

BoCA::ConfigureMAD::ConfigureMAD()
{
	const Config	*config = Config::Get();
	I18n		*i18n	= I18n::Get();

	i18n->SetContext("Decoders::MAD");

	enable24bit = config->GetIntValue(ConfigID, "Enable24Bit", False);

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

	config->SetIntValue(ConfigID, "Enable24Bit", enable24bit);

	return Success();
}
