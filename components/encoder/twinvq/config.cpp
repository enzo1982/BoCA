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

BoCA::ConfigureTwinVQ::ConfigureTwinVQ()
{
	Config	*config = Config::Get();
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::TwinVQ");

	Int	 boxWidth = Font().GetUnscaledTextSizeX("00") + 23;

	group_bitrate		= new GroupBox(i18n->TranslateString("Bitrate"), Point(7, 11), Size(233, 39));

	text_bitrate		= new Text(i18n->AddColon(i18n->TranslateString("Bitrate per channel")), Point(9, 13));
	text_bitrate_kbps	= new Text(i18n->TranslateString("%1 kbps", "Technical").Replace("%1", NIL).Replace(" ", NIL), Point(200, 13));
	text_bitrate_kbps->SetOrientation(OR_UPPERRIGHT);
	text_bitrate_kbps->SetX(text_bitrate_kbps->GetUnscaledTextWidth() + 10);

	combo_bitrate		= new ComboBox(Point(text_bitrate_kbps->GetUnscaledTextWidth() + 17 + boxWidth, 10), Size(boxWidth, 0));
	combo_bitrate->SetOrientation(OR_UPPERRIGHT);
	combo_bitrate->AddEntry("24");
	combo_bitrate->AddEntry("32");
	combo_bitrate->AddEntry("48");

	switch (config->GetIntValue("TwinVQ", "Bitrate", 48))
	{
		case 24: combo_bitrate->SelectNthEntry(0); break;
		case 32: combo_bitrate->SelectNthEntry(1); break;
		case 48: combo_bitrate->SelectNthEntry(2); break;
	}

	group_bitrate->Add(text_bitrate);
	group_bitrate->Add(combo_bitrate);
	group_bitrate->Add(text_bitrate_kbps);

	group_precand		= new GroupBox(i18n->TranslateString("Preselection"), Point(7, 62), Size(233, 39));

	text_precand		= new Text(i18n->AddColon(i18n->TranslateString("Number of preselection candidates")), Point(9, 13));

	combo_precand		= new ComboBox(Point(10 + boxWidth, 10), Size(boxWidth, 0));
	combo_precand->SetOrientation(OR_UPPERRIGHT);
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

	group_precand->Add(text_precand);
	group_precand->Add(combo_precand);

	group_bitrate->SetWidth(Math::Max(168, Math::Max(text_bitrate->GetUnscaledTextWidth() + text_bitrate_kbps->GetUnscaledTextWidth() + 33, text_precand->GetUnscaledTextWidth() + 26) + boxWidth));
	group_precand->SetWidth(Math::Max(168, Math::Max(text_bitrate->GetUnscaledTextWidth() + text_bitrate_kbps->GetUnscaledTextWidth() + 33, text_precand->GetUnscaledTextWidth() + 26) + boxWidth));

	Add(group_bitrate);
	Add(group_precand);

	SetSize(Size(Math::Max(182, Math::Max(text_bitrate->GetUnscaledTextWidth() + text_bitrate_kbps->GetUnscaledTextWidth() + 47, text_precand->GetUnscaledTextWidth() + 40) + boxWidth), 108));
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
