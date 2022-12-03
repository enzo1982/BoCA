 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureVolume::ConfigID = "Volume";

BoCA::ConfigureVolume::ConfigureVolume()
{
	const Config	*config = Config::Get();

	qdB = config->GetIntValue(ConfigID, "qdB", 0);

	I18n		*i18n	= I18n::Get();

	i18n->SetContext("DSP::Volume");

	group_volume		= new GroupBox(i18n->TranslateString("Volume adjustment"), Point(7, 11), Size(350, 40));

	text_volume		= new Text(i18n->AddColon(i18n->TranslateString("Adjustment")), Point(10, 15));

	text_db_value		= new Text(i18n->TranslateString("%1 dB").Replace("%1", "+24.00"), Point(12, 15));
	text_db_value->SetX(text_db_value->GetUnscaledTextWidth() + 8);
	text_db_value->SetOrientation(OR_UPPERRIGHT);

	slider_db		= new Slider(Point(text_volume->GetUnscaledTextWidth() + 16, 13), Size(group_volume->GetWidth() - text_volume->GetUnscaledTextWidth() - text_db_value->GetUnscaledTextWidth() - 32, 0), OR_HORZ, &qdB, -96, 96);
	slider_db->onValueChange.Connect(&ConfigureVolume::OnChangeValue, this);

	OnChangeValue();

	group_volume->Add(text_volume);
	group_volume->Add(slider_db);
	group_volume->Add(text_db_value);

	Add(group_volume);

	SetSize(Size(364, 58));
}

BoCA::ConfigureVolume::~ConfigureVolume()
{
	DeleteObject(group_volume);

	DeleteObject(text_volume);
	DeleteObject(slider_db);
	DeleteObject(text_db_value);
}

Int BoCA::ConfigureVolume::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "qdB", qdB);

	return Success();
}

Void BoCA::ConfigureVolume::OnChangeValue()
{
	I18n		*i18n	= I18n::Get();

	i18n->SetContext("DSP::Volume");

	text_db_value->SetText(i18n->TranslateString("%1 dB").Replace("%1", String(qdB > 0 ? "+" : NIL).Append(String::FromFloat(qdB / 4.0))
												       .Append(		 qdB % 4  == 0 ? ".00" : NIL)
												       .Append(Math::Abs(qdB % 4) == 2 ?   "0" : NIL)));
}
