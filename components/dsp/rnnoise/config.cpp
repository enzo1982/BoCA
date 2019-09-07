 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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
#include "dllinterface.h"

const String	 BoCA::ConfigureRNNoise::ConfigID = "rnnoise";

BoCA::ConfigureRNNoise::ConfigureRNNoise()
{
	const Config	*config = Config::Get();
	I18n		*i18n	= I18n::Get();

	i18n->SetContext("DSP::RNNoise");

	Int	 signalType = config->GetIntValue(ConfigID, "SignalType", 2);
	Int	 noiseType  = config->GetIntValue(ConfigID, "NoiseType", 0);

	group_signal	= new GroupBox(i18n->TranslateString("Signal/noise type"), Point(7, 11), Size(286, 68));

	text_signal	= new Text(i18n->AddColon(i18n->TranslateString("Signal type")), Point(10, 15));
	text_noise	= new Text(i18n->AddColon(i18n->TranslateString("Noise type")), Point(10, 42));

	Int	 maxTextSize = Math::Max(text_signal->GetUnscaledTextWidth(), text_noise->GetUnscaledTextWidth());

	combo_signal	= new ComboBox(Point(17 + maxTextSize, 12), Size(259 - maxTextSize, 0));
	combo_signal->AddEntry(i18n->TranslateString("General"));
	combo_signal->AddEntry(i18n->TranslateString("Voice (incl. laughter etc.)"));
	combo_signal->AddEntry(i18n->TranslateString("Speech"));
	combo_signal->SelectNthEntry(signalType);

	combo_noise	= new ComboBox(Point(17 + maxTextSize, 39), Size(259 - maxTextSize, 0));
	combo_noise->AddEntry(i18n->TranslateString("General"));
	combo_noise->AddEntry(i18n->TranslateString("Recording"));
	combo_noise->SelectNthEntry(noiseType);

	group_signal->Add(text_signal);
	group_signal->Add(combo_signal);

	group_signal->Add(text_noise);
	group_signal->Add(combo_noise);

	Add(group_signal);

	SetSize(Size(300, 86));
}

BoCA::ConfigureRNNoise::~ConfigureRNNoise()
{
	DeleteObject(group_signal);

	DeleteObject(text_signal);
	DeleteObject(combo_signal);

	DeleteObject(text_noise);
	DeleteObject(combo_noise);
}

Int BoCA::ConfigureRNNoise::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "SignalType", combo_signal->GetSelectedEntryNumber());
	config->SetIntValue(ConfigID, "NoiseType", combo_noise->GetSelectedEntryNumber());

	return Success();
}
