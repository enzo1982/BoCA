 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureBonk::ConfigID = "Bonk";

BoCA::ConfigureBonk::ConfigureBonk()
{
	const Config	*config = Config::Get();

	quant		= config->GetIntValue(ConfigID, "Quantization", 8);
	predictor	= config->GetIntValue(ConfigID, "Predictor", 32);
	downsampling	= config->GetIntValue(ConfigID, "Downsampling", 2);
	jstereo		= config->GetIntValue(ConfigID, "JointStereo", 0);
	lossless	= config->GetIntValue(ConfigID, "Lossless", 0);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::Bonk");

	group_mode		= new GroupBox(i18n->TranslateString("Encoder mode"), Point(7, 11), Size(179, 43));

	check_lossless		= new CheckBox(i18n->TranslateString("Enable lossless encoding"), Point(10, 13), Size(158, 0), &lossless);
	check_lossless->onAction.Connect(&ConfigureBonk::SetEncoderMode, this);

	group_mode->Add(check_lossless);

	group_stereo		= new GroupBox(i18n->TranslateString("Stereo mode"), Point(194, 11), Size(179, 43));

	check_joint		= new CheckBox(i18n->TranslateString("Enable Joint Stereo"), Point(10, 13), Size(158, 0), &jstereo);

	group_stereo->Add(check_joint);

	group_quant		= new GroupBox(i18n->TranslateString("Quantization"), Point(7, 66), Size(179, 43));

	slider_quant		= new Slider(Point(10, 13), Size(131, 0), OR_HORZ, &quant, 0, 40);
	slider_quant->onValueChange.Connect(&ConfigureBonk::SetQuantization, this);

	text_quant		= new Text("0.00", Point(148, 15));

	text_quant->SetX(169 - text_quant->GetUnscaledTextWidth());
	slider_quant->SetWidth(151 - text_quant->GetUnscaledTextWidth());

	group_quant->Add(slider_quant);
	group_quant->Add(text_quant);

	SetQuantization();

	group_downsampling	= new GroupBox(i18n->TranslateString("Downsampling ratio"), Point(194, 66), Size(179, 43));

	slider_downsampling	= new Slider(Point(10, 13), Size(131, 0), OR_HORZ, &downsampling, 1, 10);
	slider_downsampling->onValueChange.Connect(&ConfigureBonk::SetDownsamplingRatio, this);

	text_downsampling	= new Text("00:0", Point(148, 15));

	text_downsampling->SetX(169 - text_downsampling->GetUnscaledTextWidth());
	slider_downsampling->SetWidth(151 - text_downsampling->GetUnscaledTextWidth());

	group_downsampling->Add(slider_downsampling);
	group_downsampling->Add(text_downsampling);

	SetDownsamplingRatio();

	group_predictor		= new GroupBox(i18n->TranslateString("Predictor size"), Point(7, 121), Size(366, 43));

	slider_predictor	= new Slider(Point(10, 13), Size(318, 0), OR_HORZ, &predictor, 0, 512);
	slider_predictor->onValueChange.Connect(&ConfigureBonk::SetPredictorSize, this);

	text_predictor		= new Text("00:0", Point(335, 15));

	text_predictor->SetX(356 - text_predictor->GetUnscaledTextWidth());
	slider_predictor->SetWidth(338 - text_predictor->GetUnscaledTextWidth());

	group_predictor->Add(slider_predictor);
	group_predictor->Add(text_predictor);

	SetPredictorSize();
	SetEncoderMode();

	Add(group_quant);
	Add(group_stereo);
	Add(group_mode);
	Add(group_downsampling);
	Add(group_predictor);

	SetSize(Size(380, 171));
}

BoCA::ConfigureBonk::~ConfigureBonk()
{
	DeleteObject(group_quant);
	DeleteObject(slider_quant);
	DeleteObject(text_quant);
	DeleteObject(group_stereo);
	DeleteObject(check_joint);
	DeleteObject(group_mode);
	DeleteObject(check_lossless);
	DeleteObject(group_downsampling);
	DeleteObject(slider_downsampling);
	DeleteObject(text_downsampling);
	DeleteObject(group_predictor);
	DeleteObject(slider_predictor);
	DeleteObject(text_predictor);
}

Int BoCA::ConfigureBonk::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "Quantization", quant);
	config->SetIntValue(ConfigID, "Predictor", predictor);
	config->SetIntValue(ConfigID, "Downsampling", downsampling);
	config->SetIntValue(ConfigID, "JointStereo", jstereo);
	config->SetIntValue(ConfigID, "Lossless", lossless);

	return Success();
}

Void BoCA::ConfigureBonk::SetQuantization()
{
	String	 val = String::FromFloat(0.05 * (double) quant);

	switch (val.Length())
	{
		case 1:
			val.Append(".00");
			break;
		case 3:
			val.Append("0");
			break;
	}

	text_quant->SetText(val);
}

Void BoCA::ConfigureBonk::SetPredictorSize()
{
	text_predictor->SetText(String::FromInt(predictor));
}

Void BoCA::ConfigureBonk::SetDownsamplingRatio()
{
	text_downsampling->SetText(String::FromInt(downsampling).Append(":1"));
}

Void BoCA::ConfigureBonk::SetEncoderMode()
{
	if (lossless)
	{
		group_quant->Deactivate();
		slider_quant->Deactivate();
		text_quant->Deactivate();

		group_downsampling->Deactivate();
		slider_downsampling->Deactivate();
		text_downsampling->Deactivate();
	}
	else
	{
		group_quant->Activate();
		slider_quant->Activate();
		text_quant->Activate();

		group_downsampling->Activate();
		slider_downsampling->Activate();
		text_downsampling->Activate();
	}
}
