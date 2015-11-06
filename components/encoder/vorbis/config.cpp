 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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

BoCA::ConfigureVorbis::ConfigureVorbis()
{
	Config	*config = Config::Get();

	mode		= config->GetIntValue("Vorbis", "Mode", 0);
	fileExtension	= config->GetIntValue("Vorbis", "FileExtension", 0);
	quality		= config->GetIntValue("Vorbis", "Quality", 60);
	setABRMin	= config->GetIntValue("Vorbis", "SetMinBitrate", False);
	abrMin		= config->GetIntValue("Vorbis", "MinBitrate", 32);
	setABRNom	= config->GetIntValue("Vorbis", "SetBitrate", True);
	abrNom		= config->GetIntValue("Vorbis", "Bitrate", 192);
	setABRMax	= config->GetIntValue("Vorbis", "SetMaxBitrate", False);
	abrMax		= config->GetIntValue("Vorbis", "MaxBitrate", 320);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::Vorbis");

	group_mode		= new GroupBox(i18n->TranslateString("Encoding Mode"), Point(7, 11), Size(168, 65));

	option_mode_vbr		= new OptionBox(i18n->AddBrackets("VBR", i18n->TranslateString("Variable Bitrate")), Point(10, 13), Size(148, 0), &mode, 0);
	option_mode_vbr->onAction.Connect(&ConfigureVorbis::SetMode, this);

	option_mode_abr		= new OptionBox(i18n->AddBrackets("ABR", i18n->TranslateString("Average Bitrate")), Point(10, 38), Size(148, 0), &mode, 1);
	option_mode_abr->onAction.Connect(&ConfigureVorbis::SetMode, this);

	group_mode->Add(option_mode_vbr);
	group_mode->Add(option_mode_abr);

	group_extension		= new GroupBox(i18n->TranslateString("File extension"), Point(183, 11), Size(168, 65));

	option_extension_ogg	= new OptionBox(".ogg", Point(10, 13), Size(148, 0), &fileExtension, 0);
	option_extension_oga	= new OptionBox(".oga", Point(10, 38), Size(148, 0), &fileExtension, 1);

	group_extension->Add(option_extension_ogg);
	group_extension->Add(option_extension_oga);

	group_quality		= new GroupBox(i18n->TranslateString("Quality"), Point(7, 88), Size(344, 42));

	text_quality		= new Text(i18n->AddColon(i18n->TranslateString("Quality")), Point(10, 16));

	slider_quality		= new Slider(Point(text_quality->GetUnscaledTextWidth() + 18, 14), Size(289 - text_quality->GetUnscaledTextWidth(), 0), OR_HORZ, &quality, -20, 100);
	slider_quality->onValueChange.Connect(&ConfigureVorbis::SetQuality, this);

	text_quality_value	= new Text(NIL, Point(313, 16));

	group_quality->Add(text_quality);
	group_quality->Add(slider_quality);
	group_quality->Add(text_quality_value);

	group_bitrate		= new GroupBox(i18n->TranslateString("Bitrate"), Point(7, 88), Size(344, 96));

	check_abrmin		= new CheckBox(i18n->AddColon(i18n->TranslateString("Minimum bitrate")), Point(10, 14), Size(), &setABRMin);
	check_abrmin->onAction.Connect(&ConfigureVorbis::ToggleABRMin, this);

	slider_abrmin		= new Slider(Point(38, 14), Size(228, 0), OR_HORZ, &abrMin, 32, 320);
	slider_abrmin->onValueChange.Connect(&ConfigureVorbis::SetABRMin, this);

	edit_abrmin		= new EditBox(NIL, Point(279, 13), Size(25, 0), 3);
	edit_abrmin->SetFlags(EDB_NUMERIC);
	edit_abrmin->onInput.Connect(&ConfigureVorbis::SetABRMinByEditBox, this);

	text_abrmin_kbps	= new Text(i18n->TranslateString("%1 kbps", "Technical").Replace("%1", NIL).Replace(" ", NIL), Point(311, 16));

	check_abrnom		= new CheckBox(i18n->AddColon(i18n->TranslateString("Average bitrate")), Point(10, 41), Size(), &setABRNom);
	check_abrnom->onAction.Connect(&ConfigureVorbis::ToggleABRNom, this);

	slider_abrnom		= new Slider(Point(38, 41), Size(228, 0), OR_HORZ, &abrNom, 32, 320);
	slider_abrnom->onValueChange.Connect(&ConfigureVorbis::SetABRNom, this);

	edit_abrnom		= new EditBox(NIL, Point(279, 40), Size(25, 0), 3);
	edit_abrnom->SetFlags(EDB_NUMERIC);
	edit_abrnom->onInput.Connect(&ConfigureVorbis::SetABRNomByEditBox, this);

	text_abrnom_kbps	= new Text(i18n->TranslateString("%1 kbps", "Technical").Replace("%1", NIL).Replace(" ", NIL), Point(311, 43));

	check_abrmax		= new CheckBox(i18n->AddColon(i18n->TranslateString("Maximum bitrate")), Point(10, 68), Size(), &setABRMax);
	check_abrmax->onAction.Connect(&ConfigureVorbis::ToggleABRMax, this);

	slider_abrmax		= new Slider(Point(38, 68), Size(228, 0), OR_HORZ, &abrMax, 32, 320);
	slider_abrmax->onValueChange.Connect(&ConfigureVorbis::SetABRMax, this);

	edit_abrmax		= new EditBox(NIL, Point(279, 67), Size(25, 0), 3);
	edit_abrmax->SetFlags(EDB_NUMERIC);
	edit_abrmax->onInput.Connect(&ConfigureVorbis::SetABRMaxByEditBox, this);

	text_abrmax_kbps	= new Text(i18n->TranslateString("%1 kbps", "Technical").Replace("%1", NIL).Replace(" ", NIL), Point(311, 70));

	Int	 maxTextSize = Math::Max(Math::Max(check_abrmin->GetUnscaledTextWidth(), check_abrnom->GetUnscaledTextWidth()), check_abrmax->GetUnscaledTextWidth());

	check_abrmin->SetWidth(maxTextSize + 20);
	check_abrnom->SetWidth(maxTextSize + 20);
	check_abrmax->SetWidth(maxTextSize + 20);

	slider_abrmin->SetX(maxTextSize + 38);
	slider_abrnom->SetX(maxTextSize + 38);
	slider_abrmax->SetX(maxTextSize + 38);

	slider_abrmin->SetWidth(233 - maxTextSize);
	slider_abrnom->SetWidth(233 - maxTextSize);
	slider_abrmax->SetWidth(233 - maxTextSize);

	group_bitrate->Add(check_abrmin);
	group_bitrate->Add(slider_abrmin);
	group_bitrate->Add(edit_abrmin);
	group_bitrate->Add(text_abrmin_kbps);
	group_bitrate->Add(check_abrnom);
	group_bitrate->Add(slider_abrnom);
	group_bitrate->Add(edit_abrnom);
	group_bitrate->Add(text_abrnom_kbps);
	group_bitrate->Add(check_abrmax);
	group_bitrate->Add(slider_abrmax);
	group_bitrate->Add(edit_abrmax);
	group_bitrate->Add(text_abrmax_kbps);

	SetQuality();

	ToggleABRMin();
	ToggleABRNom();
	ToggleABRMax();

	SetABRMin();
	SetABRNom();
	SetABRMax();

	Add(group_mode);
	Add(group_extension);
	Add(group_quality);
	Add(group_bitrate);

	switch (mode)
	{
		case 0:
			group_bitrate->Hide();
			break;
		case 1:
			group_quality->Hide();
			break;
	}

	SetSize(Size(358, 191));
}

BoCA::ConfigureVorbis::~ConfigureVorbis()
{
	DeleteObject(group_mode);
	DeleteObject(option_mode_abr);
	DeleteObject(option_mode_vbr);

	DeleteObject(group_extension);
	DeleteObject(option_extension_ogg);
	DeleteObject(option_extension_oga);

	DeleteObject(group_quality);
	DeleteObject(slider_quality);
	DeleteObject(text_quality);
	DeleteObject(text_quality_value);

	DeleteObject(group_bitrate);
	DeleteObject(check_abrmin);
	DeleteObject(slider_abrmin);
	DeleteObject(edit_abrmin);
	DeleteObject(text_abrmin_kbps);
	DeleteObject(check_abrnom);
	DeleteObject(slider_abrnom);
	DeleteObject(edit_abrnom);
	DeleteObject(text_abrnom_kbps);
	DeleteObject(check_abrmax);
	DeleteObject(slider_abrmax);
	DeleteObject(edit_abrmax);
	DeleteObject(text_abrmax_kbps);
}

Int BoCA::ConfigureVorbis::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("Vorbis", "Mode", mode);

	config->SetIntValue("Vorbis", "FileExtension", fileExtension);

	config->SetIntValue("Vorbis", "Quality", quality);

	config->SetIntValue("Vorbis", "SetMinBitrate", setABRMin);
	config->SetIntValue("Vorbis", "MinBitrate", abrMin);

	config->SetIntValue("Vorbis", "SetBitrate", setABRNom);
	config->SetIntValue("Vorbis", "Bitrate", abrNom);

	config->SetIntValue("Vorbis", "SetMaxBitrate", setABRMax);
	config->SetIntValue("Vorbis", "MaxBitrate", abrMax);

	return Success();
}

Void BoCA::ConfigureVorbis::SetMode()
{
	switch (mode)
	{
		case 0:
			group_bitrate->Hide();
			group_quality->Show();
			break;
		case 1:
			group_quality->Hide();
			group_bitrate->Show();
			break;
	}
}

Void BoCA::ConfigureVorbis::SetQuality()
{
	String	 txt = String::FromFloat(((double) quality) / 10);

	if (quality % 10 == 0) txt.Append(".0");

	text_quality_value->SetText(txt);
}

Void BoCA::ConfigureVorbis::ToggleABRMin()
{
	if (!setABRMin) { edit_abrmin->Deactivate(); slider_abrmin->Deactivate(); }
	else		{ edit_abrmin->Activate();   slider_abrmin->Activate();	  }
}

Void BoCA::ConfigureVorbis::SetABRMin()
{
	edit_abrmin->SetText(String::FromInt(abrMin));
}

Void BoCA::ConfigureVorbis::SetABRMinByEditBox()
{
	slider_abrmin->SetValue(edit_abrmin->GetText().ToInt());
}

Void BoCA::ConfigureVorbis::ToggleABRNom()
{
	if (!setABRNom) { edit_abrnom->Deactivate(); slider_abrnom->Deactivate(); }
	else		{ edit_abrnom->Activate();   slider_abrnom->Activate();	  }
}

Void BoCA::ConfigureVorbis::SetABRNom()
{
	edit_abrnom->SetText(String::FromInt(abrNom));
}

Void BoCA::ConfigureVorbis::SetABRNomByEditBox()
{
	slider_abrnom->SetValue(edit_abrnom->GetText().ToInt());
}

Void BoCA::ConfigureVorbis::ToggleABRMax()
{
	if (!setABRMax) { edit_abrmax->Deactivate(); slider_abrmax->Deactivate(); }
	else		{ edit_abrmax->Activate();   slider_abrmax->Activate();	  }
}

Void BoCA::ConfigureVorbis::SetABRMax()
{
	edit_abrmax->SetText(String::FromInt(abrMax));
}

Void BoCA::ConfigureVorbis::SetABRMaxByEditBox()
{
	slider_abrmax->SetValue(edit_abrmax->GetText().ToInt());
}
