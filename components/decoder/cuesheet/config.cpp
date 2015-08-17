 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"

using namespace smooth::GUI::Dialogs;

BoCA::ConfigureCueSheet::ConfigureCueSheet()
{
	Config	*config = Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Decoders::CueSheet");

	lookForAlternatives	= config->GetIntValue("CueSheet", "LookForAlternativeFiles", False);

	group_alternatives	= new GroupBox(i18n->TranslateString("Alternative files"), Point(7, 11), Size(552, 41));

	check_alternatives	= new CheckBox(i18n->TranslateString("Look for compressed alternatives if referenced Wave files cannot be located"), Point(10, 14), Size(532, 0), &lookForAlternatives);

	group_alternatives->Add(check_alternatives);

	Add(group_alternatives);

	SetSize(Size(566, 59));
}

BoCA::ConfigureCueSheet::~ConfigureCueSheet()
{
	DeleteObject(group_alternatives);
	DeleteObject(check_alternatives);
}

Int BoCA::ConfigureCueSheet::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("CueSheet", "LookForAlternativeFiles", lookForAlternatives);

	return Success();
}
