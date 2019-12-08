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
#include "config_encoder.h"

const String	 BoCA::ConfigureMultiEncoderHub::ConfigID = "meh!";

BoCA::ConfigureMultiEncoderHub::ConfigureMultiEncoderHub()
{
	const Config	*config = Config::Get();

	separateFolders = config->GetIntValue(ConfigID, "SeparateFolders", False);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::meh!");

	group_encoders	= new GroupBox(i18n->TranslateString("Encoders"), Point(7, 11), Size(552, 185));

	text_available	= new Text(i18n->AddColon(i18n->TranslateString("Available")), Point(10, 10));

	list_available	= new ListBox(Point(10, 29), Size(245, 116));
	list_available->onSelectEntry.Connect(&ConfigureMultiEncoderHub::OnSelectAvailable, this);

	btn_add		= new Button(i18n->IsActiveLanguageRightToLeft() ? "<-" : "->", Point(263, 60), Size(26, 0));
	btn_add->onAction.Connect(&ConfigureMultiEncoderHub::OnAddEncoder, this);
	btn_add->Deactivate();

	btn_remove	= new Button(i18n->IsActiveLanguageRightToLeft() ? "->" : "<-", Point(263, 90), Size(26, 0));
	btn_remove->onAction.Connect(&ConfigureMultiEncoderHub::OnRemoveEncoder, this);
	btn_remove->Deactivate();

	text_selected	= new Text(i18n->AddColon(i18n->TranslateString("Selected")), Point(297, 10));

	list_selected	= new ListBox(Point(297, 29), Size(245, 116));
	list_selected->onSelectEntry.Connect(&ConfigureMultiEncoderHub::OnSelectEncoder, this);

	btn_configure	= new Button(i18n->TranslateString("Configure encoder"), Point(382, 153), Size(160, 0));
	btn_configure->onAction.Connect(&ConfigureMultiEncoderHub::OnConfigureEncoder, this);
	btn_configure->Deactivate();

	btn_configure->SetWidth(Math::Max(80, btn_configure->GetUnscaledTextWidth() + 14));
	btn_configure->SetX(542 - btn_configure->GetWidth());

	group_encoders->Add(text_available);
	group_encoders->Add(list_available);

	group_encoders->Add(btn_add);
	group_encoders->Add(btn_remove);

	group_encoders->Add(text_selected);
	group_encoders->Add(list_selected);

	group_encoders->Add(btn_configure);

	group_options	= new GroupBox(i18n->TranslateString("Options"), Point(7, 207), Size(552, 40));

	check_folders	= new CheckBox(i18n->TranslateString("Create a separate folder for each output format"), Point(10, 13), Size(532, 0), &separateFolders);

	group_options->Add(check_folders);

	Add(group_encoders);
	Add(group_options);

	AddEncoders();

	SetSize(Size(566, 254));
}

BoCA::ConfigureMultiEncoderHub::~ConfigureMultiEncoderHub()
{
	DeleteObject(group_encoders);

	DeleteObject(text_available);
	DeleteObject(list_available);

	DeleteObject(btn_add);
	DeleteObject(btn_remove);

	DeleteObject(text_selected);
	DeleteObject(list_selected);

	DeleteObject(btn_configure);

	DeleteObject(group_options);

	DeleteObject(check_folders);
}

Void BoCA::ConfigureMultiEncoderHub::AddEncoders()
{
	const Config	*config = Config::Get();

	AS::Registry		&boca	    = AS::Registry::Get();
	const Array<String>	&encoderIDs = config->GetStringValue(ConfigID, "Encoders", "flac-enc,lame-enc").Explode(",");

	for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != COMPONENT_TYPE_ENCODER ||
		    boca.GetComponentID(i)   == "meh-enc") continue;

		list_available->AddEntry(boca.GetComponentName(i));
	}

	foreach (const String &encoderID, encoderIDs)
	{
		if (!boca.ComponentExists(encoderID)) continue;

		Int	 entryNumber = -1;

		for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
		{
			if (boca.GetComponentType(i) != COMPONENT_TYPE_ENCODER ||
			    boca.GetComponentID(i)   == "meh-enc") continue;

			entryNumber++;

			if (boca.GetComponentID(i) != encoderID) continue;

			list_available->GetNthEntry(entryNumber)->SetHeight(0);
			list_selected->AddEntry(boca.GetComponentName(i));

			break;
		}
	}
}

Void BoCA::ConfigureMultiEncoderHub::OnSelectAvailable()
{
	btn_add->Activate();
}

Void BoCA::ConfigureMultiEncoderHub::OnSelectEncoder()
{
	btn_remove->Activate();
	btn_configure->Activate();
}

Void BoCA::ConfigureMultiEncoderHub::OnAddEncoder()
{
	Surface		*surface = GetDrawSurface();
	ListEntry	*entry	 = list_available->GetSelectedEntry();

	surface->StartPaint(Rect(list_available->GetRealPosition(), list_available->GetRealSize()));

	entry->Deselect();
	entry->SetHeight(0);

	list_available->Paint(SP_PAINT);

	surface->EndPaint();

	list_selected->AddEntry(entry->GetText());

	btn_add->Deactivate();
}

Void BoCA::ConfigureMultiEncoderHub::OnRemoveEncoder()
{
	Surface		*surface = GetDrawSurface();
	ListEntry	*entry	 = list_selected->GetSelectedEntry();

	surface->StartPaint(Rect(list_available->GetRealPosition(), list_available->GetRealSize()));

	for (Int i = 0; i < list_available->Length(); i++)
	{
		ListEntry	*nthEntry = list_available->GetNthEntry(i);

		if (nthEntry->GetHeight() != 0)		     continue;
		if (nthEntry->GetText() != entry->GetText()) continue;

		nthEntry->SetHeight(16);

		list_available->Paint(SP_PAINT);
	}

	surface->EndPaint();

	list_selected->Remove(entry);

	btn_remove->Deactivate();
	btn_configure->Deactivate();
}

Void BoCA::ConfigureMultiEncoderHub::OnConfigureEncoder()
{
	AS::Registry	&boca = AS::Registry::Get();
	String		 encoderID;

	for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != COMPONENT_TYPE_ENCODER ||
		    boca.GetComponentID(i)   == "meh-enc") continue;

		if (list_selected->GetSelectedEntry()->GetText() == boca.GetComponentName(i))
		{
			encoderID = boca.GetComponentID(i);

			break;
		}
	}

	AS::Component	*component = boca.CreateComponentByID(encoderID);

	if (component != NIL)
	{
		Point	 position = GetContainerWindow()->GetPosition() + Point(60, 60);

		if (ConfigureEncoder(component, position).ShowDialog() == Error()) Utilities::ErrorMessage("No configuration dialog available for:\n\n%1", component->GetName());

		boca.DeleteComponent(component);
	}
}

Int BoCA::ConfigureMultiEncoderHub::SaveSettings()
{
	Config	*config = Config::Get();

	config->SetIntValue(ConfigID, "SeparateFolders", separateFolders);

	AS::Registry	&boca	     = AS::Registry::Get();
	Int		 entryNumber = 0;
	String		 encoders;

	for (Int i = 0; i < boca.GetNumberOfComponents(); i++)
	{
		if (boca.GetComponentType(i) != COMPONENT_TYPE_ENCODER ||
		    boca.GetComponentID(i)   == "meh-enc") continue;

		if (list_available->GetNthEntry(entryNumber++)->GetHeight() != 0) continue;

		encoders.Append(encoders.Length() > 0 ? "," : NIL).Append(boca.GetComponentID(i));
	}

	config->SetStringValue(ConfigID, "Encoders", encoders);

	return Success();
}
