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

BoCA::ConfigureVorbis::ConfigureVorbis()
{
	Point	 pos;
	Size	 size;

	Config	*config = Config::Get();

	mode		= config->GetIntValue("Vorbis", "Mode", 0);
	quality		= config->GetIntValue("Vorbis", "Quality", 60);
	abr		= config->GetIntValue("Vorbis", "Bitrate", 192);

	I18n	*i18n = I18n::Get();

	layer_vbr	= new Layer();
	layer_abr	= new Layer();

	pos.x = 7;
	pos.y = 11;
	size.cx = 344;
	size.cy = 43;

	group_mode		= new GroupBox(i18n->TranslateString("Encoding Mode"), pos, size);

	size.cy = 84;

	group_mode2		= new GroupBox(i18n->TranslateString("Encoding Mode"), pos, size);

	pos.x = 17;
	pos.y = 24;
	size.cx = 157;
	size.cy = 0;

	option_mode_vbr		= new OptionBox(String("VBR (").Append(i18n->TranslateString("Variable Bitrate")).Append(")"), pos, size, &mode, 0);
	option_mode_vbr->onAction.Connect(&ConfigureVorbis::SetMode, this);

	pos.x += 166;

	option_mode_abr		= new OptionBox(String("ABR (").Append(i18n->TranslateString("Average Bitrate")).Append(")"), pos, size, &mode, 1);
	option_mode_abr->onAction.Connect(&ConfigureVorbis::SetMode, this);

	pos.x = 19;
	pos.y += 43;

	text_quality		= new Text(String(i18n->TranslateString("Quality")).Append(":"), pos);

	pos.x += (text_quality->textSize.cx + 8);
	pos.y -= 2;
	size.cx = 283 - text_quality->textSize.cx;

	slider_quality		= new Slider(pos, size, OR_HORZ, &quality, -20, 100);
	slider_quality->onValueChange.Connect(&ConfigureVorbis::SetQuality, this);

	pos.x += (size.cx + 7);
	pos.y += 2;

	text_quality_value	= new Text("", pos);

	pos.x = 19;

	text_abr		= new Text(String(i18n->TranslateString("Average Bitrate")).Append(":"), pos);

	pos.x += (text_abr->textSize.cx + 8);
	pos.y -= 2;
	size.cx = 248 - text_abr->textSize.cx;

	slider_abr		= new Slider(pos, size, OR_HORZ, &abr, 45, 500);
	slider_abr->onValueChange.Connect(&ConfigureVorbis::SetBitrate, this);

	pos.x += (size.cx + 8);
	pos.y -= 1;
	size.cx = 25;

	edit_abr		= new EditBox("", pos, size, 3);
	edit_abr->SetFlags(EDB_NUMERIC);
	edit_abr->onInput.Connect(&ConfigureVorbis::SetBitrateByEditBox, this);

	pos.x += 32;
	pos.y += 3;

	text_abr_kbps		= new Text("kbps", pos);

	SetQuality();
	SetBitrate();

	Add(group_mode);
	Add(group_mode2);
	Add(option_mode_vbr);
	Add(option_mode_abr);

	layer_vbr->Add(slider_quality);
	layer_vbr->Add(text_quality);
	layer_vbr->Add(text_quality_value);

	layer_abr->Add(slider_abr);
	layer_abr->Add(text_abr);
	layer_abr->Add(edit_abr);
	layer_abr->Add(text_abr_kbps);

	Add(layer_vbr);
	Add(layer_abr);

	switch (mode)
	{
		case 0:
			layer_abr->Hide();
			break;
		case 1:
			layer_vbr->Hide();
			break;
	}

	SetSize(Size(358, 102));
}

BoCA::ConfigureVorbis::~ConfigureVorbis()
{
	DeleteObject(layer_abr);
	DeleteObject(layer_vbr);

	DeleteObject(group_mode);
	DeleteObject(group_mode2);
	DeleteObject(option_mode_abr);
	DeleteObject(option_mode_vbr);

	DeleteObject(slider_quality);
	DeleteObject(text_quality);
	DeleteObject(text_quality_value);

	DeleteObject(slider_abr);
	DeleteObject(text_abr);
	DeleteObject(edit_abr);
	DeleteObject(text_abr_kbps);
}

Int BoCA::ConfigureVorbis::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("Vorbis", "Mode", mode);
	config->SetIntValue("Vorbis", "Quality", quality);
	config->SetIntValue("Vorbis", "Bitrate", abr);

	return Success();
}

Void BoCA::ConfigureVorbis::SetMode()
{
	switch (mode)
	{
		case 0:
			layer_abr->Hide();
			layer_vbr->Show();
			break;
		case 1:
			layer_vbr->Hide();
			layer_abr->Show();
			break;
	}
}

Void BoCA::ConfigureVorbis::SetQuality()
{
	String	 txt = String::FromFloat(9 - ((double) quality) / 100);

	if (quality %  10 == 0) txt.Append(".0");
	if (quality % 100 == 0) txt.Append(".0");

	text_quality_value->SetText(txt);
}

Void BoCA::ConfigureVorbis::SetBitrate()
{
	edit_abr->SetText(String::FromInt(abr));
}

Void BoCA::ConfigureVorbis::SetBitrateByEditBox()
{
	slider_abr->SetValue(edit_abr->GetText().ToInt());
}
