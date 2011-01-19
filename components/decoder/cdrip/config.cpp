 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <windows.h>

#include "config.h"

BoCA::ConfigureCDRip::ConfigureCDRip()
{
	Config	*config = Config::Get();

	setspeed	= config->GetIntValue("Ripper", "RippingSpeed", 0);
	autoRead	= config->GetIntValue("Ripper", "AutoReadContents", True);
	autoRip		= config->GetIntValue("Ripper", "AutoRip", False);

	jitter		= config->GetIntValue("CDRip", "JitterCorrection", False);
	swapchannels	= config->GetIntValue("CDRip", "SwapChannels", False);

	ntscsi		= config->GetIntValue("CDRip", "UseNTSCSI", True);

	locktray	= config->GetIntValue("CDRip", "LockTray", True);
	autoEject	= config->GetIntValue("CDRip", "EjectAfterRipping", False);

	readCDText	= config->GetIntValue("CDRip", "ReadCDText", True);
	readCDPlayerIni	= config->GetIntValue("CDRip", "ReadCDPlayerIni", True);
	readISRC	= config->GetIntValue("CDRip", "ReadISRC", False);

	cdparanoia	= config->GetIntValue("CDRip", "CDParanoia", False);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Decoders::CDRip");

	group_drive	= new GroupBox(i18n->TranslateString("Active CD-ROM drive"), Point(7, 11), Size(344, 68));

	combo_drive	= new ComboBox(Point(10, 12), Size(324, 0));

	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*info = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdrip-info");

	if (info != NIL)
	{
		for (Int i = 0; i < info->GetNumberOfDevices(); i++)
		{
			combo_drive->AddEntry(info->GetNthDeviceInfo(i).name);
		}

		boca.DeleteComponent(info);
	}

	combo_drive->SelectNthEntry(config->GetIntValue("Ripper", "ActiveDrive", 0));

	check_speed		= new CheckBox(i18n->TranslateString("Set drive speed limit:"), Point(10, 40), Size(157, 0), &setspeed);
	check_speed->onAction.Connect(&ConfigureCDRip::ToggleSetSpeed, this);

	combo_speed		= new ComboBox(Point(176, 39), Size(158, 0));

	for (Int i = 48; i > 0; i -= 4) combo_speed->AddEntry(String::FromInt(i).Append("x"));

	combo_speed->SelectNthEntry((48 - config->GetIntValue("Ripper", "RippingSpeed", 0)) / 4);

	ToggleSetSpeed();

	group_drive->Add(combo_drive);
	group_drive->Add(check_speed);
	group_drive->Add(combo_speed);

	group_cdinfo		= new GroupBox(i18n->TranslateString("CD information"), Point(7, 91), Size(344, 65));

	check_readCDText	= new CheckBox(i18n->TranslateString("Read CD Text"), Point(10, 11), Size(157, 0), &readCDText);
	check_readCDPlayerIni	= new CheckBox(i18n->TranslateString("Read cdplayer.ini"), Point(176, 11), Size(157, 0), &readCDPlayerIni);
	check_readISRC		= new CheckBox(i18n->TranslateString("Read ISRC when adding tracks to joblist"), Point(10, 37), Size(323, 0), &readISRC);

	group_cdinfo->Add(check_readCDText);
	group_cdinfo->Add(check_readCDPlayerIni);
	group_cdinfo->Add(check_readISRC);

	group_ripping		= new GroupBox(i18n->TranslateString("Ripper settings"), Point(7, 168), Size(344, 68));

	check_paranoia		= new CheckBox(i18n->TranslateString("Activate cdparanoia mode:"), Point(10, 14), Size(157, 0), &cdparanoia);
	check_paranoia->onAction.Connect(&ConfigureCDRip::ToggleParanoia, this);

	combo_paranoia_mode	= new ComboBox(Point(176, 13), Size(158, 0));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("Overlap only"));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("No verify"));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("No scratch repair"));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("Full cdparanoia mode"));
	combo_paranoia_mode->SelectNthEntry(config->GetIntValue("CDRip", "CDParanoiaMode", 3));

	ToggleParanoia();

	check_jitter		= new CheckBox(i18n->TranslateString("Activate jitter correction"), Point(10, 40), Size(157, 0), &jitter);
	check_swapchannels	= new CheckBox(i18n->TranslateString("Swap left/right channel"), Point(176, 40), Size(157, 0), &swapchannels);

	group_ripping->Add(check_paranoia);
	group_ripping->Add(combo_paranoia_mode);
	group_ripping->Add(check_jitter);
	group_ripping->Add(check_swapchannels);

	group_automatization	= new GroupBox(i18n->TranslateString("Automatization"), Point(359, 11), Size(178, 94));

	check_autoRead	= new CheckBox(i18n->TranslateString("Read CD contents on insert"), Point(10, 14), Size(157, 0), &autoRead);
	check_autoRead->onAction.Connect(&ConfigureCDRip::ToggleAutoRead, this);

	check_autoRip	= new CheckBox(i18n->TranslateString("Start ripping automatically"), check_autoRead->GetPosition() + Point(0, 26), Size(157, 0), &autoRip);
	check_autoEject	= new CheckBox(i18n->TranslateString("Eject disk after ripping"), check_autoRip->GetPosition() + Point(0, 26), Size(157, 0), &autoEject);

	group_automatization->Add(check_autoRead);
	group_automatization->Add(check_autoRip);
	group_automatization->Add(check_autoEject);

	group_cdoptions	= new GroupBox(i18n->TranslateString("CD options"), Point(359, 168), Size(178, 68));

	check_locktray	= new CheckBox(i18n->TranslateString("Lock CD tray while ripping"), Point(10, 14), Size(157, 0), &locktray);
	check_ntscsi	= new CheckBox(i18n->TranslateString("Use native NT SCSI library"), check_locktray->GetPosition() + Point(0, 26), Size(157, 0), &ntscsi);

	group_cdoptions->Add(check_locktray);
	group_cdoptions->Add(check_ntscsi);

#ifdef __WIN32__
	OSVERSIONINFOA	 vInfo;

	vInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOA);

	GetVersionExA(&vInfo);

	if (vInfo.dwPlatformId != VER_PLATFORM_WIN32_NT) check_ntscsi->Deactivate();
#else
	check_ntscsi->Deactivate();
#endif

	ToggleAutoRead();

	Add(group_drive);
	Add(group_ripping);
	Add(group_automatization);
	Add(group_cdoptions);
	Add(group_cdinfo);

	SetSize(Size(544, 243));
}

BoCA::ConfigureCDRip::~ConfigureCDRip()
{
	DeleteObject(group_drive);
	DeleteObject(combo_drive);
	DeleteObject(check_speed);
	DeleteObject(combo_speed);

	DeleteObject(group_ripping);
	DeleteObject(check_paranoia);
	DeleteObject(combo_paranoia_mode);
	DeleteObject(check_jitter);
	DeleteObject(check_swapchannels);

	DeleteObject(group_automatization);
	DeleteObject(check_autoRead);
	DeleteObject(check_autoRip);
	DeleteObject(check_autoEject);

	DeleteObject(group_cdoptions);
	DeleteObject(check_locktray);
	DeleteObject(check_ntscsi);

	DeleteObject(group_cdinfo);
	DeleteObject(check_readCDText);
	DeleteObject(check_readCDPlayerIni);
	DeleteObject(check_readISRC);
}

Void BoCA::ConfigureCDRip::ToggleSetSpeed()
{
	if (setspeed)	combo_speed->Activate();
	else		combo_speed->Deactivate();
}

Void BoCA::ConfigureCDRip::ToggleParanoia()
{
	if (cdparanoia)	combo_paranoia_mode->Activate();
	else		combo_paranoia_mode->Deactivate();
}

Void BoCA::ConfigureCDRip::ToggleAutoRead()
{
	if (autoRead)	check_autoRip->Activate();
	else		check_autoRip->Deactivate();
}

Int BoCA::ConfigureCDRip::SaveSettings()
{
	Config	*config = Config::Get();

	if (config->cdrip_numdrives >= 1) config->SetIntValue("Ripper", "ActiveDrive", combo_drive->GetSelectedEntryNumber());

	config->SetIntValue("Ripper", "RippingSpeed", setspeed ? 48 - (combo_speed->GetSelectedEntryNumber() * 4) : 0);
	config->SetIntValue("Ripper", "AutoReadContents", autoRead);
	config->SetIntValue("Ripper", "AutoRip", autoRip);

	config->SetIntValue("CDRip", "JitterCorrection", jitter);
	config->SetIntValue("CDRip", "SwapChannels", swapchannels);

	config->SetIntValue("CDRip", "UseNTSCSI", ntscsi);

	config->SetIntValue("CDRip", "LockTray", locktray);
	config->SetIntValue("CDRip", "EjectAfterRipping", autoEject);

	config->SetIntValue("CDRip", "ReadCDText", readCDText);
	config->SetIntValue("CDRip", "ReadCDPlayerIni", readCDPlayerIni);
	config->SetIntValue("CDRip", "ReadISRC", readISRC);

	config->SetIntValue("CDRip", "CDParanoia", cdparanoia);
	config->SetIntValue("CDRip", "CDParanoiaMode", combo_paranoia_mode->GetSelectedEntryNumber());

	return Success();
}
