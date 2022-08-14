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

const String	 BoCA::ConfigureFormat::ConfigID = "Format";

BoCA::ConfigureFormat::ConfigureFormat()
{
	const Config	*config = Config::Get();

	unsignedSamples	= !config->GetIntValue(ConfigID, "Signed", True);
	applyDither	= config->GetIntValue(ConfigID, "ApplyDither", True);

	I18n		*i18n	= I18n::Get();

	i18n->SetContext("DSP::Format");

	group_format		= new GroupBox(i18n->TranslateString("Sample format"), Point(7, 11), Size(296, 94));

	text_numbers		= new Text(i18n->AddColon(i18n->TranslateString("Number format")), Point(10, 16));
	text_resolution		= new Text(i18n->AddColon(i18n->TranslateString("Sample resolution")), Point(10, 43));
	text_dither_type	= new Text(i18n->AddColon(i18n->TranslateString("Dither type")), Point(10, 41));
	text_bit		= new Text(i18n->TranslateString("bit"), Point(0, 43));
	text_bit->SetX(10 + text_bit->GetUnscaledTextWidth());
	text_bit->SetOrientation(OR_UPPERRIGHT);

	Int	 maxTextSize = Math::Max(Math::Max(text_numbers->GetUnscaledTextWidth(), text_resolution->GetUnscaledTextWidth()), text_dither_type->GetUnscaledTextWidth());

	combo_numbers		= new ComboBox(Point(17 + maxTextSize, 13), Size(263, 0));
	combo_numbers->AddEntry(i18n->TranslateString("Integer"));
	combo_numbers->AddEntry(i18n->TranslateString("Float"));
	combo_numbers->SelectNthEntry(config->GetIntValue(ConfigID, "Float", False));
	combo_numbers->onSelectEntry.Connect(&ConfigureFormat::OnChangeNumberFormat, this);

	combo_resolution	= new ComboBox(Point(17 + maxTextSize, 40), Size(263, 0));
	combo_resolution->onSelectEntry.Connect(&ConfigureFormat::OnChangeResolution, this);

	check_unsigned		= new CheckBox(i18n->TranslateString("unsigned"), Point(17 + maxTextSize, 67), Size(263, 0), &unsignedSamples);

	group_format->Add(text_numbers);
	group_format->Add(combo_numbers);

	group_format->Add(text_resolution);
	group_format->Add(combo_resolution);
	group_format->Add(text_bit);

	group_format->Add(check_unsigned);

	group_dither		= new GroupBox(i18n->TranslateString("Dithering"), Point(7, 116), Size(296, 67));

	check_dither		= new CheckBox(i18n->TranslateString("Apply dither when reducing sample resolution"), Point(10, 13), Size(276, 0), &applyDither);
	check_dither->onAction.Connect(&ConfigureFormat::OnToggleDither, this);

	combo_dither_type	= new ComboBox(Point(17 + maxTextSize, 38), Size(269, 0));
	combo_dither_type->AddEntry(i18n->TranslateString("RPDF"));
	combo_dither_type->AddEntry(i18n->TranslateString("TPDF"));
	combo_dither_type->SelectNthEntry(config->GetIntValue(ConfigID, "DitherType", 1));

	group_dither->Add(check_dither);

	group_dither->Add(text_dither_type);
	group_dither->Add(combo_dither_type);

	/* Adjust element widths.
	 */
	check_dither->SetWidth(Math::Max(276, check_dither->GetUnscaledTextWidth() + 21));

	group_format->SetWidth(check_dither->GetWidth() + 20);
	group_dither->SetWidth(check_dither->GetWidth() + 20);

	combo_numbers->SetWidth(check_dither->GetWidth() - maxTextSize - 13 - text_bit->GetUnscaledTextWidth());
	combo_resolution->SetWidth(check_dither->GetWidth() - maxTextSize - 13 - text_bit->GetUnscaledTextWidth());
	check_unsigned->SetWidth(check_dither->GetWidth() - maxTextSize - 13 - text_bit->GetUnscaledTextWidth());

	combo_dither_type->SetWidth(check_dither->GetWidth() - maxTextSize - 7);

	/* Add groups to layers.
	 */
	Add(group_format);
	Add(group_dither);

	OnChangeNumberFormat();
	OnToggleDither();

	if (combo_numbers->GetSelectedEntryNumber() == 0) combo_resolution->SelectNthEntry(config->GetIntValue(ConfigID, "Bits", 16) /	8 - 1);
	else						  combo_resolution->SelectNthEntry(config->GetIntValue(ConfigID, "Bits", 32) / 32 - 1);

	check_unsigned->SetChecked(!config->GetIntValue(ConfigID, "Signed", True));

	SetSize(Size(check_dither->GetWidth() + 34, 190));
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

	DeleteObject(group_dither);

	DeleteObject(check_dither);

	DeleteObject(text_dither_type);
	DeleteObject(combo_dither_type);
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

		check_unsigned->SetChecked(False);
		check_unsigned->Deactivate();

		group_dither->Deactivate();
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

	if (combo_resolution->GetSelectedEntryNumber() != 0) check_unsigned->SetChecked(False);

	if (combo_resolution->GetSelectedEntryNumber() == 0) check_unsigned->Activate();
	else						     check_unsigned->Deactivate();

	if (combo_resolution->GetSelectedEntryNumber() == 3) group_dither->Deactivate();
	else						     group_dither->Activate();
	
}

Void BoCA::ConfigureFormat::OnToggleDither()
{
	if (applyDither)
	{
		text_dither_type->Activate();
		combo_dither_type->Activate();
	}
	else
	{
		text_dither_type->Deactivate();
		combo_dither_type->Deactivate();
	}
}

Int BoCA::ConfigureFormat::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "Float", combo_numbers->GetSelectedEntryNumber());
	config->SetIntValue(ConfigID, "Signed", !unsignedSamples);

	if (combo_numbers->GetSelectedEntryNumber() == 0) config->SetIntValue(ConfigID, "Bits", (combo_resolution->GetSelectedEntryNumber() + 1) *  8);
	else						  config->SetIntValue(ConfigID, "Bits", (combo_resolution->GetSelectedEntryNumber() + 1) * 32);

	config->SetIntValue(ConfigID, "ApplyDither", applyDither);
	config->SetIntValue(ConfigID, "DitherType", combo_dither_type->GetSelectedEntryNumber());

	return Success();
}
