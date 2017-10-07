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
#include "dllinterface.h"

const String	 BoCA::ConfigureSndFile::ConfigID = "SndFile";

BoCA::ConfigureSndFile::ConfigureSndFile()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::SndFile");

	group_format	= new GroupBox(i18n->TranslateString("Output format"), Point(7, 11), Size(328, 65));

	text_format	= new Text(i18n->AddColon(i18n->TranslateString("File format")), Point(10, 13));

	combo_format	= new ComboBox(Point(88, 10), Size(230, 0));
	combo_format->onSelectEntry.Connect(&ConfigureSndFile::SelectFormat, this);

	text_subformat	= new Text(i18n->AddColon(i18n->TranslateString("Audio format")), Point(10, 39));

	combo_subformat	= new ComboBox(Point(88, 36), Size(230, 0));

	Int	 maxTextSize = Math::Max(text_format->GetUnscaledTextWidth(), text_subformat->GetUnscaledTextWidth());

	combo_format->SetX(maxTextSize + 17);
	combo_format->SetWidth(301 - maxTextSize);
	combo_subformat->SetX(maxTextSize + 17);
	combo_subformat->SetWidth(301 - maxTextSize);

	group_format->Add(text_format);
	group_format->Add(combo_format);
	group_format->Add(text_subformat);
	group_format->Add(combo_subformat);

	Add(group_format);

	FillFormats();

	SetSize(Size(342, 83));
}

BoCA::ConfigureSndFile::~ConfigureSndFile()
{
	DeleteObject(group_format);
	DeleteObject(text_format);
	DeleteObject(combo_format);
	DeleteObject(text_subformat);
	DeleteObject(combo_subformat);
}

Int BoCA::ConfigureSndFile::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "Format", formats.Get(combo_format->GetSelectedEntry()->GetHandle()));
	config->SetIntValue(ConfigID, "SubFormat", subformats.Get(combo_subformat->GetSelectedEntry()->GetHandle()));

	return Success();
}

Void BoCA::ConfigureSndFile::FillFormats()
{
	const Config	*config = Config::Get();
	int		 count	= 0;

	ex_sf_command(NIL, SFC_GET_FORMAT_MAJOR_COUNT, &count, sizeof(int));

	for (Int i = 0; i < count; i++)
	{
		SF_FORMAT_INFO	 format_info;

		format_info.format = i;

		ex_sf_command(NIL, SFC_GET_FORMAT_MAJOR, &format_info, sizeof(format_info));

		if (format_info.format != SF_FORMAT_WAV	&& format_info.format != SF_FORMAT_AIFF	 &&
		    format_info.format != SF_FORMAT_AU	&& format_info.format != SF_FORMAT_PAF	 &&
		    format_info.format != SF_FORMAT_SVX	&& format_info.format != SF_FORMAT_IRCAM &&
		    format_info.format != SF_FORMAT_VOC	&& format_info.format != SF_FORMAT_W64	 &&
		    format_info.format != SF_FORMAT_PVF	&& format_info.format != SF_FORMAT_HTK	 &&
		    format_info.format != SF_FORMAT_CAF	&& format_info.format != SF_FORMAT_AVR	 &&
		    format_info.format != SF_FORMAT_WVE	&& format_info.format != SF_FORMAT_RF64) continue;

		ListEntry	*entry = combo_format->AddEntry(format_info.name);

		formats.Add(format_info.format, entry->GetHandle());

#ifdef __APPLE__
		if (config->GetIntValue(ConfigID, "Format", SF_FORMAT_AIFF) == format_info.format) combo_format->SelectEntry(entry);
#else
		if (config->GetIntValue(ConfigID, "Format", SF_FORMAT_WAV) == format_info.format) combo_format->SelectEntry(entry);
#endif
	}

	SelectFormat();
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

	int	 format = formats.Get(combo_format->GetSelectedEntry()->GetHandle());
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
