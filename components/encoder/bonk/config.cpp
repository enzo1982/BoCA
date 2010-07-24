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

BoCA::ConfigureBonk::ConfigureBonk()
{
	Point	 pos;
	Size	 size;

	Config	*config = Config::Get();

	quant		= config->GetIntValue("Bonk", "Quantization", 8);
	predictor	= config->GetIntValue("Bonk", "Predictor", 32);
	downsampling	= config->GetIntValue("Bonk", "Downsampling", 2);
	jstereo		= config->GetIntValue("Bonk", "JointStereo", 0);
	lossless	= config->GetIntValue("Bonk", "Lossless", 0);

	I18n	*i18n = I18n::Get();

	pos.x = 7;
	pos.y = 11;
	size.cx = 168;
	size.cy = 43;

	group_mode		= new GroupBox(i18n->TranslateString("Encoder mode"), pos, size);

	pos.x += 176;

	group_stereo		= new GroupBox(i18n->TranslateString("Stereo mode"), pos, size);

	pos.x -= 176;
	pos.y += 55;

	group_quant		= new GroupBox(i18n->TranslateString("Quantization"), pos, size);

	pos.x += 176;

	group_downsampling	= new GroupBox(i18n->TranslateString("Downsampling ratio"), pos, size);

	pos.x -= 176;
	pos.y += 55;
	size.cx += 176;

	group_predictor		= new GroupBox(i18n->TranslateString("Predictor size"), pos, size);

	pos.x = 17;
	pos.y = 24;
	size.cx = 147;
	size.cy = 0;

	check_lossless		= new CheckBox(i18n->TranslateString("Enable lossless encoding"), pos, size, &lossless);
	check_lossless->onAction.Connect(&ConfigureBonk::SetEncoderMode, this);

	pos.x += 176;

	check_joint		= new CheckBox(i18n->TranslateString("Enable Joint Stereo"), pos, size, &jstereo);

	pos.x = 17;
	pos.y += 55;
	size.cx = 120;
	size.cy = 0;

	slider_quant		= new Slider(pos, size, OR_HORZ, &quant, 0, 40);
	slider_quant->onValueChange.Connect(&ConfigureBonk::SetQuantization, this);

	pos.x += 127;
	pos.y += 2;

	text_quant		= new Text(NIL, pos);
	SetQuantization();

	pos.x += 49;
	pos.y -= 2;

	slider_downsampling	= new Slider(pos, size, OR_HORZ, &downsampling, 1, 10);
	slider_downsampling->onValueChange.Connect(&ConfigureBonk::SetDownsamplingRatio, this);

	pos.x += 127;
	pos.y += 2;

	text_downsampling	= new Text(NIL, pos);
	SetDownsamplingRatio();

	pos.x -= 303;
	pos.y += 53;
	size.cx += 176;

	slider_predictor	= new Slider(pos, size, OR_HORZ, &predictor, 0, 512);
	slider_predictor->onValueChange.Connect(&ConfigureBonk::SetPredictorSize, this);

	pos.x += 303;
	pos.y += 2;

	text_predictor		= new Text(NIL, pos);
	SetPredictorSize();
	SetEncoderMode();

	Add(group_quant);
	Add(slider_quant);
	Add(text_quant);
	Add(group_stereo);
	Add(check_joint);
	Add(group_mode);
	Add(check_lossless);
	Add(group_downsampling);
	Add(slider_downsampling);
	Add(text_downsampling);
	Add(group_predictor);
	Add(slider_predictor);
	Add(text_predictor);

	SetSize(Size(358, 171));
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

	config->SetIntValue("Bonk", "Quantization", quant);
	config->SetIntValue("Bonk", "Predictor", predictor);
	config->SetIntValue("Bonk", "Downsampling", downsampling);
	config->SetIntValue("Bonk", "JointStereo", jstereo);
	config->SetIntValue("Bonk", "Lossless", lossless);

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
