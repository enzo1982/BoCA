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

const String	 BoCA::ConfigureWinamp::ConfigID = "WinampOut";

BoCA::ConfigureWinamp::ConfigureWinamp()
{
	const Config	*config = Config::Get();
	I18n		*i18n	= I18n::Get();

	i18n->SetContext("Extensions::Winamp Adapter");

	list_output		= new ListBox(Point(7, 7), Size(425, 170));
	list_output->SetFlags(LF_MULTICHECKBOX);
	list_output->onSelectEntry.Connect(&ConfigureWinamp::SelectOutputPlugin, this);
	list_output->onMarkEntry.Connect(&ConfigureWinamp::SelectOutputPlugin, this);

	button_output		= new Button(i18n->TranslateString("Configure"), Point(440, 7), Size());
	button_output->onAction.Connect(&ConfigureWinamp::ConfigureOutputPlugin, this);
	button_output->Deactivate();

	button_output_about	= new Button(i18n->TranslateString("About"), Point(440, 37), Size());
	button_output_about->onAction.Connect(&ConfigureWinamp::AboutOutputPlugin, this);
	button_output_about->Deactivate();

	for (Int i = 0; i < winamp_out_modules.Length(); i++)
	{
		if (winamp_out_modules.GetNth(i)->version == OUT_VER) list_output->AddEntry(winamp_out_modules.GetNth(i)->description, config->GetIntValue(ConfigID, "OutputPlugin", 0) == i);
		else						      list_output->AddEntry((wchar_t *) winamp_out_modules.GetNth(i)->description, config->GetIntValue(ConfigID, "OutputPlugin", 0) == i);
	}

	Add(list_output);
	Add(button_output);
	Add(button_output_about);

	SetSize(Size(527, 184));
}

BoCA::ConfigureWinamp::~ConfigureWinamp()
{
	DeleteObject(list_output);
	DeleteObject(button_output);
	DeleteObject(button_output_about);
}

Int BoCA::ConfigureWinamp::SaveSettings()
{
	Config	*config = Config::Get();
	Int	 plugin = -1;

	for (Int i = 0; i < list_output->Length(); i++) if (list_output->GetNthEntry(i)->IsMarked()) plugin = i;

	config->SetIntValue(ConfigID, "OutputPlugin", plugin);

	return Success();
}

Void BoCA::ConfigureWinamp::SelectOutputPlugin()
{
	ListEntry	*selectedEntry = list_output->GetSelectedEntry();

	if (selectedEntry == NIL) return;

	button_output->Activate();
	button_output_about->Activate();

	/* Make sure only one entry is marked at any time.
	 */
	if (selectedEntry->IsMarked())
	{
		/* Unmark any other plugins.
		 */
		for (Int i = 0; i < list_output->Length(); i++) if (list_output->GetNthEntry(i) != selectedEntry) list_output->GetNthEntry(i)->SetMark(False);
	}
	else
	{
		/* Mark this plugin if no other plugin is marked.
		 */
		Bool	 selected = False;

		for (Int i = 0; i < list_output->Length(); i++) if (list_output->GetNthEntry(i)->IsMarked()) selected = True;

		if (!selected) selectedEntry->SetMark(True);
	}
}

Void BoCA::ConfigureWinamp::ConfigureOutputPlugin()
{
	if (list_output->GetSelectedEntry() == NIL) return;

	winamp_out_modules.GetNth(list_output->GetSelectedEntryNumber())->Config((HWND) GetContainerWindow()->GetSystemWindow());
}

Void BoCA::ConfigureWinamp::AboutOutputPlugin()
{
	if (list_output->GetSelectedEntry() == NIL) return;

	winamp_out_modules.GetNth(list_output->GetSelectedEntryNumber())->About((HWND) GetContainerWindow()->GetSystemWindow());
}
