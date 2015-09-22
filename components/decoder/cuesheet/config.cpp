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

	readInfoTags		= config->GetIntValue("CueSheet", "ReadInformationTags", True);
	preferCueSheets		= config->GetIntValue("CueSheet", "PreferCueSheets", True);

	group_alternatives	= new GroupBox(i18n->TranslateString("Alternative files"), Point(7, 11), Size(552, 41));

	check_alternatives	= new CheckBox(i18n->TranslateString("Look for compressed alternatives if referenced Wave files cannot be located"), Point(10, 14), Size(532, 0), &lookForAlternatives);

	group_alternatives->Add(check_alternatives);

	group_information	= new GroupBox(i18n->TranslateString("Title information"), Point(7, 64), Size(552, 64));

	check_read_tags		= new CheckBox(i18n->TranslateString("Read tags from referenced files"), Point(10, 14), Size(532, 0), &readInfoTags);
	check_read_tags->onAction.Connect(&ConfigureCueSheet::ToggleReadTags, this);

	check_prefer_cue	= new CheckBox(i18n->TranslateString("Prefer cue sheet metadata over referenced files' tags"), Point(27, 37), Size(515, 0), &preferCueSheets);

	ToggleReadTags();

	group_information->Add(check_read_tags);
	group_information->Add(check_prefer_cue);

	Add(group_alternatives);
	Add(group_information);

	SetSize(Size(566, 135));
}

BoCA::ConfigureCueSheet::~ConfigureCueSheet()
{
	DeleteObject(group_alternatives);
	DeleteObject(check_alternatives);

	DeleteObject(group_information);
	DeleteObject(check_read_tags);
	DeleteObject(check_prefer_cue);
}

Void BoCA::ConfigureCueSheet::ToggleReadTags()
{
	if (readInfoTags) check_prefer_cue->Activate();
	else		  check_prefer_cue->Deactivate();
}

Int BoCA::ConfigureCueSheet::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("CueSheet", "LookForAlternativeFiles", lookForAlternatives);

	config->SetIntValue("CueSheet", "ReadInformationTags", readInfoTags);
	config->SetIntValue("CueSheet", "PreferCueSheets", preferCueSheets);

	return Success();
}
