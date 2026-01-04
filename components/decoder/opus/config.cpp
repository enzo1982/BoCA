 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureOpus::ConfigID = "OpusDecoder";

BoCA::ConfigureOpus::ConfigureOpus(Bool iBweSupported)
{
	bweSupported = iBweSupported;

	const Config	*config = Config::Get();

	complexity	= config->GetIntValue(ConfigID, "Complexity", 10);
	enableBwe	= bweSupported && config->GetIntValue(ConfigID, "BandwidthExtension", False);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Decoders::Opus");

	group_quality		= new GroupBox(i18n->TranslateString("Quality"), Point(7, 11), Size(286, 64));

	text_complexity		= new Text(i18n->AddColon(i18n->TranslateString("Complexity")), Point(10, 14));

	slider_complexity	= new Slider(Point(17 + text_complexity->GetUnscaledTextWidth(), 12), Size(240 - text_complexity->GetUnscaledTextWidth(), 0), OR_HORZ, &complexity, 0, 10);
	slider_complexity->onValueChange.Connect(&ConfigureOpus::SetComplexity, this);

	text_complexity_value	= new Text(NIL, Point(264, 14));

	check_enable_bwe	= new CheckBox(i18n->TranslateString("Enable Speech Bandwidth Extension"), Point(10, text_complexity->GetY() + 23), Size(266, 0), &enableBwe);

	group_quality->Add(text_complexity);
	group_quality->Add(slider_complexity);
	group_quality->Add(text_complexity_value);
	group_quality->Add(check_enable_bwe);

	SetComplexity();

	Add(group_quality);

	SetSize(Size(300, 192));
}

BoCA::ConfigureOpus::~ConfigureOpus()
{
	DeleteObject(group_quality);
	DeleteObject(text_complexity);
	DeleteObject(slider_complexity);
	DeleteObject(text_complexity_value);
	DeleteObject(check_enable_bwe);
}

Int BoCA::ConfigureOpus::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "Complexity", complexity);

	if (bweSupported) config->SetIntValue(ConfigID, "BandwidthExtension", enableBwe);

	return Success();
}

Void BoCA::ConfigureOpus::SetComplexity()
{
	text_complexity_value->SetText(String::FromInt(complexity));

	if (bweSupported && complexity >= 4) check_enable_bwe->Activate();
	else				     check_enable_bwe->Deactivate();
}
