 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dialog.h"

BoCA::DonateDialog::DonateDialog()
{
	Config	*config = Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Extensions::Donate");

	mainWnd			= new GUI::Window(i18n->TranslateString("Support this project"), Point(100, 100), Size(380, 350));
	mainWnd->SetRightToLeft(i18n->IsActiveLanguageRightToLeft());

	mainWnd_titlebar	= new Titlebar(TB_NONE);
	divider			= new Divider(39, OR_HORZ | OR_BOTTOM);

	mainWnd->Add(mainWnd_titlebar);
	mainWnd->Add(divider);

	button_close		= new Button(i18n->TranslateString("Close"), NIL, Point(87, 29), Size());
	button_close->onAction.Connect(&DonateDialog::Close, this);
	button_close->SetOrientation(OR_LOWERRIGHT);

	mainWnd->Add(button_close);

	text_donate		= new Text(String(i18n->TranslateString("Please help keep this software free by supporting the %1 project\nwith a donation.\n\nClick one of the buttons below to make a donation using PayPal:")).Replace("%1", "fre:ac"), Point(7, 45));
	text_donate_other	= new Text(String(i18n->TranslateString("You can also send money directly to %1 using PayPal.\n\nPlease write to %1 if you would like to donate using\nelectronic transfer, mail a cheque or send money by mail.")).Replace("%1", "donate@freac.org"), Point(7, text_donate->textSize.cy + 116));

	text_intro		= new Text(i18n->TranslateString("Please support this project!"), Point(7, 12));
	text_intro->SetFont(Font(Font::Default, 12, Font::Bold));
	text_intro->SetX((Math::Max(text_donate->textSize.cx, text_donate_other->textSize.cx) + 9 - text_intro->textSize.cx) / 2);

	text_thanks		= new Text(i18n->TranslateString("Thank you very much!"), Point(7, text_donate->textSize.cy + text_donate_other->textSize.cy + 129));
	text_thanks->SetFont(Font(Font::Default, 12, Font::Bold));
	text_thanks->SetX((Math::Max(text_donate->textSize.cx, text_donate_other->textSize.cx) + 9 - text_thanks->textSize.cx) / 2);

	mainWnd->Add(text_intro);
	mainWnd->Add(text_donate);
	mainWnd->Add(text_donate_other);
	mainWnd->Add(text_thanks);

	link_donate_5		= new Hyperlink(NIL, ImageLoader::Load(File(String(Application::GetApplicationDirectory()).Append("boca/boca.extension.donate/donate_").Append(i18n->TranslateString("usd")).Append("_5.png"))), String("file:///").Append(Application::GetApplicationDirectory().Replace("\\", "/")).Append("boca/boca.extension.donate/donate_").Append(i18n->TranslateString("usd")).Append("_5.html"), Point((Math::Max(text_donate->textSize.cx, text_donate_other->textSize.cx) + 21) / 2 - 100, text_donate->textSize.cy + 56));
	link_donate_10		= new Hyperlink(NIL, ImageLoader::Load(File(String(Application::GetApplicationDirectory()).Append("boca/boca.extension.donate/donate_").Append(i18n->TranslateString("usd")).Append("_10.png"))), String("file:///").Append(Application::GetApplicationDirectory().Replace("\\", "/")).Append("boca/boca.extension.donate/donate_").Append(i18n->TranslateString("usd")).Append("_10.html"), Point((Math::Max(text_donate->textSize.cx, text_donate_other->textSize.cx) + 21) / 2 - 30, text_donate->textSize.cy + 56));
	link_donate_other	= new Hyperlink(NIL, ImageLoader::Load(File(String(Application::GetApplicationDirectory()).Append("boca/boca.extension.donate/donate_other_").Append(i18n->TranslateString("en")).Append(".png"))), String("file:///").Append(Application::GetApplicationDirectory().Replace("\\", "/")).Append("boca/boca.extension.donate/donate_").Append(i18n->TranslateString("usd")).Append("_other.html"), Point((Math::Max(text_donate->textSize.cx, text_donate_other->textSize.cx) + 21) / 2 + 40, text_donate->textSize.cy + 56));

	mainWnd->Add(link_donate_5);
	mainWnd->Add(link_donate_10);
	mainWnd->Add(link_donate_other);

	remind = config->GetIntValue("Donate", "ShowAgain", True);

	check_remind		= new CheckBox(i18n->TranslateString("Remind me later"), Point(7, 27), Size(), &remind);
	check_remind->SetOrientation(OR_LOWERLEFT);
	check_remind->SetWidth(check_remind->textSize.cx + 23);

	mainWnd->Add(check_remind);

	Add(mainWnd);

	mainWnd->SetFlags(mainWnd->GetFlags() | WF_TOPMOST | WF_NOTASKBUTTON);
	mainWnd->SetIcon(ImageLoader::Load("freac.pci:0"));

	Rect	 workArea = MultiMonitor::GetActiveMonitorWorkArea();

	Size	 wndSize  = Size(Math::Max(text_donate->textSize.cx, text_donate_other->textSize.cx) + 21, text_donate->textSize.cy + text_donate_other->textSize.cy + 236);
	Point	 wndPos	  = workArea.GetPosition() + Point((workArea.GetSize().cx - wndSize.cx) / 2, (workArea.GetSize().cy - wndSize.cy) / 2);

	mainWnd->SetMetrics(wndPos, wndSize);
}

BoCA::DonateDialog::~DonateDialog()
{
	DeleteObject(mainWnd);
	DeleteObject(mainWnd_titlebar);

	DeleteObject(divider);

	DeleteObject(button_close);

	DeleteObject(text_intro);
	DeleteObject(text_donate);
	DeleteObject(text_donate_other);
	DeleteObject(text_thanks);

	DeleteObject(link_donate_5);
	DeleteObject(link_donate_10);
	DeleteObject(link_donate_other);

	DeleteObject(check_remind);
}

const Error &BoCA::DonateDialog::ShowDialog()
{
	mainWnd->Show();

	return error;
}

Void BoCA::DonateDialog::Close()
{
	Config	*config = Config::Get();

	mainWnd->Close();

	config->SetIntValue("Donate", "ShowAgain", remind);
}
