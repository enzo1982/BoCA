 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2024 Robert Kausch <robert.kausch@freac.org>
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

const String	 BoCA::ConfigureRubberBand::ConfigID = "RubberBand";

BoCA::ConfigureRubberBand::ConfigureRubberBand()
{
	const Config	*config = Config::Get();

	tempo		= Math::Round(100.0 / (config->GetIntValue(ConfigID, "Stretch", 1000) / 1000.0));

	pitchOption	= config->GetIntValue(ConfigID, "PitchOption", 0);
	pitch		= Math::Round(Math::Log2(config->GetIntValue(ConfigID, "Pitch", 1000) / 1000.0) * 12.0 * 2.0);

	smoothing	= config->GetIntValue(ConfigID, "Smoothing", False);

	I18n		*i18n	= I18n::Get();

	i18n->SetContext("DSP::RubberBand");

	/* Basic options.
	 */
	group_basic		= new GroupBox(i18n->TranslateString("Basic controls"), Point(7, 11), Size(400, 92));

	text_tempo		= new Text(i18n->AddColon(i18n->TranslateString("Tempo")), Point(10, 15));
	text_pitch		= new Text(i18n->AddColon(i18n->TranslateString("Pitch")), Point(10, 40));

	Int	 maxTextSize = Math::Max(text_tempo->GetUnscaledTextWidth(), text_pitch->GetUnscaledTextWidth());

	slider_tempo		= new Slider(Point(maxTextSize + 16, 13), Size(150, 0), OR_HORZ, &tempo, 25, 200);
	slider_tempo->onValueChange.Connect(&ConfigureRubberBand::OnChangeTempo, this);

	text_tempo_value	= new Text(i18n->TranslateString("%1%", "Technical").Replace("%1", "+100"), Point(10, 15));
	text_tempo_value->SetOrientation(OR_UPPERRIGHT);

	option_semitones		= new OptionBox(i18n->TranslateString("Semitones"), Point(maxTextSize + 16, 38), Size(100, 0), &pitchOption, 0);
 	option_semitones->onAction.Connect(&ConfigureRubberBand::OnChangePitchOption, this);

	option_ratio	= new OptionBox(i18n->TranslateString("Frequency ratio"), Point(maxTextSize + 16, 64), Size(100, 0), &pitchOption, 1);
 	option_ratio->onAction.Connect(&ConfigureRubberBand::OnChangePitchOption, this);

	maxTextSize = Math::Max(option_semitones->GetUnscaledTextWidth(), option_ratio->GetUnscaledTextWidth());

	option_semitones->SetWidth(maxTextSize + 23);
	option_ratio->SetWidth(maxTextSize + 23);

	slider_semitones	= new Slider(Point(option_semitones->GetX() + option_semitones->GetWidth() + 8, 38), Size(150, 0), OR_HORZ, &pitch, -36, 36);
	slider_semitones->onValueChange.Connect(&ConfigureRubberBand::OnChangeSemitones, this);

	text_semitones_value	= new Text("+24.0", Point(10, 40));
	text_semitones_value->SetOrientation(OR_UPPERRIGHT);

	edit_ratio_num		= new EditBox("000.00", Point(option_ratio->GetX() + option_ratio->GetWidth() + 8, 63), Size(50, 0), 8);
	edit_ratio_num->SetFlags(EDB_NUMERIC);
	edit_ratio_num->SetWidth(edit_ratio_num->GetUnscaledTextWidth() + 6);
	edit_ratio_num->SetText(String::FromFloat(config->GetIntValue(ConfigID, "RatioNum", 4320000) / 10000.0));
 	edit_ratio_num->onInput.Connect(&ConfigureRubberBand::OnEditRatio, this);

	text_ratio_num_hz	= new Text(i18n->TranslateString("Hz"), Point(edit_ratio_num->GetX() + edit_ratio_num->GetWidth() + 5, 66));
	text_ratio_colon	= new Text(i18n->TranslateString(":"), Point(text_ratio_num_hz->GetX() + text_ratio_num_hz->GetUnscaledTextWidth() + 5, 66));

	edit_ratio_den		= new EditBox(String::FromFloat(config->GetIntValue(ConfigID, "RatioDen", 4400000) / 10000.0), Point(text_ratio_colon->GetX() + text_ratio_colon->GetUnscaledTextWidth() + 5, 63), Size(edit_ratio_num->GetWidth(), 0), 8);
	edit_ratio_den->SetFlags(EDB_NUMERIC);
 	edit_ratio_den->onInput.Connect(&ConfigureRubberBand::OnEditRatio, this);

	text_ratio_den_hz	= new Text(i18n->TranslateString("Hz"), Point(edit_ratio_den->GetX() + edit_ratio_den->GetWidth() + 5, 66));
	text_ratio_equals	= new Text(i18n->TranslateString("="), Point(text_ratio_den_hz->GetX() + text_ratio_den_hz->GetUnscaledTextWidth() + 5, 66));

	text_ratio_value	= new Text(NIL, Point(text_ratio_equals->GetX() + text_ratio_equals->GetUnscaledTextWidth() + 5, 66));

	maxTextSize = Math::Max(text_tempo_value->GetUnscaledTextWidth(), text_semitones_value->GetUnscaledTextWidth());

	text_tempo_value->SetX(maxTextSize + 10);
	text_semitones_value->SetX(maxTextSize + 10);

	slider_tempo->SetWidth(group_basic->GetWidth() - slider_tempo->GetX() - maxTextSize - 18);
	slider_semitones->SetWidth(group_basic->GetWidth() - slider_semitones->GetX() - maxTextSize - 18);

	group_basic->Add(text_tempo);
	group_basic->Add(slider_tempo);
	group_basic->Add(text_tempo_value);

	group_basic->Add(text_pitch);

	group_basic->Add(option_semitones);
	group_basic->Add(slider_semitones);
	group_basic->Add(text_semitones_value);

	group_basic->Add(option_ratio);
	group_basic->Add(edit_ratio_num);
	group_basic->Add(text_ratio_num_hz);
	group_basic->Add(text_ratio_colon);
	group_basic->Add(edit_ratio_den);
	group_basic->Add(text_ratio_den_hz);
	group_basic->Add(text_ratio_equals);
	group_basic->Add(text_ratio_value);

	Add(group_basic);

	OnChangeTempo(slider_tempo->GetValue());

	OnChangePitchOption();
	OnEditRatio();
	OnChangeSemitones(slider_semitones->GetValue());

	/* Tuning options.
	 */
	group_tuning		= new GroupBox(i18n->TranslateString("Advanced options"), Point(7, 114), Size(400, 228));

	text_detector		= new Text(i18n->AddColon(i18n->TranslateString("Transient detection")), Point(10, 15));
	combo_detector		= new ComboBox(Point(10, 12), Size(300, 0));
	combo_detector->AddEntry(i18n->TranslateString("Compound"));
	combo_detector->AddEntry(i18n->TranslateString("Percussive"));
	combo_detector->AddEntry(i18n->TranslateString("Soft"));
	combo_detector->SelectNthEntry(config->GetIntValue(ConfigID, "Detector", 0));

	text_transients		= new Text(i18n->AddColon(i18n->TranslateString("Transient handling")), Point(10, 42));
	combo_transients	= new ComboBox(Point(10, 39), Size(300, 0));
	combo_transients->AddEntry(i18n->TranslateString("Crisp"));
	combo_transients->AddEntry(i18n->TranslateString("Mixed"));
	combo_transients->AddEntry(i18n->TranslateString("Smooth"));
	combo_transients->SelectNthEntry(config->GetIntValue(ConfigID, "Transients", 0));

	text_window		= new Text(i18n->AddColon(i18n->TranslateString("Transformation window")), Point(10, 69));
	combo_window		= new ComboBox(Point(10, 66), Size(300, 0));
	combo_window->AddEntry(i18n->TranslateString("Standard"));
	combo_window->AddEntry(i18n->TranslateString("Short"));
	combo_window->AddEntry(i18n->TranslateString("Long"));
	combo_window->SelectNthEntry(config->GetIntValue(ConfigID, "Window", 0));

	check_smoothing		= new CheckBox(i18n->TranslateString("Use time-domain smoothing"), Point(10, 93), Size(200, 0), &smoothing);

	text_phase		= new Text(i18n->AddColon(i18n->TranslateString("Phase handling")), Point(10, 121));
	combo_phase		= new ComboBox(Point(10, 118), Size(300, 0));
	combo_phase->AddEntry(i18n->TranslateString("Laminar"));
	combo_phase->AddEntry(i18n->TranslateString("Independent"));
	combo_phase->SelectNthEntry(config->GetIntValue(ConfigID, "Phase", 0));

	text_formant		= new Text(i18n->AddColon(i18n->TranslateString("Formant handling")), Point(10, 148));
	combo_formant		= new ComboBox(Point(10, 145), Size(300, 0));
	combo_formant->AddEntry(i18n->TranslateString("Shift"));
	combo_formant->AddEntry(i18n->TranslateString("Preserve"));
	combo_formant->SelectNthEntry(config->GetIntValue(ConfigID, "Formant", 0));

	text_pitchmode		= new Text(i18n->AddColon(i18n->TranslateString("Pitch processing")), Point(10, 175));
	combo_pitchmode		= new ComboBox(Point(10, 172), Size(300, 0));
	combo_pitchmode->AddEntry(i18n->TranslateString("High quality"));
	combo_pitchmode->AddEntry(i18n->TranslateString("High speed"));
	combo_pitchmode->AddEntry(i18n->TranslateString("High consistency"));
	combo_pitchmode->SelectNthEntry(config->GetIntValue(ConfigID, "PitchMode", 0));

	text_channels		= new Text(i18n->AddColon(i18n->TranslateString("Stereo processing")), Point(10, 202));
	combo_channels		= new ComboBox(Point(10, 199), Size(300, 0));
	combo_channels->AddEntry(i18n->TranslateString("Separate"));
	combo_channels->AddEntry(i18n->TranslateString("Combined"));
	combo_channels->SelectNthEntry(config->GetIntValue(ConfigID, "Channels", 0));

	maxTextSize = Math::Max(Math::Max(Math::Max(text_detector->GetUnscaledTextWidth(), text_transients->GetUnscaledTextWidth()),
					  Math::Max(text_window->GetUnscaledTextWidth(), text_phase->GetUnscaledTextWidth())),
				Math::Max(Math::Max(text_formant->GetUnscaledTextWidth(), text_pitchmode->GetUnscaledTextWidth()), text_channels->GetUnscaledTextWidth()));

	combo_detector->SetX(maxTextSize + 16);
	combo_transients->SetX(maxTextSize + 16);
	combo_window->SetX(maxTextSize + 16);
	combo_phase->SetX(maxTextSize + 16);
	combo_formant->SetX(maxTextSize + 16);
	combo_pitchmode->SetX(maxTextSize + 16);
	combo_channels->SetX(maxTextSize + 16);

	check_smoothing->SetX(maxTextSize + 16);

	combo_detector->SetWidth(group_tuning->GetWidth() - maxTextSize - 26);
	combo_transients->SetWidth(group_tuning->GetWidth() - maxTextSize - 26);
	combo_window->SetWidth(group_tuning->GetWidth() - maxTextSize - 26);
	combo_phase->SetWidth(group_tuning->GetWidth() - maxTextSize - 26);
	combo_formant->SetWidth(group_tuning->GetWidth() - maxTextSize - 26);
	combo_pitchmode->SetWidth(group_tuning->GetWidth() - maxTextSize - 26);
	combo_channels->SetWidth(group_tuning->GetWidth() - maxTextSize - 26);

	check_smoothing->SetWidth(group_tuning->GetWidth() - maxTextSize - 26);

	group_tuning->Add(text_detector);
	group_tuning->Add(combo_detector);

	group_tuning->Add(text_transients);
	group_tuning->Add(combo_transients);

	group_tuning->Add(text_window);
	group_tuning->Add(combo_window);
	group_tuning->Add(check_smoothing);

	group_tuning->Add(text_phase);
	group_tuning->Add(combo_phase);

	group_tuning->Add(text_formant);
	group_tuning->Add(combo_formant);

	group_tuning->Add(text_pitchmode);
	group_tuning->Add(combo_pitchmode);

	group_tuning->Add(text_channels);
	group_tuning->Add(combo_channels);

	Add(group_tuning);

	SetSize(Size(414, 349));
}

BoCA::ConfigureRubberBand::~ConfigureRubberBand()
{
	DeleteObject(group_basic);

	DeleteObject(text_tempo);
	DeleteObject(slider_tempo);
	DeleteObject(text_tempo_value);

	DeleteObject(text_pitch);

	DeleteObject(option_semitones);
	DeleteObject(slider_semitones);
	DeleteObject(text_semitones_value);

	DeleteObject(option_ratio);
	DeleteObject(edit_ratio_num);
	DeleteObject(text_ratio_num_hz);
	DeleteObject(text_ratio_colon);
	DeleteObject(edit_ratio_den);
	DeleteObject(text_ratio_den_hz);
	DeleteObject(text_ratio_equals);
	DeleteObject(text_ratio_value);

	DeleteObject(group_tuning);

	DeleteObject(text_detector);
	DeleteObject(combo_detector);

	DeleteObject(text_transients);
	DeleteObject(combo_transients);

	DeleteObject(text_window);
	DeleteObject(combo_window);
	DeleteObject(check_smoothing);

	DeleteObject(text_phase);
	DeleteObject(combo_phase);

	DeleteObject(text_formant);
	DeleteObject(combo_formant);

	DeleteObject(text_pitchmode);
	DeleteObject(combo_pitchmode);

	DeleteObject(text_channels);
	DeleteObject(combo_channels);
}

Int BoCA::ConfigureRubberBand::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "Stretch", Math::Round(1000.0 / (tempo / 100.0)));

	config->SetIntValue(ConfigID, "PitchOption", pitchOption);
	config->SetIntValue(ConfigID, "RatioNum", Math::Round(edit_ratio_num->GetText().ToFloat() * 10000.0));
	config->SetIntValue(ConfigID, "RatioDen", Math::Round(edit_ratio_den->GetText().ToFloat() * 10000.0));
	config->SetIntValue(ConfigID, "Pitch", Math::Round(Math::Pow(2.0, pitch / 2.0 / 12.0) * 1000.0));

	config->SetIntValue(ConfigID, "Detector", combo_detector->GetSelectedEntryNumber());
	config->SetIntValue(ConfigID, "Transients", combo_transients->GetSelectedEntryNumber());
	config->SetIntValue(ConfigID, "Window", combo_window->GetSelectedEntryNumber());
	config->SetIntValue(ConfigID, "Phase", combo_phase->GetSelectedEntryNumber());
	config->SetIntValue(ConfigID, "Formant", combo_formant->GetSelectedEntryNumber());
	config->SetIntValue(ConfigID, "PitchMode", combo_pitchmode->GetSelectedEntryNumber());
	config->SetIntValue(ConfigID, "Channels", combo_channels->GetSelectedEntryNumber());

	config->SetIntValue(ConfigID, "Smoothing", smoothing);

	return Success();
}

Void BoCA::ConfigureRubberBand::OnChangeTempo(Int tempo)
{
	I18n	*i18n = I18n::Get();

	text_tempo_value->SetText(i18n->TranslateString("%1%", "Technical").Replace("%1", String(tempo >= 100 ? "+" : NIL).Append(String::FromInt(tempo - 100))));
}

Void BoCA::ConfigureRubberBand::OnChangePitchOption()
{
	if (pitchOption == 0)
	{
		edit_ratio_num->Deactivate();
		text_ratio_num_hz->Deactivate();
		text_ratio_colon->Deactivate();
		edit_ratio_den->Deactivate();
		text_ratio_den_hz->Deactivate();
		text_ratio_equals->Deactivate();
		text_ratio_value->Deactivate();

		slider_semitones->Activate();
		text_semitones_value->Activate();
	}
	else
	{
		slider_semitones->Deactivate();
		text_semitones_value->Deactivate();

		edit_ratio_num->Activate();
		text_ratio_num_hz->Activate();
		text_ratio_colon->Activate();
		edit_ratio_den->Activate();
		text_ratio_den_hz->Activate();
		text_ratio_equals->Activate();
		text_ratio_value->Activate();
	}
}

Void BoCA::ConfigureRubberBand::OnEditRatio()
{
	I18n	*i18n = I18n::Get();

	Float	 ratioNum = edit_ratio_num->GetText().ToFloat();
	Float	 ratioDen = edit_ratio_den->GetText().ToFloat();

	if (ratioNum <= 0.0 || ratioDen <= 0.0)	text_ratio_value->SetText(i18n->TranslateString("invalid"));
	else					text_ratio_value->SetText(String::FromFloat(Math::Round(ratioNum / ratioDen * 10000.0) / 10000.0));
}

Void BoCA::ConfigureRubberBand::OnChangeSemitones(Int pitch)
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("DSP::RubberBand");

	text_semitones_value->SetText(String(pitch >= 0 ? "+" : NIL).Append(String::FromFloat(pitch / 2.0)).Append(pitch % 2 == 0 ? ".0" : NIL));
}
