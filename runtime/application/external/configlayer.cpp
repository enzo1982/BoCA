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

#include <boca/application/external/configlayer.h>
#include <boca/common/config.h>
#include <boca/common/i18n.h>

BoCA::AS::ConfigLayerExternal::ConfigLayerExternal(ComponentSpecs *iSpecs)
{
	specs = iSpecs;

	Config	*config = Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Configuration");

	group_parameters = new GroupBox(i18n->TranslateString("Command line arguments"), Point(6, 10), Size(400, 140));

	text_commandline = new Text(i18n->AddColon(i18n->TranslateString("Resulting arguments string")), Point(6, 45));
	text_commandline->SetOrientation(OR_LOWERLEFT);

	edit_commandline = new EditBox(Point(6, 26), Size(400, 0));
	edit_commandline->SetOrientation(OR_LOWERLEFT);
	edit_commandline->Deactivate();

	/* Create dynamic widgets.
	 */
	Point	 position    = Point(10, 13);
	Int	 maxTextSize = 0;

	i18n->SetContext(String("External::").Append(specs->name));

	foreach (Parameter *param, specs->parameters)
	{
		CheckBox	*checkBox = new CheckBox(i18n->TranslateString(param->GetName()), position, Size(100, 0));
		Layer		*layer	  = new Layer(param->GetName());

		checkBox->onAction.Connect(&ConfigLayerExternal::OnSelectParameter, this);
		checkBox->onAction.Connect(&ConfigLayerExternal::OnUpdateParameterValue, this);

		layer->SetMetrics(position + Point(230, -1), Size(230, 39));
		layer->SetOrientation(OR_UPPERRIGHT);

		switch (param->GetType())
		{
			case PARAMETER_TYPE_SWITCH:
				checkBox->SetChecked(config->GetIntValue(specs->id, param->GetName(), param->GetEnabled()));

				position += Point(0, 25);
				layer->SetHeight(25);

				break;
			case PARAMETER_TYPE_SELECTION:
				checkBox->SetText(i18n->AddColon(checkBox->GetText()));
				checkBox->SetChecked(config->GetIntValue(specs->id, String("Set ").Append(param->GetName()), param->GetEnabled()));

				maxTextSize = Math::Max(maxTextSize, checkBox->GetUnscaledTextWidth());

				{
					ComboBox	*selection     = new ComboBox(Point(0, 0), Size(230, 0));
					String		 selectedValue = config->GetStringValue(specs->id, param->GetName(), param->GetDefault());

					foreach (Option *option, param->GetOptions())
					{
						ListEntry	*entry = selection->AddEntry(i18n->TranslateString(option->GetAlias()).Replace("%1", option->GetValue()));

						if (selectedValue == option->GetValue()) selection->SelectEntry(entry);
					}

					/* Select first entry if no default was provided
					 */
					if (selectedValue == NIL) selection->SelectNthEntry(0);

					selection->onSelectEntry.Connect(&ConfigLayerExternal::OnUpdateParameterValue, this);

					layer->Add(selection);

					group_parameters->Add(layer);

					widgets_parameters.Add(selection);
				}

				position += Point(0, 26);
				layer->SetHeight(26);

				break;
			case PARAMETER_TYPE_RANGE:
				checkBox->SetText(i18n->AddColon(checkBox->GetText()));
				checkBox->SetChecked(config->GetIntValue(specs->id, String("Set ").Append(param->GetName()), param->GetEnabled()));

				maxTextSize = Math::Max(maxTextSize, checkBox->GetUnscaledTextWidth());

				{
					Int	 min = 0;
					Int	 max = 0;

					String	 minAlias;
					String	 maxAlias;

					foreach (Option *option, param->GetOptions())
					{
						if (option->GetType() == OPTION_TYPE_MIN)
						{
							min	 = Math::Round(option->GetValue().ToFloat() / param->GetStepSize());
							minAlias = i18n->TranslateString(option->GetAlias()).Replace("%1", option->GetValue());
						}

						if (option->GetType() == OPTION_TYPE_MAX)
						{
							max	 = Math::Round(option->GetValue().ToFloat() / param->GetStepSize());
							maxAlias = i18n->TranslateString(option->GetAlias()).Replace("%1", option->GetValue());
						}
					}

					Slider	*range = new Slider(Point(0, 1), Size(210, 0), OR_HORZ, NIL, min, max);
					Text	*value = new Text(String::FromFloat(max * param->GetStepSize()).Append(param->GetStepSize() < 1 ? ".0" : NIL), Point(220, 3));

					range->SetWidth(222 - value->GetUnscaledTextWidth());

					range->SetValue(config->GetIntValue(specs->id, param->GetName(), param->GetDefault().ToFloat() / param->GetStepSize()));

					range->onValueChange.Connect(&ConfigLayerExternal::OnSliderValueChange, this);
					range->onValueChange.Connect(&ConfigLayerExternal::OnUpdateParameterValue, this);

					Text	*minText = new Text(minAlias, Point(0, 20));
					Text	*maxText = new Text(maxAlias, Point(0, 20));

					maxText->SetPosition(Point(221 - value->GetUnscaledTextWidth() - maxText->GetUnscaledTextWidth(), 20));

					layer->Add(range);
					layer->Add(value);
					layer->Add(minText);
					layer->Add(maxText);

					group_parameters->Add(layer);

					widgets_parameters.Add(range);
					widgets_parameters.Add(value);
					widgets_parameters.Add(minText);
					widgets_parameters.Add(maxText);
				}

				position += Point(0, 39);
				layer->SetHeight(39);

				break;
			default:
				/* Unsupported parameter type.
				 */
				break;
		}

		group_parameters->Add(checkBox);

		checks_parameters.Add(checkBox);
		layers_parameters.Add(layer);
	}

	/* Create additional arguments fields.
	 */
	i18n->SetContext("Configuration");

	check_additional = new CheckBox(i18n->AddColon(i18n->TranslateString("Additional arguments")), position, Size(100, 0));
	check_additional->SetChecked(config->GetIntValue(specs->id, "Set Additional arguments", False));
	check_additional->onAction.Connect(&ConfigLayerExternal::OnSelectParameter, this);
	check_additional->onAction.Connect(&ConfigLayerExternal::OnUpdateParameterValue, this);

	edit_additional	 = new EditBox(position + Point(230, -1), Size(230, 0));
	edit_additional->SetOrientation(OR_UPPERRIGHT);
	edit_additional->SetText(config->GetStringValue(specs->id, "Additional arguments", NIL));
	edit_additional->onInput.Connect(&ConfigLayerExternal::OnUpdateParameterValue, this);

	maxTextSize = Math::Max(maxTextSize, check_additional->GetUnscaledTextWidth());
	position += Point(0, 26);

	group_parameters->Add(check_additional);
	group_parameters->Add(edit_additional);

	/* Adjust check box sizes.
	 */
	foreach (Parameter *param, specs->parameters)
	{
		CheckBox	*checkBox = checks_parameters.GetNth(foreachindex);

		if (param->GetType() == PARAMETER_TYPE_SWITCH) checkBox->SetWidth(maxTextSize + 259);
		else					       checkBox->SetWidth(maxTextSize + 21);
	}

	check_additional->SetWidth(maxTextSize + 21);

	group_parameters->SetSize(Size(279 + maxTextSize, position.y + 2));
	edit_commandline->SetWidth(279 + maxTextSize);

	Add(group_parameters);

	Add(text_commandline);
	Add(edit_commandline);

	OnSliderValueChange();
	OnSelectParameter();
	OnUpdateParameterValue();

	SetSize(Size(291 + maxTextSize, group_parameters->GetHeight() + 60));
}

BoCA::AS::ConfigLayerExternal::~ConfigLayerExternal()
{
	foreach (CheckBox *checkBox, checks_parameters) DeleteObject(checkBox);
	foreach (Layer *layer, layers_parameters) DeleteObject(layer);
	foreach (Widget *widget, widgets_parameters) DeleteObject(widget);

	DeleteObject(group_parameters);

	DeleteObject(check_additional);
	DeleteObject(edit_additional);

	DeleteObject(text_commandline);
	DeleteObject(edit_commandline);
}

Int BoCA::AS::ConfigLayerExternal::SaveSettings()
{
	Config	*config = Config::Get();

	foreach (const Parameter *param, specs->parameters)
	{
		CheckBox	*checkBox = checks_parameters.GetNth(foreachindex);

		switch (param->GetType())
		{
			case PARAMETER_TYPE_SWITCH:
				config->SetIntValue(specs->id, param->GetName(), checkBox->IsChecked() ? 1 : 0);

				break;
			case PARAMETER_TYPE_SELECTION:
				{
					Layer	*layer = GetParameterLayer(param->GetName());

					if (layer == NIL) break;

					ComboBox	*selection = (ComboBox *) layer->GetNthObject(0);
					Option		*option	   = param->GetOptions().GetNth(selection->GetSelectedEntryNumber());

					if (option != NIL)
					{
						config->SetIntValue(specs->id, String("Set ").Append(param->GetName()), checkBox->IsChecked() ? 1 : 0);
						config->SetStringValue(specs->id, param->GetName(), option->GetValue());
					}
				}

				break;
			case PARAMETER_TYPE_RANGE:
				{
					Layer	*layer = GetParameterLayer(param->GetName());

					if (layer == NIL) break;

					Slider		*range = (Slider *) layer->GetNthObject(0);

					config->SetIntValue(specs->id, String("Set ").Append(param->GetName()), checkBox->IsChecked() ? 1 : 0);
					config->SetIntValue(specs->id, param->GetName(), range->GetValue());
				}

				break;
			default:
				/* Unsupported parameter type.
				 */
				break;
		}
	}

	config->SetIntValue(specs->id, "Set Additional arguments", check_additional->IsChecked() ? 1 : 0);
	config->SetStringValue(specs->id, "Additional arguments", edit_additional->GetText());

	return Success();
}

Layer *BoCA::AS::ConfigLayerExternal::GetParameterLayer(const String &name)
{
	foreach (Layer *layer, layers_parameters)
	{
		if (layer->GetText() == name) return layer;
	}

	return NIL;
}

String BoCA::AS::ConfigLayerExternal::GetArgumentsString()
{
	/* Still initializing?
	 */
	if (checks_parameters.Length() < specs->parameters.Length()) return NIL;

	String	 arguments;

	foreach (const Parameter *param, specs->parameters)
	{
		CheckBox	*checkBox = checks_parameters.GetNth(foreachindex);

		if (!checkBox->IsChecked()) continue;

		switch (param->GetType())
		{
			case PARAMETER_TYPE_SWITCH:
				arguments.Append(param->GetArgument()).Append(" ");

				break;
			case PARAMETER_TYPE_SELECTION:
				{
					Layer	*layer = GetParameterLayer(param->GetName());

					if (layer == NIL) break;

					ComboBox	*selection = (ComboBox *) layer->GetNthObject(0);
					Option		*option	   = param->GetOptions().GetNth(selection->GetSelectedEntryNumber());

					if (option != NIL) arguments.Append(param->GetArgument().Replace("%VALUE", option->GetValue())).Append(" ");
				}

				break;
			case PARAMETER_TYPE_RANGE:
				{
					Layer	*layer = GetParameterLayer(param->GetName());

					if (layer == NIL) break;

					Slider		*range = (Slider *) layer->GetNthObject(0);

					arguments.Append(param->GetArgument().Replace("%VALUE", String::FromFloat(range->GetValue() * param->GetStepSize()))).Append(" ");
				}

				break;
			default:
				/* Unsupported parameter type.
				 */
				break;
		}
	}

	if (check_additional->IsChecked()) arguments.Append(" ").Append(edit_additional->GetText());

	return arguments;
}

Void BoCA::AS::ConfigLayerExternal::OnSelectParameter()
{
	foreach (CheckBox *checkBox, checks_parameters)
	{
		Layer	*layer = layers_parameters.GetNth(foreachindex);

		if (layer == NIL) continue;

		if (checkBox->IsChecked()) layer->Activate();
		else			   layer->Deactivate();
	}

	if (check_additional->IsChecked()) edit_additional->Activate();
	else				   edit_additional->Deactivate();
}

Void BoCA::AS::ConfigLayerExternal::OnUpdateParameterValue()
{
	edit_commandline->SetText(GetArgumentsString());
}

Void BoCA::AS::ConfigLayerExternal::OnSliderValueChange()
{
	foreach (const Parameter *param, specs->parameters)
	{
		switch (param->GetType())
		{
			case PARAMETER_TYPE_RANGE:
				{
					Layer	*layer = GetParameterLayer(param->GetName());

					if (layer == NIL) break;

					Slider		*range = (Slider *) layer->GetNthObject(0);
					Text		*value = (Text *) layer->GetNthObject(1);

					value->SetText(String::FromFloat(range->GetValue() * param->GetStepSize()).Append(param->GetStepSize() < 1 && range->GetValue() % 10 == 0 ? ".0" : NIL));
					value->SetPosition(Point(230 - value->GetUnscaledTextWidth(), 3));
				}

				break;
			default:
				/* Only interested in slider ranges here.
				 */
				break;
		}
	}
}
