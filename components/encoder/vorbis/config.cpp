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

const String	 BoCA::ConfigureVorbis::ConfigID = "Vorbis";

BoCA::ConfigureVorbis::ConfigureVorbis()
{
	const Config	*config = Config::Get();
	I18n		*i18n	= I18n::Get();

	i18n->SetContext("Encoders::Vorbis");

	/* Get configuration.
	 */
	mode		= config->GetIntValue(ConfigID, "Mode", 0);
	fileExtension	= config->GetIntValue(ConfigID, "FileExtension", 0);
	quality		= config->GetIntValue(ConfigID, "Quality", 60);
	setABRMin	= config->GetIntValue(ConfigID, "SetMinBitrate", False);
	abrMin		= config->GetIntValue(ConfigID, "MinBitrate", 32);
	setABRNom	= config->GetIntValue(ConfigID, "SetBitrate", True);
	abrNom		= config->GetIntValue(ConfigID, "Bitrate", 192);
	setABRMax	= config->GetIntValue(ConfigID, "SetMaxBitrate", False);
	abrMax		= config->GetIntValue(ConfigID, "MaxBitrate", 320);

	/* Encoding mode group.
	 */
	group_mode		= new GroupBox(i18n->TranslateString("Encoding Mode"), Point(7, 11), Size(168, 65));

	option_mode_vbr		= new OptionBox(i18n->AddBrackets("VBR", i18n->TranslateString("Variable Bitrate")), Point(10, 13), Size(148, 0), &mode, 0);
	option_mode_vbr->onAction.Connect(&ConfigureVorbis::SetMode, this);

	option_mode_abr		= new OptionBox(i18n->AddBrackets("ABR", i18n->TranslateString("Average Bitrate")), Point(10, 38), Size(148, 0), &mode, 1);
	option_mode_abr->onAction.Connect(&ConfigureVorbis::SetMode, this);

	group_mode->Add(option_mode_vbr);
	group_mode->Add(option_mode_abr);

	Int	 maxTextSize = Math::Max(option_mode_vbr->GetUnscaledTextWidth(), option_mode_abr->GetUnscaledTextWidth());

	group_mode->SetWidth(Math::Max(168, maxTextSize + 41));

	option_mode_vbr->SetWidth(group_mode->GetWidth() - 20);
	option_mode_abr->SetWidth(group_mode->GetWidth() - 20);

	/* File extension group.
	 */
	group_extension		= new GroupBox(i18n->TranslateString("File extension"), Point(group_mode->GetWidth() + 15, 11), Size(336 - group_mode->GetWidth(), 65));

	option_extension_ogg	= new OptionBox(".ogg", Point(10, 13), Size(group_extension->GetWidth() - 20, 0), &fileExtension, 0);
	option_extension_oga	= new OptionBox(".oga", Point(10, 38), Size(group_extension->GetWidth() - 20, 0), &fileExtension, 1);

	group_extension->Add(option_extension_ogg);
	group_extension->Add(option_extension_oga);

	/* Quality group.
	 */
	group_quality		= new GroupBox(i18n->TranslateString("Quality"), Point(7, 88), Size(344, 42));

	text_quality		= new Text(i18n->AddColon(i18n->TranslateString("Quality")), Point(10, 16));

	slider_quality		= new Slider(Point(text_quality->GetUnscaledTextWidth() + 18, 14), Size(289 - text_quality->GetUnscaledTextWidth(), 0), OR_HORZ, &quality, -20, 100);
	slider_quality->onValueChange.Connect(&ConfigureVorbis::SetQuality, this);

	text_quality_value	= new Text(NIL, Point(313, 16));

	group_quality->Add(text_quality);
	group_quality->Add(slider_quality);
	group_quality->Add(text_quality_value);

	/* Bitrate group.
	 */
	group_bitrate		= new GroupBox(i18n->TranslateString("Bitrate"), Point(7, 88), Size(344, 96));

	check_abrmin		= new CheckBox(i18n->AddColon(i18n->TranslateString("Minimum bitrate")), Point(10, 14), Size(), &setABRMin);
	check_abrmin->onAction.Connect(&ConfigureVorbis::ToggleABRMin, this);

	slider_abrmin		= new Slider(Point(38, 14), Size(228, 0), OR_HORZ, &abrMin, 32, 320);
	slider_abrmin->onValueChange.Connect(&ConfigureVorbis::SetABRMin, this);

	text_abrmin_kbps	= new Text(i18n->TranslateString("%1 kbps", "Technical").Replace("%1", NIL).Trim(), Point(35, 16));
	text_abrmin_kbps->SetOrientation(OR_UPPERRIGHT);
	text_abrmin_kbps->SetX(text_abrmin_kbps->GetUnscaledTextWidth() + 10);

	edit_abrmin		= new EditBox(Point(text_abrmin_kbps->GetX() + 32, 13), Size(25, 0), 3);
	edit_abrmin->SetFlags(EDB_NUMERIC);
	edit_abrmin->SetOrientation(OR_UPPERRIGHT);
	edit_abrmin->onInput.Connect(&ConfigureVorbis::SetABRMinByEditBox, this);

	check_abrnom		= new CheckBox(i18n->AddColon(i18n->TranslateString("Average bitrate")), Point(10, 41), Size(), &setABRNom);
	check_abrnom->onAction.Connect(&ConfigureVorbis::ToggleABRNom, this);

	slider_abrnom		= new Slider(Point(38, 41), Size(228, 0), OR_HORZ, &abrNom, 32, 320);
	slider_abrnom->onValueChange.Connect(&ConfigureVorbis::SetABRNom, this);

	text_abrnom_kbps	= new Text(text_abrmin_kbps->GetText(), Point(text_abrmin_kbps->GetX(), 43));
	text_abrnom_kbps->SetOrientation(OR_UPPERRIGHT);

	edit_abrnom		= new EditBox(Point(text_abrnom_kbps->GetX() + 32, 40), Size(25, 0), 3);
	edit_abrnom->SetFlags(EDB_NUMERIC);
	edit_abrnom->SetOrientation(OR_UPPERRIGHT);
	edit_abrnom->onInput.Connect(&ConfigureVorbis::SetABRNomByEditBox, this);

	check_abrmax		= new CheckBox(i18n->AddColon(i18n->TranslateString("Maximum bitrate")), Point(10, 68), Size(), &setABRMax);
	check_abrmax->onAction.Connect(&ConfigureVorbis::ToggleABRMax, this);

	slider_abrmax		= new Slider(Point(38, 68), Size(228, 0), OR_HORZ, &abrMax, 32, 320);
	slider_abrmax->onValueChange.Connect(&ConfigureVorbis::SetABRMax, this);

	text_abrmax_kbps	= new Text(text_abrnom_kbps->GetText(), Point(text_abrnom_kbps->GetX(), 70));
	text_abrmax_kbps->SetOrientation(OR_UPPERRIGHT);

	edit_abrmax		= new EditBox(Point(text_abrmax_kbps->GetX() + 32, 67), Size(25, 0), 3);
	edit_abrmax->SetFlags(EDB_NUMERIC);
	edit_abrmax->SetOrientation(OR_UPPERRIGHT);
	edit_abrmax->onInput.Connect(&ConfigureVorbis::SetABRMaxByEditBox, this);

	maxTextSize = Math::Max(Math::Max(check_abrmin->GetUnscaledTextWidth(), check_abrnom->GetUnscaledTextWidth()), check_abrmax->GetUnscaledTextWidth());

	check_abrmin->SetWidth(maxTextSize + 20);
	check_abrnom->SetWidth(maxTextSize + 20);
	check_abrmax->SetWidth(maxTextSize + 20);

	slider_abrmin->SetX(maxTextSize + 38);
	slider_abrnom->SetX(maxTextSize + 38);
	slider_abrmax->SetX(maxTextSize + 38);

	slider_abrmin->SetWidth(266 - maxTextSize - text_abrmin_kbps->GetX());
	slider_abrnom->SetWidth(266 - maxTextSize - text_abrnom_kbps->GetX());
	slider_abrmax->SetWidth(266 - maxTextSize - text_abrmax_kbps->GetX());

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

	config->SetIntValue(ConfigID, "Mode", mode);

	config->SetIntValue(ConfigID, "FileExtension", fileExtension);

	config->SetIntValue(ConfigID, "Quality", quality);

	config->SetIntValue(ConfigID, "SetMinBitrate", setABRMin);
	config->SetIntValue(ConfigID, "MinBitrate", abrMin);

	config->SetIntValue(ConfigID, "SetBitrate", setABRNom);
	config->SetIntValue(ConfigID, "Bitrate", abrNom);

	config->SetIntValue(ConfigID, "SetMaxBitrate", setABRMax);
	config->SetIntValue(ConfigID, "MaxBitrate", abrMax);

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
	if (!edit_abrmin->IsFocussed()) edit_abrmin->SetText(String::FromInt(abrMin));
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
	if (!edit_abrnom->IsFocussed()) edit_abrnom->SetText(String::FromInt(abrNom));
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
	if (!edit_abrmax->IsFocussed()) edit_abrmax->SetText(String::FromInt(abrMax));
}

Void BoCA::ConfigureVorbis::SetABRMaxByEditBox()
{
	slider_abrmax->SetValue(edit_abrmax->GetText().ToInt());
}
