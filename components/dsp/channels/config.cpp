 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureChannels::ConfigID = "Channels";

BoCA::ConfigureChannels::ConfigureChannels()
{
	const Config	*config = Config::Get();

	Int	 channels = config->GetIntValue(ConfigID, "Channels", 2);

	swapChannels = config->GetIntValue(ConfigID, "SwapChannels", False);

	I18n		*i18n	= I18n::Get();

	i18n->SetContext("DSP::Channels");

	group_output		= new GroupBox(i18n->TranslateString("Output channels"), Point(7, 11), Size(250, 67));

	check_swap		= new CheckBox(i18n->TranslateString("Switch stereo channels"), Point(10, 40), Size(230, 0), &swapChannels);

	text_channels		= new Text(i18n->AddColon(i18n->TranslateString("Channel configuration")), Point(10, 16));

	combo_channels		= new ComboBox(Point(17 + text_channels->GetUnscaledTextWidth(), 13), Size(223 - text_channels->GetUnscaledTextWidth(), 0));
	combo_channels->onSelectEntry.Connect(&ConfigureChannels::OnSelectChannels, this);
	combo_channels->AddEntry(i18n->TranslateString("Mono"));
	combo_channels->AddEntry(i18n->TranslateString("Stereo"));
	combo_channels->AddEntry("2.1");
	combo_channels->AddEntry("4.0");
	combo_channels->AddEntry("5.1");
	combo_channels->AddEntry("7.1");
	combo_channels->SelectNthEntry(channels <= 4 ? channels - 1 : channels / 2 + 1);

	group_output->Add(text_channels);
	group_output->Add(combo_channels);
	group_output->Add(check_swap);

	Add(group_output);

	SetSize(Size(264, 85));
}

BoCA::ConfigureChannels::~ConfigureChannels()
{
	DeleteObject(group_output);

	DeleteObject(text_channels);
	DeleteObject(combo_channels);

	DeleteObject(check_swap);
}

Int BoCA::ConfigureChannels::SaveSettings()
{
	Config	*config = Config::Get();

	Int	 entry = combo_channels->GetSelectedEntryNumber();

	config->SetIntValue(ConfigID, "Channels", entry <= 3 ? entry + 1 : (entry - 1) * 2);
	config->SetIntValue(ConfigID, "SwapChannels", swapChannels);

	return Success();
}

Void BoCA::ConfigureChannels::OnSelectChannels()
{
	if (combo_channels->GetSelectedEntryNumber() == 1) check_swap->Activate();
	else						   check_swap->Deactivate();
}
