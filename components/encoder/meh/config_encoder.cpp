 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config_encoder.h"

BoCA::ConfigureEncoder::ConfigureEncoder(ConfigLayer *iLayer, const Point &pos)
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Encoders::meh!");

	layer			= iLayer;

	mainWnd			= new Window(i18n->TranslateString("Encoder configuration"), pos, layer->GetSize() + Size(8, 73));
	mainWnd->SetRightToLeft(i18n->IsActiveLanguageRightToLeft());

	mainWnd_titlebar	= new Titlebar(TB_CLOSEBUTTON);
	divbar			= new Divider(39, OR_HORZ | OR_BOTTOM);

	btn_cancel		= new Button(i18n->TranslateString("Cancel"), NIL, Point(175, 29), Size());
	btn_cancel->onAction.Connect(&ConfigureEncoder::Cancel, this);
	btn_cancel->SetOrientation(OR_LOWERRIGHT);

	btn_ok			= new Button(i18n->TranslateString("OK"), NIL, btn_cancel->GetPosition() - Point(88, 0), Size());
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
	if (Directory(GUI::Application::GetApplicationDirectory().Append("../share/freac")).Exists()) resourcesPath = "../share/freac/";
#endif

	mainWnd->SetFlags(mainWnd->GetFlags() | WF_NOTASKBUTTON | WF_MODAL);
	mainWnd->SetIcon(ImageLoader::Load(String(resourcesPath).Append("icons/freac.png")));
}

BoCA::ConfigureEncoder::~ConfigureEncoder()
{
	DeleteObject(mainWnd_titlebar);
	DeleteObject(mainWnd);
	DeleteObject(btn_ok);
	DeleteObject(btn_cancel);
	DeleteObject(divbar);
}

const Error &BoCA::ConfigureEncoder::ShowDialog()
{
	mainWnd->WaitUntilClosed();

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
