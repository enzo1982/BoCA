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

BoCA::ConfigureWinamp::ConfigureWinamp()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Extensions::Winamp Adapter");

	list_input		= new ListBox(Point(7, 7), Size(425, 170));
	list_input->onSelectEntry.Connect(&ConfigureWinamp::SelectInputPlugin, this);

	for (Int k = 0; k < winamp_in_modules.Length(); k++)
	{
		if (winamp_in_modules.GetNth(k)->version == IN_VER_OLD) list_input->AddEntry(winamp_in_modules.GetNth(k)->description);
		else							list_input->AddEntry((wchar_t *) winamp_in_modules.GetNth(k)->description);
	}

	button_input		= new Button(i18n->TranslateString("Configure"), Point(440, 7), Size());
	button_input->onAction.Connect(&ConfigureWinamp::ConfigureInputPlugin, this);
	button_input->Deactivate();

	button_input_about	= new Button(i18n->TranslateString("About"), Point(440, 37), Size());
	button_input_about->onAction.Connect(&ConfigureWinamp::AboutInputPlugin, this);
	button_input_about->Deactivate();

	Add(list_input);
	Add(button_input);
	Add(button_input_about);

	SetSize(Size(527, 184));
}

BoCA::ConfigureWinamp::~ConfigureWinamp()
{
	DeleteObject(list_input);
	DeleteObject(button_input);
	DeleteObject(button_input_about);
}

Int BoCA::ConfigureWinamp::SaveSettings()
{
	return Success();
}

Void BoCA::ConfigureWinamp::SelectInputPlugin()
{
	button_input->Activate();
	button_input_about->Activate();
}

Void BoCA::ConfigureWinamp::ConfigureInputPlugin()
{
	if (list_input->GetSelectedEntry() == NIL) return;

	winamp_in_modules.GetNth(list_input->GetSelectedEntryNumber())->Config((HWND) GetContainerWindow()->GetSystemWindow());
}

Void BoCA::ConfigureWinamp::AboutInputPlugin()
{
	if (list_input->GetSelectedEntry() == NIL) return;

	winamp_in_modules.GetNth(list_input->GetSelectedEntryNumber())->About((HWND) GetContainerWindow()->GetSystemWindow());
}
