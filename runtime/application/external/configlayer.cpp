 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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

	text_parameters = new Text(i18n->AddColon(i18n->TranslateString("Command line arguments")), Point(6, 6));

	list_parameters	= new ListBox(Point(6, 24), Size(250, 64));
	list_parameters->SetFlags(LF_MULTICHECKBOX);
	list_parameters->onSelectEntry.Connect(&ConfigLayerExternal::OnSelectParameter, this);
	list_parameters->onMarkEntry.Connect(&ConfigLayerExternal::OnSelectParameter, this);
	list_parameters->onMarkEntry.Connect(&ConfigLayerExternal::OnUpdateParameterValue, this);

	text_commandline = new Text(i18n->AddColon(i18n->TranslateString("Resulting arguments string")), Point(6, 156));

	edit_commandline = new EditBox(NIL, Point(6, 174), Size(250, 0));
	edit_commandline->Deactivate();

	foreach (Parameter *param, specs->external_parameters)
	{
		ListEntry	*entry = list_parameters->AddEntry(param->GetName());

		switch (param->GetType())
		{
			case PARAMETER_TYPE_SWITCH:
				entry->SetMark(config->GetIntValue(specs->id, param->GetName(), param->GetEnabled()));

				break;
			case PARAMETER_TYPE_SELECTION:
				entry->SetMark(config->GetIntValue(specs->id, String("Set ").Append(param->GetName()), param->GetEnabled()));

				{
					GroupBox	*group = new GroupBox(param->GetName(), Point(6, 100), Size(250, 40));
					ComboBox	*selection = new ComboBox(Point(10, 11), Size(230, 0));

					String		 selectedValue = config->GetStringValue(specs->id, param->GetName(), param->GetDefault());

					foreach (Option *option, param->GetOptions())
					{
						ListEntry	*entry = selection->AddEntry(option->GetAlias());

						if (selectedValue == option->GetValue()) selection->SelectEntry(entry);
					}

					/* Select first entry if no default was provided
					 */
					if (selectedValue == NIL) selection->SelectNthEntry(0);

					selection->onSelectEntry.Connect(&ConfigLayerExternal::OnUpdateParameterValue, this);

					group->Hide();
					group->Add(selection);

					Add(group);

					widgets_parameters.Add(selection);
					groups_parameters.Add(group);
				}

				break;
			case PARAMETER_TYPE_RANGE:
				entry->SetMark(config->GetIntValue(specs->id, String("Set ").Append(param->GetName()), param->GetEnabled()));

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
							minAlias = option->GetAlias();
						}

						if (option->GetType() == OPTION_TYPE_MAX)
						{
							max	 = Math::Round(option->GetValue().ToFloat() / param->GetStepSize());
							maxAlias = option->GetAlias();
						}
					}

					GroupBox	*group = new GroupBox(param->GetName(), Point(6, 100), Size(250, 50));
					Slider		*range = new Slider(Point(10, 11), Size(210, 0), OR_HORZ, NIL, min, max);
					Text		*value = new Text(String::FromFloat(max * param->GetStepSize()).Append(param->GetStepSize() < 1 ? ".0" : NIL), Point(230, 13));

					range->SetWidth(222 - value->GetUnscaledTextWidth());

					range->SetValue(config->GetIntValue(specs->id, param->GetName(), param->GetDefault().ToFloat() / param->GetStepSize()));

					range->onValueChange.Connect(&ConfigLayerExternal::OnSliderValueChange, this);
					range->onValueChange.Connect(&ConfigLayerExternal::OnUpdateParameterValue, this);

					Text		*minText = new Text(minAlias, Point(10, 30));
					Text		*maxText = new Text(maxAlias, Point(10, 30));

					maxText->SetPosition(Point(231 - value->GetUnscaledTextWidth() - maxText->GetUnscaledTextWidth(), 30));

					group->Hide();
					group->Add(range);
					group->Add(value);
					group->Add(minText);
					group->Add(maxText);

					Add(group);

					widgets_parameters.Add(range);
					widgets_parameters.Add(value);
					widgets_parameters.Add(minText);
					widgets_parameters.Add(maxText);
					groups_parameters.Add(group);
				}

				break;
			default:
				/* Unsupported parameter type.
				 */
				break;
		}
	}

	Add(text_parameters);
	Add(list_parameters);
	Add(text_commandline);
	Add(edit_commandline);

	OnSliderValueChange();
	OnUpdateParameterValue();

	SetSize(Size(262, 200));
}

BoCA::AS::ConfigLayerExternal::~ConfigLayerExternal()
{
	foreach (GroupBox *group, groups_parameters) DeleteObject(group);
	foreach (Widget *widget, widgets_parameters) DeleteObject(widget);

	DeleteObject(text_parameters);
	DeleteObject(list_parameters);
	DeleteObject(text_commandline);
	DeleteObject(edit_commandline);
}

Int BoCA::AS::ConfigLayerExternal::SaveSettings()
{
	Config	*config = Config::Get();

	for (Int i = 0; i < specs->external_parameters.Length(); i++)
	{
		Parameter	*param = specs->external_parameters.GetNth(i);
		ListEntry	*entry = list_parameters->GetNthEntry(i);

		switch (param->GetType())
		{
			case PARAMETER_TYPE_SWITCH:
				config->SetIntValue(specs->id, param->GetName(), entry->IsMarked() ? 1 : 0);

				break;
			case PARAMETER_TYPE_SELECTION:
				{
					GroupBox	*group = GetParameterGroupBox(param->GetName());
					ComboBox	*selection = (ComboBox *) group->GetNthObject(0);

					config->SetIntValue(specs->id, String("Set ").Append(param->GetName()), entry->IsMarked() ? 1 : 0);
					config->SetStringValue(specs->id, param->GetName(), param->GetOptions().GetNth(selection->GetSelectedEntryNumber())->GetValue());
				}

				break;
			case PARAMETER_TYPE_RANGE:
				{
					GroupBox	*group = GetParameterGroupBox(param->GetName());
					Slider		*range = (Slider *) group->GetNthObject(0);

					config->SetIntValue(specs->id, String("Set ").Append(param->GetName()), entry->IsMarked() ? 1 : 0);
					config->SetIntValue(specs->id, param->GetName(), range->GetValue());
				}

				break;
			default:
				/* Unsupported parameter type.
				 */
				break;
		}
	}

	return Success();
}

GroupBox *BoCA::AS::ConfigLayerExternal::GetParameterGroupBox(const String &name)
{
	foreach (GroupBox *group, groups_parameters)
	{
		if (group->GetText() == name) return group;
	}

	return NIL;
}

String BoCA::AS::ConfigLayerExternal::GetArgumentsString()
{
	/* Still initializing?
	 */
	if (list_parameters->Length() < specs->external_parameters.Length()) return NIL;

	String	 arguments;

	for (Int i = 0; i < specs->external_parameters.Length(); i++)
	{
		Parameter	*param = specs->external_parameters.GetNth(i);
		ListEntry	*entry = list_parameters->GetNthEntry(i);

		if (!entry->IsMarked()) continue;

		switch (param->GetType())
		{
			case PARAMETER_TYPE_SWITCH:
				arguments.Append(param->GetArgument()).Append(" ");

				break;
			case PARAMETER_TYPE_SELECTION:
				{
					GroupBox	*group = GetParameterGroupBox(param->GetName());

					if (group != NIL)
					{
						ComboBox	*selection = (ComboBox *) group->GetNthObject(0);

						arguments.Append(param->GetArgument().Replace("%VALUE", param->GetOptions().GetNth(selection->GetSelectedEntryNumber())->GetValue())).Append(" ");
					}
				}

				break;
			case PARAMETER_TYPE_RANGE:
				{
					GroupBox	*group = GetParameterGroupBox(param->GetName());

					if (group != NIL)
					{
						Slider		*range = (Slider *) group->GetNthObject(0);

						arguments.Append(param->GetArgument().Replace("%VALUE", String::FromFloat(range->GetValue() * param->GetStepSize()))).Append(" ");
					}
				}

				break;
			default:
				/* Unsupported parameter type.
				 */
				break;
		}
	}

	return arguments;
}

Void BoCA::AS::ConfigLayerExternal::OnSelectParameter(ListEntry *entry)
{
	foreach (GroupBox *group, groups_parameters) group->Hide();

	if (entry->IsMarked() && GetParameterGroupBox(entry->GetText()) != NIL)
	{
		GetParameterGroupBox(entry->GetText())->Show();
	}
}

Void BoCA::AS::ConfigLayerExternal::OnUpdateParameterValue()
{
	edit_commandline->SetText(GetArgumentsString());
}

Void BoCA::AS::ConfigLayerExternal::OnSliderValueChange()
{
	for (Int i = 0; i < specs->external_parameters.Length(); i++)
	{
		Parameter	*param = specs->external_parameters.GetNth(i);

		switch (param->GetType())
		{
			case PARAMETER_TYPE_RANGE:
				{
					GroupBox	*group = GetParameterGroupBox(param->GetName());
					Slider		*range = (Slider *) group->GetNthObject(0);
					Text		*value = (Text *) group->GetNthObject(1);

					value->SetText(String::FromFloat(range->GetValue() * param->GetStepSize()).Append(param->GetStepSize() < 1 && range->GetValue() % 10 == 0 ? ".0" : NIL));
					value->SetPosition(Point(240 - value->GetUnscaledTextWidth(), 13));
				}

				break;
			default:
				/* Only interested in slider ranges here.
				 */
				break;
		}
	}
}
