 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"
#include "dllinterface.h"

BoCA::ConfigureFLAC::ConfigureFLAC()
{
	Point	 pos;
	Size	 size;

	Config	*config = Config::Get();

	preset				= config->GetIntValue("FLAC", "Preset", 5);
	file_format			= config->GetIntValue("FLAC", "FileFormat", 0);
	streamable_subset		= config->GetIntValue("FLAC", "StreamableSubset", 1);
	do_mid_side_stereo		= config->GetIntValue("FLAC", "DoMidSideStereo", 1);
	loose_mid_side_stereo		= config->GetIntValue("FLAC", "LooseMidSideStereo", 0);
	blocksize			= config->GetIntValue("FLAC", "Blocksize", 4608) / 8;
	max_lpc_order			= config->GetIntValue("FLAC", "MaxLPCOrder", 8);
	qlp_coeff_precision		= config->GetIntValue("FLAC", "QLPCoeffPrecision", 0);
	do_qlp_coeff_prec_search	= config->GetIntValue("FLAC", "DoQLPCoeffPrecSearch", 0);
	do_exhaustive_model_search	= config->GetIntValue("FLAC", "DoExhaustiveModelSearch", 0);
	min_residual_partition_order	= config->GetIntValue("FLAC", "MinResidualPartitionOrder", 0);
	max_residual_partition_order	= config->GetIntValue("FLAC", "MaxResidualPartitionOrder", 5);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::FLAC");

	layer_simple		= new Layer(i18n->TranslateString("Basic"));
	layer_format		= new Layer(i18n->TranslateString("Format"));
	layer_advanced		= new Layer(i18n->TranslateString("Expert"));

	tabwidget		= new TabWidget(Point(7, 7), Size(498, 245));

	i18n->SetContext("Encoders::FLAC::Basic");

	group_preset		= new GroupBox(i18n->TranslateString("Presets"), Point(7, 11), Size(480, 39));

	text_preset		= new Text(i18n->TranslateString("Use preset:"), Point(9, 13));

	combo_preset		= new ComboBox(Point(17 + text_preset->GetUnscaledTextWidth(), 10), Size(453 - text_preset->GetUnscaledTextWidth(), 0));
	combo_preset->AddEntry(i18n->TranslateString("Custom settings"));
	combo_preset->AddEntry(String("0").Append(", ").Append(i18n->TranslateString("Fastest encoding")));
	combo_preset->AddEntry("1");
	combo_preset->AddEntry("2");
	combo_preset->AddEntry("3");
	combo_preset->AddEntry("4");
	combo_preset->AddEntry(String("5").Append(", ").Append(i18n->TranslateString("Default")));
	combo_preset->AddEntry("6");
	combo_preset->AddEntry("7");
	combo_preset->AddEntry(String("8").Append(", ").Append(i18n->TranslateString("Best compression")));
	combo_preset->SelectNthEntry(preset + 1);
	combo_preset->onSelectEntry.Connect(&ConfigureFLAC::SetPreset, this);

	group_preset->Add(text_preset);
	group_preset->Add(combo_preset);

	group_file_format	= new GroupBox(i18n->TranslateString("File format"), Point(7, 62), Size(150, 65));

	option_file_format_flac	= new OptionBox("FLAC", Point(10, 13), Size(130, 0), &file_format, 0);
	option_file_format_ogg	= new OptionBox("Ogg FLAC", Point(10, 38), Size(130, 0), &file_format, 1);

	if (*ex_FLAC_API_SUPPORTS_OGG_FLAC == 0)
	{
		file_format = 0;

		option_file_format_ogg->Deactivate();
	}

	group_file_format->Add(option_file_format_flac);
	group_file_format->Add(option_file_format_ogg);

	group_stereo		= new GroupBox(i18n->TranslateString("Stereo mode"), Point(165, 62), Size(150, 65));

	check_mid_side_stereo	= new CheckBox(i18n->TranslateString("Joint Stereo"), Point(10, 13), Size(130, 0), &do_mid_side_stereo);
	check_mid_side_stereo->onAction.Connect(&ConfigureFLAC::SetStereoMode, this);

	check_loose_mid_side	= new CheckBox(i18n->TranslateString("Adaptive Joint Stereo"), Point(10, 38), Size(130, 0), &loose_mid_side_stereo);

	group_stereo->Add(check_mid_side_stereo);
	group_stereo->Add(check_loose_mid_side);

	layer_simple->Add(group_preset);
	layer_simple->Add(group_file_format);
	layer_simple->Add(group_stereo);

	i18n->SetContext("Encoders::FLAC::Format");

	group_format		= new GroupBox(i18n->TranslateString("Format"), Point(7, 11), Size(480, 66));

	check_streamable_subset	= new CheckBox(i18n->TranslateString("Use streamable subset"), Point(10, 13), Size(480, 0), &streamable_subset);
	check_streamable_subset->onAction.Connect(&ConfigureFLAC::SetStreamableSubset, this);
	check_streamable_subset->SetWidth(check_streamable_subset->GetUnscaledTextWidth() + 21);

	text_blocksize		= new Text(i18n->TranslateString("Blocksize:"), Point(9, 40));

	slider_blocksize	= new Slider(Point(16 + text_blocksize->GetUnscaledTextWidth(), 38), Size(319, 0), OR_HORZ, &blocksize, 24, 4096);
	slider_blocksize->onValueChange.Connect(&ConfigureFLAC::SetBlockSize, this);

	edit_blocksize		= new EditBox(NIL, Point(344 + text_blocksize->GetUnscaledTextWidth(), 37), Size(37, 0), 5);
	edit_blocksize->onInput.Connect(&ConfigureFLAC::EditBlockSize, this);

	text_blocksize_bytes	= new Text(i18n->TranslateString("bytes"), Point(388 + text_blocksize->GetUnscaledTextWidth(), 40));

	group_format->Add(check_streamable_subset);
	group_format->Add(text_blocksize);
	group_format->Add(slider_blocksize);
	group_format->Add(edit_blocksize);
	group_format->Add(text_blocksize_bytes);

	layer_format->Add(group_format);

	i18n->SetContext("Encoders::FLAC::Expert");

	group_apodization	= new GroupBox(i18n->TranslateString("Apodization"), Point(7, 11), Size(480, 56));

	text_apodization	= new Text(i18n->TranslateString("Apodization function(s):"), Point(9, 13));
	edit_apodization	= new EditBox(config->GetStringValue("FLAC", "Apodization", "tukey(0.5)"), Point(16 + text_apodization->GetUnscaledTextWidth(), 10), Size(454 - text_apodization->GetUnscaledTextWidth(), 0));

	list_apodization	= new ListBox(pos, size);
	list_apodization->AddEntry("bartlett");
	list_apodization->AddEntry("bartlett_hann");
	list_apodization->AddEntry("blackman");
	list_apodization->AddEntry("blackman_harris_4term_92db");
	list_apodization->AddEntry("connes");
	list_apodization->AddEntry("flattop");
	list_apodization->AddEntry("gauss(0.2)");
	list_apodization->AddEntry("hamming");
	list_apodization->AddEntry("hann");
	list_apodization->AddEntry("kaiser_bessel");
	list_apodization->AddEntry("nuttall");
	list_apodization->AddEntry("rectangle");
	list_apodization->AddEntry("triangle");
	list_apodization->AddEntry("tukey(0.5)");
	list_apodization->AddEntry("welch");

	edit_apodization->SetDropDownList(list_apodization);

	text_apodization_explain= new Text(String(i18n->TranslateString("Note:")).Append(" ").Append(i18n->TranslateString("You can specify multiple functions separated by semicolons.")), Point(18 + text_apodization->GetUnscaledTextWidth(), 35));

	group_apodization->Add(text_apodization);
	group_apodization->Add(edit_apodization);
	group_apodization->Add(text_apodization_explain);

	group_lpc		= new GroupBox(i18n->TranslateString("Linear predictor"), Point(7, 79), Size(480, 62));

	text_max_lpc_order	= new Text(String(i18n->TranslateString("Maximum LPC order")).Append(":"), Point(9, 13));

	slider_max_lpc_order	= new Slider(Point(16, 11), Size(250, 0), OR_HORZ, &max_lpc_order, 0, 32);
	slider_max_lpc_order->onValueChange.Connect(&ConfigureFLAC::SetLPCOrder, this);

	text_max_lpc_order_value= new Text(i18n->TranslateString("disabled"), Point(273, 13));

	check_exhaustive_model	= new CheckBox(i18n->TranslateString("Exhaustive model search"), Point(323, 11), Size(150, 0), &do_exhaustive_model_search);

	text_qlp_precision	= new Text(String(i18n->TranslateString("Quantized LPC precision")).Append(":"), Point(9, 36));

	slider_qlp_precision	= new Slider(Point(16, 34), Size(250, 0), OR_HORZ, &qlp_coeff_precision, 0, 32);
	slider_qlp_precision->onValueChange.Connect(&ConfigureFLAC::SetQLPPrecision, this);

	text_qlp_precision_value= new Text(i18n->TranslateString("auto"), Point(273, 36));

	check_qlp_precision_search= new CheckBox(i18n->TranslateString("Optimize LPC quantization"), Point(323, 34), Size(150, 0), &do_qlp_coeff_prec_search);
	check_qlp_precision_search->onAction.Connect(&ConfigureFLAC::SetQLPSearch, this);

	Int	 maxTextSize = Math::Max(text_max_lpc_order_value->GetUnscaledTextWidth(), text_qlp_precision_value->GetUnscaledTextWidth());

	check_exhaustive_model->SetX(text_max_lpc_order_value->GetX() + maxTextSize + 8); check_exhaustive_model->SetWidth(189 - maxTextSize);
	check_qlp_precision_search->SetX(text_max_lpc_order_value->GetX() + maxTextSize + 8); check_qlp_precision_search->SetWidth(189 - maxTextSize);

	group_lpc->Add(text_max_lpc_order);
	group_lpc->Add(slider_max_lpc_order);
	group_lpc->Add(text_max_lpc_order_value);
	group_lpc->Add(check_exhaustive_model);
	group_lpc->Add(check_qlp_precision_search);
	group_lpc->Add(text_qlp_precision);
	group_lpc->Add(slider_qlp_precision);
	group_lpc->Add(text_qlp_precision_value);

	group_rice		= new GroupBox(i18n->TranslateString("Residual coding"), Point(7, 153), Size(296, 62));

	text_min_part_order	= new Text(String(i18n->TranslateString("Minimum partition order")).Append(":"), Point(9, 13));

	slider_min_part_order	= new Slider(Point(16, 11), Size(250, 0), OR_HORZ, &min_residual_partition_order, 0, 16);
	slider_min_part_order->onValueChange.Connect(&ConfigureFLAC::SetRiceOrder, this);

	text_min_part_order_value= new Text(NIL, Point(273, 13));

	text_max_part_order	= new Text(String(i18n->TranslateString("Maximum partition order")).Append(":"), Point(9, 36));

	slider_max_part_order	= new Slider(Point(16, 34), Size(250, 0), OR_HORZ, &max_residual_partition_order, 0, 16);
	slider_max_part_order->onValueChange.Connect(&ConfigureFLAC::SetRiceOrder, this);

	text_max_part_order_value= new Text(NIL, Point(273, 36));

	maxTextSize = Math::Max(Math::Max(text_min_part_order->GetUnscaledTextWidth(), text_max_part_order->GetUnscaledTextWidth()), Math::Max(text_max_lpc_order->GetUnscaledTextWidth(), text_qlp_precision->GetUnscaledTextWidth()));

	slider_min_part_order->SetX(group_lpc->GetX() + 16 + maxTextSize); slider_min_part_order->SetWidth(250 - maxTextSize);
	slider_max_part_order->SetX(group_lpc->GetX() + 16 + maxTextSize); slider_max_part_order->SetWidth(250 - maxTextSize);
	slider_max_lpc_order->SetX(group_lpc->GetX() + 16 + maxTextSize); slider_max_lpc_order->SetWidth(250 - maxTextSize);
	slider_qlp_precision->SetX(group_lpc->GetX() + 16 + maxTextSize); slider_qlp_precision->SetWidth(250 - maxTextSize);

	group_rice->Add(text_min_part_order);
	group_rice->Add(text_max_part_order);
	group_rice->Add(slider_min_part_order);
	group_rice->Add(text_min_part_order_value);
	group_rice->Add(slider_max_part_order);
	group_rice->Add(text_max_part_order_value);

	layer_advanced->Add(group_apodization);
	layer_advanced->Add(group_lpc);
	layer_advanced->Add(group_rice);

	SetStereoMode();
	SetStreamableSubset();
	SetBlockSize();
	SetRiceOrder();
	SetLPCOrder();
	SetQLPSearch();
	SetQLPPrecision();
	SetPreset();

	Add(tabwidget);

	tabwidget->Add(layer_simple);
	tabwidget->Add(layer_format);
	tabwidget->Add(layer_advanced);

	SetSize(Size(512, 259));
}

BoCA::ConfigureFLAC::~ConfigureFLAC()
{
	DeleteObject(tabwidget);
	DeleteObject(layer_simple);
	DeleteObject(layer_format);
	DeleteObject(layer_advanced);

	DeleteObject(group_preset);
	DeleteObject(text_preset);
	DeleteObject(combo_preset);

	DeleteObject(group_file_format);
	DeleteObject(option_file_format_flac);
	DeleteObject(option_file_format_ogg);

	DeleteObject(group_stereo);
	DeleteObject(check_mid_side_stereo);
	DeleteObject(check_loose_mid_side);

	DeleteObject(group_format);
	DeleteObject(check_streamable_subset);
	DeleteObject(text_blocksize);
	DeleteObject(slider_blocksize);
	DeleteObject(edit_blocksize);
	DeleteObject(text_blocksize_bytes);

	DeleteObject(group_apodization);
	DeleteObject(text_apodization);
	DeleteObject(edit_apodization);
	DeleteObject(list_apodization);
	DeleteObject(text_apodization_explain);

	DeleteObject(group_lpc);
	DeleteObject(text_max_lpc_order);
	DeleteObject(slider_max_lpc_order);
	DeleteObject(text_max_lpc_order_value);
	DeleteObject(check_exhaustive_model);
	DeleteObject(check_qlp_precision_search);
	DeleteObject(text_qlp_precision);
	DeleteObject(slider_qlp_precision);
	DeleteObject(text_qlp_precision_value);

	DeleteObject(group_rice);
	DeleteObject(text_min_part_order);
	DeleteObject(text_max_part_order);
	DeleteObject(slider_min_part_order);
	DeleteObject(text_min_part_order_value);
	DeleteObject(slider_max_part_order);
	DeleteObject(text_max_part_order_value);
}

Int BoCA::ConfigureFLAC::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue("FLAC", "Preset", preset);
	config->SetIntValue("FLAC", "FileFormat", file_format);
	config->SetIntValue("FLAC", "StreamableSubset", streamable_subset);
	config->SetIntValue("FLAC", "DoMidSideStereo", do_mid_side_stereo);
	config->SetIntValue("FLAC", "LooseMidSideStereo", loose_mid_side_stereo);
	config->SetIntValue("FLAC", "Blocksize", (streamable_subset ? blocksize * 8 : Math::Max(0, Math::Min(32768, (Int) edit_blocksize->GetText().ToInt()))));
	config->SetStringValue("FLAC", "Apodization", edit_apodization->GetText());
	config->SetIntValue("FLAC", "MaxLPCOrder", max_lpc_order);
	config->SetIntValue("FLAC", "QLPCoeffPrecision", qlp_coeff_precision);
	config->SetIntValue("FLAC", "DoQLPCoeffPrecSearch", do_qlp_coeff_prec_search);
	config->SetIntValue("FLAC", "DoExhaustiveModelSearch", do_exhaustive_model_search);
	config->SetIntValue("FLAC", "MinResidualPartitionOrder", min_residual_partition_order);
	config->SetIntValue("FLAC", "MaxResidualPartitionOrder", max_residual_partition_order);

	return Success();
}

Void BoCA::ConfigureFLAC::SetPreset()
{
	preset = combo_preset->GetSelectedEntryNumber() - 1;

	if (preset == -1)
	{
		group_stereo->Activate();

		group_format->Activate();

		group_apodization->Activate();
		group_lpc->Activate();
		group_rice->Activate();
	}
	else
	{
		group_stereo->Deactivate();

		group_format->Deactivate();

		group_apodization->Deactivate();
		group_lpc->Deactivate();
		group_rice->Deactivate();
	}
}

Void BoCA::ConfigureFLAC::SetLPCOrder()
{
	I18n	*i18n = I18n::Get();

	if (max_lpc_order == 0)	text_max_lpc_order_value->SetText(i18n->TranslateString("disabled"));
	else			text_max_lpc_order_value->SetText(String::FromInt(max_lpc_order));

	if (max_lpc_order == 0)
	{
		text_qlp_precision->Deactivate();
		slider_qlp_precision->Deactivate();
		text_qlp_precision_value->Deactivate();
		check_exhaustive_model->Deactivate();
		check_qlp_precision_search->Deactivate();
	}
	else
	{
		if (!do_qlp_coeff_prec_search)
		{
			text_qlp_precision->Activate();
			slider_qlp_precision->Activate();
			text_qlp_precision_value->Activate();
		}

		check_exhaustive_model->Activate();
		check_qlp_precision_search->Activate();
	}
}

Void BoCA::ConfigureFLAC::SetQLPSearch()
{
	if (do_qlp_coeff_prec_search)
	{
		text_qlp_precision->Deactivate();
		slider_qlp_precision->Deactivate();
		text_qlp_precision_value->Deactivate();
	}
	else
	{
		text_qlp_precision->Activate();
		slider_qlp_precision->Activate();
		text_qlp_precision_value->Activate();
	}
}

Void BoCA::ConfigureFLAC::SetQLPPrecision()
{
	GUI::Font	 font = text_qlp_precision_value->GetFont();

	if (qlp_coeff_precision >= 16)	font.SetColor(Color(255, 0, 0));
	else				font.SetColor(Color(0, 0, 0));

	text_qlp_precision_value->SetFont(font);

	I18n	*i18n = I18n::Get();

	if (qlp_coeff_precision == 0)	text_qlp_precision_value->SetText(i18n->TranslateString("auto"));
	else				text_qlp_precision_value->SetText(String::FromInt(qlp_coeff_precision));
}

Void BoCA::ConfigureFLAC::SetRiceOrder()
{
	if (max_residual_partition_order < min_residual_partition_order) slider_max_part_order->SetValue(min_residual_partition_order);
	if (min_residual_partition_order > max_residual_partition_order) slider_min_part_order->SetValue(max_residual_partition_order);

	text_min_part_order_value->SetText(String::FromInt(min_residual_partition_order));
	text_max_part_order_value->SetText(String::FromInt(max_residual_partition_order));
}

Void BoCA::ConfigureFLAC::SetStereoMode()
{
	if (do_mid_side_stereo)	check_loose_mid_side->Activate();
	else			check_loose_mid_side->Deactivate();
}

Void BoCA::ConfigureFLAC::SetStreamableSubset()
{
	if (streamable_subset)
	{
		edit_blocksize->Deactivate();

		slider_blocksize->SetRange(24, 576);
		slider_max_lpc_order->SetRange(0, 12);
	}
	else
	{
		edit_blocksize->Activate();

		slider_blocksize->SetRange(24, 4096);
		slider_max_lpc_order->SetRange(0, 32);
	}

	SetBlockSize();
}

Void BoCA::ConfigureFLAC::SetBlockSize()
{
	if (streamable_subset)
	{
		if (blocksize <= 28)				blocksize = 24;
		else if (blocksize > 28 && blocksize <= 48)	blocksize = 32;
		else if (blocksize > 48 && blocksize <= 68)	blocksize = 64;
		else if (blocksize > 68 && blocksize <= 100)	blocksize = 72;
		else if (blocksize > 100 && blocksize <= 136)	blocksize = 128;
		else if (blocksize > 136 && blocksize <= 200)	blocksize = 144;
		else if (blocksize > 200 && blocksize <= 272)	blocksize = 256;
		else if (blocksize > 272 && blocksize <= 400)	blocksize = 288;
		else if (blocksize > 400 && blocksize <= 544)	blocksize = 512;
		else if (blocksize > 544 && blocksize <= 800)	blocksize = 576;
		else if (blocksize > 800 && blocksize <= 1536)	blocksize = 1024;
		else if (blocksize > 1536 && blocksize <= 3072)	blocksize = 2048;
		else if (blocksize > 3072)			blocksize = 4096;

		slider_blocksize->SetValue(blocksize);
	}

	edit_blocksize->SetText(String::FromInt(blocksize * 8));
}

Void BoCA::ConfigureFLAC::EditBlockSize()
{
	String	 bsText = edit_blocksize->GetText();

	slider_blocksize->SetValue(bsText.ToInt() / 8);
	edit_blocksize->SetText(bsText);
}
