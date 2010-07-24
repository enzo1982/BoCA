 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"

BoCA::ConfigureBladeEnc::ConfigureBladeEnc()
{
	Config	*config = Config::Get();

	bitrate		= GetSliderValue();
	crc		= config->GetIntValue("BladeEnc", "CRC", 0);
	copyright	= config->GetIntValue("BladeEnc", "Copyright", 0);
	original	= config->GetIntValue("BladeEnc", "Original", 1);
	priv		= config->GetIntValue("BladeEnc", "Private", 0);
	dualchannel	= config->GetIntValue("BladeEnc", "DualChannel", 0);

	I18n	*i18n = I18n::Get();

	group_bit		= new GroupBox(i18n->TranslateString("Bitrate"), Point(7, 11), Size(168, 43));
	group_copyright		= new GroupBox(i18n->TranslateString("Copyright bit"), Point(183, 11), Size(168, 43));
	group_crc		= new GroupBox(i18n->TranslateString("CRC"), Point(7, 66), Size(168, 43));
	group_original		= new GroupBox(i18n->TranslateString("Original bit"), Point(183, 66), Size(168, 43));
	group_dualchannel	= new GroupBox(i18n->TranslateString("Channels"), Point(7, 121), Size(168, 43));
	group_private		= new GroupBox(i18n->TranslateString("Private bit"), Point(183, 121), Size(168, 43));

	slider_bit		= new Slider(Point(17, 24), Size(103, 0), OR_HORZ, &bitrate, 0, 13);
	slider_bit->onValueChange.Connect(&ConfigureBladeEnc::SetBitrate, this);

	text_bit		= new Text(NIL, Point(127, 26));
	SetBitrate();

	check_copyright		= new CheckBox(i18n->TranslateString("Set Copyright bit"), Point(193, 24), Size(147, 0), &copyright);
	check_crc		= new CheckBox(i18n->TranslateString("Enable CRC"), Point(17, 79), Size(147, 0), &crc);
	check_original		= new CheckBox(i18n->TranslateString("Set Original bit"), Point(193, 79), Size(147, 0), &original);
	check_dualchannel	= new CheckBox(i18n->TranslateString("Dual channel encoding"), Point(17, 134), Size(147, 0), &dualchannel);
	check_private		= new CheckBox(i18n->TranslateString("Set Private bit"), Point(193, 134), Size(147, 0), &priv);

	Add(group_bit);
	Add(slider_bit);
	Add(text_bit);
	Add(group_crc);
	Add(check_crc);
	Add(group_copyright);
	Add(check_copyright);
	Add(group_original);
	Add(check_original);
	Add(group_private);
	Add(check_private);
	Add(group_dualchannel);
	Add(check_dualchannel);

	SetSize(Size(358, 171));
}

BoCA::ConfigureBladeEnc::~ConfigureBladeEnc()
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

Int BoCA::ConfigureBladeEnc::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("BladeEnc", "Bitrate", GetBitrate());
	config->SetIntValue("BladeEnc", "CRC", crc);
	config->SetIntValue("BladeEnc", "Copyright", copyright);
	config->SetIntValue("BladeEnc", "Original", original);
	config->SetIntValue("BladeEnc", "Private", priv);
	config->SetIntValue("BladeEnc", "DualChannel", dualchannel);

	return Success();
}

Void BoCA::ConfigureBladeEnc::SetBitrate()
{
	text_bit->SetText(String::FromInt(GetBitrate()).Append(" kbit"));
}

Int BoCA::ConfigureBladeEnc::GetBitrate()
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

Int BoCA::ConfigureBladeEnc::GetSliderValue()
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
