 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureBlade::ConfigID = "BladeEnc";

BoCA::ConfigureBlade::ConfigureBlade()
{
	const Config	*config = Config::Get();

	bitrate		= GetSliderValue();
	crc		= config->GetIntValue(ConfigID, "CRC", 0);
	copyright	= config->GetIntValue(ConfigID, "Copyright", 0);
	original	= config->GetIntValue(ConfigID, "Original", 1);
	priv		= config->GetIntValue(ConfigID, "Private", 0);
	dualchannel	= config->GetIntValue(ConfigID, "DualChannel", 0);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::BladeEnc");

	group_bit		= new GroupBox(i18n->TranslateString("Bitrate"), Point(7, 11), Size(179, 43));

	slider_bit		= new Slider(Point(10, 13), Size(114, 0), OR_HORZ, &bitrate, 0, 13);
	slider_bit->onValueChange.Connect(&ConfigureBlade::SetBitrate, this);

	text_bit		= new Text("320 kbit", Point(131, 15));

	group_bit->Add(slider_bit);
	group_bit->Add(text_bit);

	group_copyright		= new GroupBox(i18n->TranslateString("Copyright bit"), Point(194, 11), Size(179, 43));
	check_copyright		= new CheckBox(i18n->TranslateString("Set Copyright bit"), Point(10, 13), Size(158, 0), &copyright);
	group_copyright->Add(check_copyright);

	group_crc		= new GroupBox(i18n->TranslateString("CRC"), Point(7, 66), Size(179, 43));
	check_crc		= new CheckBox(i18n->TranslateString("Enable CRC"), Point(10, 13), Size(158, 0), &crc);
	group_crc->Add(check_crc);

	group_original		= new GroupBox(i18n->TranslateString("Original bit"), Point(194, 66), Size(179, 43));
	check_original		= new CheckBox(i18n->TranslateString("Set Original bit"), Point(10, 13), Size(158, 0), &original);
	group_original->Add(check_original);

	group_dualchannel	= new GroupBox(i18n->TranslateString("Channels"), Point(7, 121), Size(179, 43));
	check_dualchannel	= new CheckBox(i18n->TranslateString("Dual channel encoding"), Point(10, 13), Size(158, 0), &dualchannel);
	group_dualchannel->Add(check_dualchannel);

	group_private		= new GroupBox(i18n->TranslateString("Private bit"), Point(194, 121), Size(179, 43));
	check_private		= new CheckBox(i18n->TranslateString("Set Private bit"), Point(10, 13), Size(158, 0), &priv);
	group_private->Add(check_private);

	Int	 maxTextSize = Math::Max(Math::Max(Math::Max(check_copyright->GetUnscaledTextWidth(), check_private->GetUnscaledTextWidth()), check_crc->GetUnscaledTextWidth()), Math::Max(check_original->GetUnscaledTextWidth(), check_dualchannel->GetUnscaledTextWidth()));

	check_copyright->SetWidth(Math::Max(158, maxTextSize + 21));
	check_original->SetWidth(check_copyright->GetWidth());
	check_private->SetWidth(check_copyright->GetWidth());
	check_crc->SetWidth(check_copyright->GetWidth());
	check_dualchannel->SetWidth(check_copyright->GetWidth());

	group_bit->SetWidth(check_copyright->GetWidth() + 20);
	group_copyright->SetWidth(group_bit->GetWidth());
	group_original->SetWidth(group_bit->GetWidth());
	group_private->SetWidth(group_bit->GetWidth());
	group_crc->SetWidth(group_bit->GetWidth());
	group_dualchannel->SetWidth(group_bit->GetWidth());

	group_copyright->SetX(group_bit->GetWidth() + 15);
	group_original->SetX(group_bit->GetWidth() + 15);
	group_private->SetX(group_bit->GetWidth() + 15);

	text_bit->SetX(group_bit->GetWidth() - 10 - text_bit->GetUnscaledTextWidth());
	slider_bit->SetWidth(group_bit->GetWidth() - 28 - text_bit->GetUnscaledTextWidth());

	SetBitrate();

	Add(group_bit);
	Add(group_crc);
	Add(group_copyright);
	Add(group_original);
	Add(group_private);
	Add(group_dualchannel);

	SetSize(Size(2 * group_bit->GetWidth() + 22, 171));
}

BoCA::ConfigureBlade::~ConfigureBlade()
{
	DeleteObject(group_bit);
	DeleteObject(slider_bit);
	DeleteObject(text_bit);
	DeleteObject(group_crc);
	DeleteObject(check_crc);
	DeleteObject(group_copyright);
	DeleteObject(check_copyright);
	DeleteObject(group_original);
	DeleteObject(check_original);
	DeleteObject(group_private);
	DeleteObject(check_private);
	DeleteObject(group_dualchannel);
	DeleteObject(check_dualchannel);
}

Int BoCA::ConfigureBlade::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "Bitrate", GetBitrate());
	config->SetIntValue(ConfigID, "CRC", crc);
	config->SetIntValue(ConfigID, "Copyright", copyright);
	config->SetIntValue(ConfigID, "Original", original);
	config->SetIntValue(ConfigID, "Private", priv);
	config->SetIntValue(ConfigID, "DualChannel", dualchannel);

	return Success();
}

Void BoCA::ConfigureBlade::SetBitrate()
{
	text_bit->SetText(String::FromInt(GetBitrate()).Append(" kbit"));
}

Int BoCA::ConfigureBlade::GetBitrate()
{
	switch (bitrate)
	{
		case 0:  return 32;
		case 1:  return 40;
		case 2:  return 48;
		case 3:  return 56;
		case 4:  return 64;
		case 5:  return 80;
		case 6:  return 96;
		case 7:  return 112;
		case 8:  return 128;
		case 9:  return 160;
		case 10: return 192;
		case 11: return 224;
		case 12: return 256;
		case 13: return 320;
		default: return 128;
	}
}

Int BoCA::ConfigureBlade::GetSliderValue()
{
	switch (Config::Get()->GetIntValue("BladeEnc", "Bitrate", 192))
	{
		case 32:  return 0;
		case 40:  return 1;
		case 48:  return 2;
		case 56:  return 3;
		case 64:  return 4;
		case 80:  return 5;
		case 96:  return 6;
		case 112: return 7;
		case 128: return 8;
		case 160: return 9;
		case 192: return 10;
		case 224: return 11;
		case 256: return 12;
		case 320: return 13;
		default:  return 8;
	}
}
