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

const String	 BoCA::ConfigureFormat::ConfigID = "Format";

BoCA::ConfigureFormat::ConfigureFormat()
{
	const Config	*config = Config::Get();

	unsignedSamples	= !config->GetIntValue(ConfigID, "Signed", True);

	I18n		*i18n	= I18n::Get();

	i18n->SetContext("DSP::Format");

	group_format		= new GroupBox(i18n->TranslateString("Sample format"), Point(7, 11), Size(280, 94));

	text_numbers		= new Text(i18n->AddColon(i18n->TranslateString("Number format")), Point(10, 16));
	text_resolution		= new Text(i18n->AddColon(i18n->TranslateString("Sample resolution")), Point(10, 43));
	text_bit		= new Text(i18n->TranslateString("bit"), Point(0, 43));
	text_bit->SetX(10 + text_bit->GetUnscaledTextWidth());
	text_bit->SetOrientation(OR_UPPERRIGHT);

	Int	 maxTextSize = Math::Max(text_numbers->GetUnscaledTextWidth(), text_resolution->GetUnscaledTextWidth());

	combo_numbers		= new ComboBox(Point(17 + maxTextSize, 13), Size(247 - maxTextSize - text_bit->GetUnscaledTextWidth(), 0));
	combo_numbers->AddEntry(i18n->TranslateString("Integer"));
	combo_numbers->AddEntry(i18n->TranslateString("Float"));
	combo_numbers->SelectNthEntry(config->GetIntValue(ConfigID, "Float", False));
	combo_numbers->onSelectEntry.Connect(&ConfigureFormat::OnChangeNumberFormat, this);

	combo_resolution	= new ComboBox(Point(17 + maxTextSize, 40), Size(247 - maxTextSize - text_bit->GetUnscaledTextWidth(), 0));
	combo_resolution->onSelectEntry.Connect(&ConfigureFormat::OnChangeResolution, this);

	check_unsigned		= new CheckBox(i18n->TranslateString("unsigned"), Point(17 + maxTextSize, 67), Size(247 - maxTextSize - text_bit->GetUnscaledTextWidth(), 0), &unsignedSamples);

	group_format->Add(text_numbers);
	group_format->Add(combo_numbers);

	group_format->Add(text_resolution);
	group_format->Add(combo_resolution);
	group_format->Add(text_bit);

	group_format->Add(check_unsigned);

	Add(group_format);

	OnChangeNumberFormat();

	if (combo_numbers->GetSelectedEntryNumber() == 0) combo_resolution->SelectNthEntry(config->GetIntValue(ConfigID, "Bits", 16) /	8 - 1);
	else						  combo_resolution->SelectNthEntry(config->GetIntValue(ConfigID, "Bits", 32) / 32 - 1);

	SetSize(Size(294, 112));
}

BoCA::ConfigureFormat::~ConfigureFormat()
{
	DeleteObject(group_format);

	DeleteObject(text_numbers);
	DeleteObject(combo_numbers);

	DeleteObject(text_resolution);
	DeleteObject(combo_resolution);
	DeleteObject(text_bit);

	DeleteObject(check_unsigned);
}

Void BoCA::ConfigureFormat::OnChangeNumberFormat()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("DSP::Format");

	combo_resolution->RemoveAllEntries();

	if (combo_numbers->GetSelectedEntryNumber() == 1)
	{
		combo_resolution->AddEntry("32");
		combo_resolution->AddEntry("64");

		combo_resolution->SelectNthEntry(0);
		combo_resolution->Deactivate();

		unsignedSamples = False;

		check_unsigned->Deactivate();
	}
	else
	{
		combo_resolution->AddEntry("8");
		combo_resolution->AddEntry("16");
		combo_resolution->AddEntry("24");
		combo_resolution->AddEntry("32");

		combo_resolution->SelectNthEntry(1);
		combo_resolution->Activate();

		OnChangeResolution();
	}
}

Void BoCA::ConfigureFormat::OnChangeResolution()
{
	if (combo_numbers->GetSelectedEntryNumber() == 1) return;

	unsignedSamples = False;

	if (combo_resolution->GetSelectedEntryNumber() == 0) check_unsigned->Activate();
	else						     check_unsigned->Deactivate();
}

Int BoCA::ConfigureFormat::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "Float", combo_numbers->GetSelectedEntryNumber());

	if (combo_numbers->GetSelectedEntryNumber()    == 0) config->SetIntValue(ConfigID, "Bits", (combo_resolution->GetSelectedEntryNumber() + 1) *  8);
	else						     config->SetIntValue(ConfigID, "Bits", (combo_resolution->GetSelectedEntryNumber() + 1) * 32);

	if (combo_resolution->GetSelectedEntryNumber() == 0) config->SetIntValue(ConfigID, "Signed", !unsignedSamples);

	return Success();
}
