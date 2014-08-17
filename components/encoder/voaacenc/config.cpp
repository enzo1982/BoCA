 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"
#include "dllinterface.h"

BoCA::ConfigureVOAAC::ConfigureVOAAC()
{
	Config	*config = Config::Get();

	bitrate		= config->GetIntValue("VOAACEnc", "Bitrate", 128);
	allowID3	= config->GetIntValue("VOAACEnc", "AllowID3v2", 0);
	fileFormat	= config->GetIntValue("VOAACEnc", "MP4Container", 1);
	fileExtension	= config->GetIntValue("VOAACEnc", "MP4FileExtension", 0);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::AAC::Format");

	group_id3v2		= new GroupBox(i18n->TranslateString("Tags"), Point(7, 143), Size(281, 90));

	check_id3v2		= new CheckBox(i18n->TranslateString("Allow ID3v2 tags in AAC files"), Point(10, 13), Size(200, 0), &allowID3);
	check_id3v2->SetWidth(check_id3v2->GetUnscaledTextWidth() + 20);

	text_note		= new Text(i18n->AddColon(i18n->TranslateString("Note")), Point(10, 38));
	text_id3v2		= new Text(i18n->TranslateString("Some players may have problems playing AAC\nfiles with ID3 tags attached. Please use this option only\nif you are sure that your player can handle these tags."), Point(text_note->GetUnscaledTextWidth() + 12, 38));

	group_id3v2->SetSize(Size(Math::Max(240, text_note->GetUnscaledTextWidth() + text_id3v2->GetUnscaledTextWidth() + 22), Math::Max(text_note->GetUnscaledTextHeight(), text_id3v2->GetUnscaledTextHeight()) + 48));

	group_id3v2->Add(check_id3v2);
	group_id3v2->Add(text_note);
	group_id3v2->Add(text_id3v2);

	i18n->SetContext("Encoders::AAC::Quality");

	group_bitrate		= new GroupBox(i18n->TranslateString("Bitrate"), Point(7, 11), Size(group_id3v2->GetWidth(), 43));

	text_bitrate		= new Text(i18n->AddColon(i18n->TranslateString("Bitrate")), Point(10, 15));

	slider_bitrate		= new Slider(Point(text_bitrate->GetUnscaledTextSize().cx + 17, 13), Size(group_bitrate->GetWidth() - 91 - text_bitrate->GetUnscaledTextSize().cx, 0), OR_HORZ, &bitrate, 16, 192);
	slider_bitrate->onValueChange.Connect(&ConfigureVOAAC::SetBitrate, this);

	edit_bitrate		= new EditBox(NIL, Point(group_bitrate->GetWidth() - 66, 12), Size(25, 0), 3);
	edit_bitrate->SetFlags(EDB_NUMERIC);
	edit_bitrate->onInput.Connect(&ConfigureVOAAC::SetBitrateByEditBox, this);

	text_bitrate_kbps	= new Text(i18n->TranslateString("%1 kbps", "Technical").Replace("%1", NIL).Replace(" ", NIL), Point(group_bitrate->GetWidth() - 34, 15));

	group_bitrate->Add(text_bitrate);
	group_bitrate->Add(slider_bitrate);
	group_bitrate->Add(edit_bitrate);
	group_bitrate->Add(text_bitrate_kbps);

	i18n->SetContext("Encoders::AAC::Format");

	group_mp4		= new GroupBox(i18n->TranslateString("File format"), Point(7, 66), Size(group_id3v2->GetWidth() / 2 - 4, 65));

	option_mp4		= new OptionBox("MP4", Point(10, 13), Size(group_mp4->GetWidth() - 21, 0), &fileFormat, 1);
	option_mp4->onAction.Connect(&ConfigureVOAAC::SetFileFormat, this);

	if (mp4v2dll == NIL)
	{
		option_mp4->Deactivate();

		fileFormat = 0;
	}

	option_aac		= new OptionBox("AAC", Point(10, 38), Size(group_mp4->GetWidth() - 21, 0), &fileFormat, 0);
	option_aac->onAction.Connect(&ConfigureVOAAC::SetFileFormat, this);

	group_mp4->Add(option_mp4);
	group_mp4->Add(option_aac);

	group_extension		= new GroupBox(i18n->TranslateString("File extension"), Point(group_mp4->GetWidth() + 15 + (group_id3v2->GetWidth() % 2), 66), Size(group_id3v2->GetWidth() / 2 - 4, 65));

	option_extension_m4a	= new OptionBox(".m4a", Point(10, 13),					Size(group_extension->GetWidth() / 2 - 14, 0), &fileExtension, 0);
	option_extension_m4b	= new OptionBox(".m4b", Point(10, 38),					Size(group_extension->GetWidth() / 2 - 14, 0), &fileExtension, 1);
	option_extension_m4r	= new OptionBox(".m4r", Point(group_extension->GetWidth() / 2 + 4, 13), Size(group_extension->GetWidth() / 2 - 14, 0), &fileExtension, 2);
	option_extension_mp4	= new OptionBox(".mp4", Point(group_extension->GetWidth() / 2 + 4, 38), Size(group_extension->GetWidth() / 2 - 14, 0), &fileExtension, 3);

	group_extension->Add(option_extension_m4a);
	group_extension->Add(option_extension_m4b);
	group_extension->Add(option_extension_m4r);
	group_extension->Add(option_extension_mp4);

	SetBitrate();
	SetFileFormat();

	Add(group_bitrate);
	Add(group_mp4);
	Add(group_extension);
	Add(group_id3v2);

	SetSize(group_id3v2->GetSize() + Size(14, 150));
}

BoCA::ConfigureVOAAC::~ConfigureVOAAC()
{
	DeleteObject(group_bitrate);
	DeleteObject(text_bitrate);
	DeleteObject(slider_bitrate);
	DeleteObject(edit_bitrate);
	DeleteObject(text_bitrate_kbps);
	DeleteObject(group_mp4);
	DeleteObject(option_mp4);
	DeleteObject(option_aac);
	DeleteObject(group_extension);
	DeleteObject(option_extension_m4a);
	DeleteObject(option_extension_m4b);
	DeleteObject(option_extension_m4r);
	DeleteObject(option_extension_mp4);
	DeleteObject(group_id3v2);
	DeleteObject(check_id3v2);
	DeleteObject(text_note);
	DeleteObject(text_id3v2);
}

Int BoCA::ConfigureVOAAC::SaveSettings()
{
	Config	*config = Config::Get();

	if (bitrate <  16) bitrate =  16;
	if (bitrate > 192) bitrate = 192;

	config->SetIntValue("VOAACEnc", "Bitrate", bitrate);
	config->SetIntValue("VOAACEnc", "AllowID3v2", allowID3);
	config->SetIntValue("VOAACEnc", "MP4Container", fileFormat);
	config->SetIntValue("VOAACEnc", "MP4FileExtension", fileExtension);

	return Success();
}

Void BoCA::ConfigureVOAAC::SetBitrate()
{
	edit_bitrate->SetText(String::FromInt(bitrate));
}

Void BoCA::ConfigureVOAAC::SetBitrateByEditBox()
{
	slider_bitrate->SetValue(edit_bitrate->GetText().ToInt());
}

Void BoCA::ConfigureVOAAC::SetFileFormat()
{
	if (fileFormat == 1)	// MP4 container
	{
		group_id3v2->Deactivate();

		group_extension->Activate();
	}
	else			// raw AAC file format
	{
		group_id3v2->Activate();

		group_extension->Deactivate();
	}
}
