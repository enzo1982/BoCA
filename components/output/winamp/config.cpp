 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"
#include "dllinterface.h"

BoCA::ConfigureWinampOut::ConfigureWinampOut()
{
	Point	 pos;
	Size	 size;

	Config	*config = Config::Get();
	I18n	*i18n = I18n::Get();

	pos.x	= 7;
	pos.y	= 7;
	size.cx	= 425;
	size.cy	= 170;

	list_output		= new ListBox(pos, size);
	list_output->SetFlags(LF_MULTICHECKBOX);
	list_output->onSelectEntry.Connect(&ConfigureWinampOut::SelectOutputPlugin, this);
	list_output->onMarkEntry.Connect(&ConfigureWinampOut::SelectOutputPlugin, this);

	pos.x	+= 433;
	size.cx	= 0;
	size.cy	= 0;

	button_output		= new Button(i18n->TranslateString("Configure"), NIL, pos, size);
	button_output->onAction.Connect(&ConfigureWinampOut::ConfigureOutputPlugin, this);
	button_output->Deactivate();

	pos.y += 30;

	button_output_about	= new Button(i18n->TranslateString("About"), NIL, pos, size);
	button_output_about->onAction.Connect(&ConfigureWinampOut::AboutOutputPlugin, this);
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

BoCA::ConfigureWinampOut::~ConfigureWinampOut()
{
	DeleteObject(list_output);
	DeleteObject(button_output);
	DeleteObject(button_output_about);
}

Int BoCA::ConfigureWinampOut::SaveSettings()
{
	Config	*config = Config::Get();
	Int	 plugin = -1;

	for (Int i = 0; i < list_output->Length(); i++) if (list_output->GetNthEntry(i)->IsMarked()) plugin = i;

	config->SetIntValue("WinampOut", "OutputPlugin", plugin);

	return Success();
}

Void BoCA::ConfigureWinampOut::SelectOutputPlugin()
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

Void BoCA::ConfigureWinampOut::ConfigureOutputPlugin()
{
	if (list_output->GetSelectedEntry() == NIL) return;

	winamp_out_modules.GetNth(list_output->GetSelectedEntryNumber())->Config((HWND) GetContainerWindow()->GetSystemWindow());
}

Void BoCA::ConfigureWinampOut::AboutOutputPlugin()
{
	if (list_output->GetSelectedEntry() == NIL) return;

	winamp_out_modules.GetNth(list_output->GetSelectedEntryNumber())->About((HWND) GetContainerWindow()->GetSystemWindow());
}
