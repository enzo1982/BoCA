 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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
#include "dllinterface.h"

const String	 BoCA::ConfigureSndFile::ConfigID = "SndFileWave";

BoCA::ConfigureSndFile::ConfigureSndFile()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::SndFile");

	group_format	= new GroupBox(i18n->TranslateString("Output format"), Point(7, 11), Size(328, 39));

	text_subformat	= new Text(i18n->AddColon(i18n->TranslateString("Audio format")), Point(10, 13));
	combo_subformat	= new ComboBox(Point(text_subformat->GetUnscaledTextWidth() + 17, 10), Size(301 - text_subformat->GetUnscaledTextWidth(), 0));

	group_format->Add(text_subformat);
	group_format->Add(combo_subformat);

	Add(group_format);

	SelectFormat();

	SetSize(Size(342, 57));
}

BoCA::ConfigureSndFile::~ConfigureSndFile()
{
	DeleteObject(group_format);
	DeleteObject(text_subformat);
	DeleteObject(combo_subformat);
}

Int BoCA::ConfigureSndFile::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "SubFormat", subformats.Get(combo_subformat->GetSelectedEntry()->GetHandle()));

	return Success();
}

Void BoCA::ConfigureSndFile::SelectFormat()
{
	combo_subformat->RemoveAllEntries();

	subformats.RemoveAll();

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::SndFile");

	const Config	*config = Config::Get();

	ListEntry	*entry = combo_subformat->AddEntry(i18n->TranslateString("auto select"));

	subformats.Add(0, entry->GetHandle());

	if (config->GetIntValue(ConfigID, "SubFormat", 0) == 0) combo_subformat->SelectEntry(entry);

	int	 format = SF_FORMAT_WAV;
	int	 count	= 0;

	ex_sf_command(NIL, SFC_GET_FORMAT_SUBTYPE_COUNT, &count, sizeof(int));

	for (Int i = 0; i < count; i++)
	{
		SF_FORMAT_INFO	 format_info;

		format_info.format = i;

		ex_sf_command(NIL, SFC_GET_FORMAT_SUBTYPE, &format_info, sizeof(format_info));

		SF_INFO		 info;

		info.samplerate = 44100;
		info.channels	= 1;
		info.format	= format | format_info.format;

		if (!ex_sf_format_check(&info)) continue;

		ListEntry	*entry = NIL;

		info.channels	= 2;

		if (!ex_sf_format_check(&info)) entry = combo_subformat->AddEntry(i18n->AddBrackets(format_info.name, i18n->TranslateString("mono")));
		else				entry = combo_subformat->AddEntry(format_info.name);

		subformats.Add(format_info.format, entry->GetHandle());

		if (config->GetIntValue(ConfigID, "SubFormat", 0) == format_info.format) combo_subformat->SelectEntry(entry);
	}

	combo_subformat->Paint(SP_PAINT);
}
