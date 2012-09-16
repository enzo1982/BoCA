 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"
#include "dllinterface.h"

BoCA::ConfigureResample::ConfigureResample()
{
	Config	*config = Config::Get();
	I18n	*i18n = I18n::Get();

	group_converter		= new GroupBox(i18n->TranslateString("Converter"), Point(7, 11), Size(399, 59));
	group_samplerate	= new GroupBox(i18n->TranslateString("Output samplerate"), Point(7, 82), Size(399, 43));

	text_converter		= new Text(String(i18n->TranslateString("Converter")).Append(":"), Point(17, 27));

	combo_converter		= new ComboBox(Point(24 + text_converter->GetUnscaledTextWidth(), 24), Size(325, 0));

	for (Int i = 0; true; i++)
	{
		String	 name = ex_src_get_name(i);

		if (name == NIL) break;

		combo_converter->AddEntry(name);
	}

	combo_converter->SelectNthEntry(config->GetIntValue("Resample", "Converter", SRC_SINC_MEDIUM_QUALITY) - SRC_SINC_BEST_QUALITY);
	combo_converter->onSelectEntry.Connect(&ConfigureResample::SetConverter, this);

	text_description	= new Text("Description", Point(72, 49));
	text_samplerate		= new Text(String(i18n->TranslateString("Samplerate")).Append(":"), Point(17, 98));

	edit_samplerate		= new EditBox(String::FromInt(config->GetIntValue("Resample", "Samplerate", 44100)), Point(24 + text_samplerate->GetUnscaledTextWidth(), 95), Size(70, 0), 6);
	edit_samplerate->SetFlags(EDB_NUMERIC);

	list_samplerate	= new ListBox(Point(24 + text_samplerate->GetUnscaledTextWidth(), 95), Size(70, 0));
	list_samplerate->AddEntry(  "8000");
	list_samplerate->AddEntry( "11025");
	list_samplerate->AddEntry( "12000");
	list_samplerate->AddEntry( "16000");
	list_samplerate->AddEntry( "22050");
	list_samplerate->AddEntry( "24000");
	list_samplerate->AddEntry( "32000");
	list_samplerate->AddEntry( "44100");
	list_samplerate->AddEntry( "48000");
	list_samplerate->AddEntry( "64000");
	list_samplerate->AddEntry( "88200");
	list_samplerate->AddEntry( "96000");
	list_samplerate->AddEntry("128000");
	list_samplerate->AddEntry("176400");
	list_samplerate->AddEntry("192000");
	list_samplerate->AddEntry("256000");

	edit_samplerate->SetDropDownList(list_samplerate);

	SetConverter();

	Add(group_converter);
	Add(text_converter);
	Add(combo_converter);
	Add(text_description);

	Add(group_samplerate);
	Add(text_samplerate);
	Add(edit_samplerate);

	SetSize(Size(413, 171));
}

BoCA::ConfigureResample::~ConfigureResample()
{
	DeleteObject(group_converter);
	DeleteObject(text_converter);
	DeleteObject(combo_converter);
	DeleteObject(text_description);

	DeleteObject(group_samplerate);
	DeleteObject(text_samplerate);
	DeleteObject(edit_samplerate);
	DeleteObject(list_samplerate);
}

Int BoCA::ConfigureResample::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("Resample", "Converter", combo_converter->GetSelectedEntryNumber() + SRC_SINC_BEST_QUALITY);
	config->SetIntValue("Resample", "Samplerate", edit_samplerate->GetText().ToInt());

	return Success();
}

Void BoCA::ConfigureResample::SetConverter()
{
	text_description->SetText(ex_src_get_description(combo_converter->GetSelectedEntryNumber()));
}
