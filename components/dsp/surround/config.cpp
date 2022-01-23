 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureSurround::ConfigID = "Surround";

BoCA::ConfigureSurround::ConfigureSurround()
{
	const Config	*config = Config::Get();

	Int	 channelSetup = config->GetIntValue(ConfigID, "ChannelSetup", 6);

	redirectBass = config->GetIntValue(ConfigID, "RedirectBass", True);

	I18n		*i18n	= I18n::Get();

	i18n->SetContext("DSP::Surround");

	group_output	= new GroupBox(i18n->TranslateString("Output channels"), Point(7, 11), Size(250, 67));

	check_lfe	= new CheckBox(i18n->TranslateString("Redirect bass content to LFE channel"), Point(10, 40), Size(230, 0), &redirectBass);

	text_channels	= new Text(i18n->AddColon(i18n->TranslateString("Channel configuration")), Point(10, 16));

	combo_channels	= new ComboBox(Point(17 + text_channels->GetUnscaledTextWidth(), 13), Size(223 - text_channels->GetUnscaledTextWidth(), 0));
	combo_channels->AddEntry("2.1");
	combo_channels->AddEntry("3.1");
	combo_channels->AddEntry("4.1");
	combo_channels->AddEntry("5.1");
	combo_channels->AddEntry("6.1");
	combo_channels->AddEntry("7.1");
	combo_channels->SelectNthEntry(channelSetup - 3);

	group_output->Add(text_channels);
	group_output->Add(combo_channels);
	group_output->Add(check_lfe);

	Add(group_output);

	SetSize(Size(264, 85));
}

BoCA::ConfigureSurround::~ConfigureSurround()
{
	DeleteObject(group_output);

	DeleteObject(text_channels);
	DeleteObject(combo_channels);

	DeleteObject(check_lfe);
}

Int BoCA::ConfigureSurround::SaveSettings()
{
	Config	*config = Config::Get();

	Int	 entry = combo_channels->GetSelectedEntryNumber();

	config->SetIntValue(ConfigID, "ChannelSetup", entry + 3);
	config->SetIntValue(ConfigID, "RedirectBass", redirectBass);

	return Success();
}
