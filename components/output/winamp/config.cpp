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

BoCA::ConfigureWinamp::ConfigureWinamp()
{
	Config	*config = Config::Get();
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Extensions::Winamp Adapter");

	list_output		= new ListBox(Point(7, 7), Size(425, 170));
	list_output->SetFlags(LF_MULTICHECKBOX);
	list_output->onSelectEntry.Connect(&ConfigureWinamp::SelectOutputPlugin, this);
	list_output->onMarkEntry.Connect(&ConfigureWinamp::SelectOutputPlugin, this);

	button_output		= new Button(i18n->TranslateString("Configure"), NIL, Point(440, 7), Size());
	button_output->onAction.Connect(&ConfigureWinamp::ConfigureOutputPlugin, this);
	button_output->Deactivate();

	button_output_about	= new Button(i18n->TranslateString("About"), NIL, Point(440, 37), Size());
	button_output_about->onAction.Connect(&ConfigureWinamp::AboutOutputPlugin, this);
	button_output_about->Deactivate();

	for (Int l = 0; l < winamp_out_modules.Length(); l++)
	{
		ListEntry	*entry = list_output->AddEntry(winamp_out_modules.GetNth(l)->description);

		if (l == config->GetIntValue("WinampOut", "OutputPlugin", 0)) entry->SetMark(True);
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

	config->SetIntValue("WinampOut", "OutputPlugin", plugin);

	return Success();
}

Void BoCA::ConfigureWinamp::SelectOutputPlugin()
{
	if (list_output->GetSelectedEntry() == NIL) return;

	button_output->Activate();
	button_output_about->Activate();

	if (list_output->GetSelectedEntry()->IsMarked())
	{
		for (Int i = 0; i < list_output->Length(); i++) list_output->GetNthEntry(i)->SetMark(False);

		list_output->GetSelectedEntry()->SetMark(True);
		list_output->Paint(SP_PAINT);
		list_output->Paint(SP_MOUSEIN);
	}
	else
	{
		Bool	 selected = False;

		for (Int i = 0; i < list_output->Length(); i++) if (list_output->GetNthEntry(i)->IsMarked()) selected = True;

		if (!selected)
		{
			list_output->GetSelectedEntry()->SetMark(True);
			list_output->Paint(SP_PAINT);
			list_output->Paint(SP_MOUSEIN);
		}
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
