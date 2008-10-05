 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
	Point	 pos;
	Size	 size;

	Config	*config = Config::Get();
	I18n	*i18n = I18n::Get();

	pos.x = 7;
	pos.y = 11;
	size.cx = 399;
	size.cy = 59;

	group_converter		= new GroupBox(i18n->TranslateString("Converter"), pos, size);

	pos.y += 71;
	size.cy = 43;

	group_samplerate	= new GroupBox(i18n->TranslateString("Output samplerate"), pos, size);

	pos.x = 17;
	pos.y = 27;

	text_converter		= new Text(String(i18n->TranslateString("Converter")).Append(":"), pos);

	pos.x += text_converter->textSize.cx + 7;
	pos.y -= 3;
	size.cx = 325;
	size.cy = 0;

	combo_converter		= new ComboBox(pos, size);
	combo_converter->SelectNthEntry(config->GetIntValue("Resample", "Converter", SRC_SINC_BEST_QUALITY));
	combo_converter->onSelectEntry.Connect(&ConfigureResample::SetConverter, this);

	for (Int i = 0; true; i++)
	{
		String	 name = ex_src_get_name(i);

		if (name == NIL) break;

		combo_converter->AddEntry(name);
	}

	pos.x = 72;
	pos.y = 49;

	text_description	= new Text("Description", pos);

	pos.x = 17;
	pos.y = 98;

	text_samplerate		= new Text(String(i18n->TranslateString("Samplerate")).Append(":"), pos);

	pos.x += text_samplerate->textSize.cx + 7;
	pos.y -= 3;
	size.cx = 70;
	size.cy = 0;

	edit_samplerate		= new EditBox(String::FromInt(config->GetIntValue("Resample", "Samplerate", 44100)), pos, size, 6);
	edit_samplerate->SetFlags(EDB_NUMERIC);

	list_samplerate	= new ListBox(pos, size);
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

	config->SetIntValue("Resample", "Converter", combo_converter->GetSelectedEntryNumber());
	config->SetIntValue("Resample", "Samplerate", edit_samplerate->GetText().ToInt());

	return Success();
}

Void BoCA::ConfigureResample::SetConverter()
{
	text_description->SetText(ex_src_get_description(combo_converter->GetSelectedEntryNumber()));
}
