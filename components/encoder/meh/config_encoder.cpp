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

#include "config_encoder.h"

BoCA::ConfigureEncoder::ConfigureEncoder(AS::Component *component, const Point &pos)
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::meh!");

	layer = component->GetConfigurationLayer();

	if (layer != NIL)
	{
		mainWnd			= new Window(component->GetName(), pos, layer->GetSize() + Size(8, 73));
		mainWnd->SetRightToLeft(i18n->IsActiveLanguageRightToLeft());

		mainWnd_titlebar	= new Titlebar(TB_CLOSEBUTTON);
		divbar			= new Divider(39, OR_HORZ | OR_BOTTOM);

		btn_cancel		= new Button(i18n->TranslateString("Cancel"), Point(175, 29), Size());
		btn_cancel->onAction.Connect(&ConfigureEncoder::Cancel, this);
		btn_cancel->SetOrientation(OR_LOWERRIGHT);

		btn_ok			= new Button(i18n->TranslateString("OK"), btn_cancel->GetPosition() - Point(88, 0), Size());
		btn_ok->onAction.Connect(&ConfigureEncoder::OK, this);
		btn_ok->SetOrientation(OR_LOWERRIGHT);

		Add(mainWnd);

		mainWnd->Add(mainWnd_titlebar);
		mainWnd->Add(divbar);
		mainWnd->Add(btn_ok);
		mainWnd->Add(btn_cancel);

		mainWnd->GetMainLayer()->Add(layer);

		String	 resourcesPath;

#ifndef __WIN32__
		if (Directory(S::System::System::GetResourcesDirectory().Append("freac")).Exists()) resourcesPath = S::System::System::GetResourcesDirectory().Append("freac/").Append(Directory::GetDirectoryDelimiter());
#endif

		mainWnd->SetFlags(mainWnd->GetFlags() | WF_NOTASKBUTTON | WF_MODAL);
		mainWnd->SetIcon(ImageLoader::Load(String(resourcesPath).Append("icons/freac.png")));
	}
	else
	{
		mainWnd			= NIL;
		mainWnd_titlebar	= NIL;

		btn_cancel		= NIL;
		btn_ok			= NIL;

		divbar			= NIL;
	}
}

BoCA::ConfigureEncoder::~ConfigureEncoder()
{
	if (layer == NIL) return;

	DeleteObject(mainWnd_titlebar);
	DeleteObject(mainWnd);
	DeleteObject(btn_ok);
	DeleteObject(btn_cancel);
	DeleteObject(divbar);
}

const Error &BoCA::ConfigureEncoder::ShowDialog()
{
	if (layer != NIL) mainWnd->WaitUntilClosed();
	else		  error = Error();

	return error;
}

Void BoCA::ConfigureEncoder::OK()
{
	if (layer->SaveSettings() == Error()) return;

	mainWnd->Close();
}

Void BoCA::ConfigureEncoder::Cancel()
{
	mainWnd->Close();
}
