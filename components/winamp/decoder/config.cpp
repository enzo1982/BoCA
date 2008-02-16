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

BoCA::ConfigureWinampIn::ConfigureWinampIn()
{
	Point	 pos;
	Size	 size;

	I18n	*i18n = I18n::Get();

	pos.x	= 7;
	pos.y	= 7;
	size.cx	= 425;
	size.cy	= 170;

	list_input		= new ListBox(pos, size);
	list_input->onSelectEntry.Connect(&ConfigureWinampIn::SelectInputPlugin, this);

	for (Int k = 0; k < winamp_in_modules.Length(); k++)
	{
		list_input->AddEntry(winamp_in_modules.GetNth(k)->description);
	}

	pos.x	+= 433;
	size.cx	= 0;
	size.cy	= 0;

	button_input		= new Button(i18n->TranslateString("Configure"), NIL, pos, size);
	button_input->onAction.Connect(&ConfigureWinampIn::ConfigureInputPlugin, this);
	button_input->Deactivate();

	pos.y += 30;

	button_input_about	= new Button(i18n->TranslateString("About"), NIL, pos, size);
	button_input_about->onAction.Connect(&ConfigureWinampIn::AboutInputPlugin, this);
	button_input_about->Deactivate();

	Add(list_input);
	Add(button_input);
	Add(button_input_about);

	SetSize(Size(527, 184));
}

BoCA::ConfigureWinampIn::~ConfigureWinampIn()
{
	DeleteObject(list_input);
	DeleteObject(button_input);
	DeleteObject(button_input_about);
}

Int BoCA::ConfigureWinampIn::SaveSettings()
{
	return Success();
}

Void BoCA::ConfigureWinampIn::SelectInputPlugin()
{
	button_input->Activate();
	button_input_about->Activate();
}

Void BoCA::ConfigureWinampIn::ConfigureInputPlugin()
{
	if (list_input->GetSelectedEntry() == NIL) return;

	winamp_in_modules.GetNth(list_input->GetSelectedEntryNumber())->Config((HWND) GetContainerWindow()->GetSystemWindow());
}

Void BoCA::ConfigureWinampIn::AboutInputPlugin()
{
	if (list_input->GetSelectedEntry() == NIL) return;

	winamp_in_modules.GetNth(list_input->GetSelectedEntryNumber())->About((HWND) GetContainerWindow()->GetSystemWindow());
}
