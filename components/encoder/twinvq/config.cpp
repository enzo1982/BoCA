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

BoCA::ConfigureTwinVQ::ConfigureTwinVQ()
{
	Config	*config = Config::Get();
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::TwinVQ");

	group_bitrate		= new GroupBox(i18n->TranslateString("Bitrate"), Point(7, 11), Size(233, 39));

	text_bitrate		= new Text(i18n->TranslateString("Bitrate per channel:"), Point(16, 24));

	combo_bitrate		= new ComboBox(Point(24 + text_bitrate->GetUnscaledTextWidth(), 21), Size(176 - text_bitrate->GetUnscaledTextWidth(), 0));
	combo_bitrate->AddEntry("24");
	combo_bitrate->AddEntry("32");
	combo_bitrate->AddEntry("48");

	switch (config->GetIntValue("TwinVQ", "Bitrate", 48))
	{
		case 24: combo_bitrate->SelectNthEntry(0); break;
		case 32: combo_bitrate->SelectNthEntry(1); break;
		case 48: combo_bitrate->SelectNthEntry(2); break;
	}

	text_bitrate_kbps	= new Text("kbps", Point(207, 24));

	group_precand		= new GroupBox(i18n->TranslateString("Preselection"), Point(7, 62), Size(233, 39));

	text_precand		= new Text(i18n->TranslateString("Number of preselection candidates:"), Point(16, 75));

	combo_precand		= new ComboBox(Point(24 + text_precand->GetUnscaledTextWidth(), 72), Size(206 - text_precand->GetUnscaledTextWidth(), 0));
	combo_precand->AddEntry("4");
	combo_precand->AddEntry("8");
	combo_precand->AddEntry("16");
	combo_precand->AddEntry("32");

	switch (config->GetIntValue("TwinVQ", "PreselectionCandidates", 32))
	{
		case 4:	 combo_precand->SelectNthEntry(0); break;
		case 8:	 combo_precand->SelectNthEntry(1); break;
		case 16: combo_precand->SelectNthEntry(2); break;
		case 32: combo_precand->SelectNthEntry(3); break;
	}

	Add(group_bitrate);
	Add(text_bitrate);
	Add(combo_bitrate);
	Add(text_bitrate_kbps);
	Add(group_precand);
	Add(text_precand);
	Add(combo_precand);

	SetSize(Size(247, 108));
}

BoCA::ConfigureTwinVQ::~ConfigureTwinVQ()
{
	DeleteObject(group_bitrate);
	DeleteObject(text_bitrate);
	DeleteObject(combo_bitrate);
	DeleteObject(text_bitrate_kbps);
	DeleteObject(group_precand);
	DeleteObject(text_precand);
	DeleteObject(combo_precand);
}

Int BoCA::ConfigureTwinVQ::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("TwinVQ", "Bitrate", combo_bitrate->GetSelectedEntry()->GetText().ToInt());
	config->SetIntValue("TwinVQ", "PreselectionCandidates", combo_precand->GetSelectedEntry()->GetText().ToInt());

	return Success();
}
