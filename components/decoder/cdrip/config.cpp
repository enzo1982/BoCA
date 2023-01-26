 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2023 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <windows.h>

#include "config.h"

const String	 BoCA::ConfigureCDRip::ConfigID = "Ripper";

BoCA::ConfigureCDRip::ConfigureCDRip()
{
	const Config	*config = Config::Get();

	Bool	 notificationAvailable = config->GetIntValue("Settings", "NotificationAvailable", False);

	autoRead	= notificationAvailable && config->GetIntValue(ConfigID, "AutoReadContents", True);
	autoRip		= notificationAvailable && config->GetIntValue(ConfigID, "AutoRip", False);

	jitter		= config->GetIntValue(ConfigID, "JitterCorrection", False);
	swapchannels	= config->GetIntValue(ConfigID, "SwapChannels", False);

	autoEject	= config->GetIntValue(ConfigID, "EjectAfterRipping", False);

	readCDText	= config->GetIntValue(ConfigID, "ReadCDText", True);
	readCDPlayerIni	= config->GetIntValue(ConfigID, "ReadCDPlayerIni", True);
	readISRC	= config->GetIntValue(ConfigID, "ReadISRC", False);

	cdparanoia	= config->GetIntValue(ConfigID, "CDParanoia", False);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Ripper");

	group_drive	= new GroupBox(i18n->TranslateString("Active CD-ROM drive"), Point(7, 11), Size(354, 121));

	combo_drive	= new ComboBox(Point(10, 12), Size(334, 0));

	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*info = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdrip-info");

	if (info != NIL)
	{
		if (info->GetNumberOfDevices() == 0) group_drive->Deactivate();

		for (Int i = 0; i < info->GetNumberOfDevices(); i++)
		{
			const Device	&device	  = info->GetNthDeviceInfo(i);
			String		 deviceID = device.GetID();

			combo_drive->AddEntry(device.GetName());

			driveOffsetUsed.Add(config->GetIntValue(ConfigID, String("UseOffsetDrive-").Append(deviceID), 0));
			driveOffsets.Add(config->GetIntValue(ConfigID, String("ReadOffsetDrive-").Append(deviceID), 0));
			driveSpeeds.Add(config->GetIntValue(ConfigID, String("RippingSpeedDrive-").Append(deviceID), 0));
			driveSpinUpTimes.Add(config->GetIntValue(ConfigID, String("SpinUpTimeDrive-").Append(deviceID), -5));
		}

		boca.DeleteComponent(info);
	}

	combo_drive->SelectNthEntry(config->GetIntValue(ConfigID, "ActiveDrive", 0));
	combo_drive->onSelectEntry.Connect(&ConfigureCDRip::SelectDrive, this);

	useoffset = driveOffsetUsed.GetNth(config->GetIntValue(ConfigID, "ActiveDrive", 0));
	spinup	  = driveSpinUpTimes.GetNth(config->GetIntValue(ConfigID, "ActiveDrive", 0)) > 0;
	setspeed  = driveSpeeds.GetNth(config->GetIntValue(ConfigID, "ActiveDrive", 0));

	check_speed		= new CheckBox(i18n->AddColon(i18n->TranslateString("Set drive speed limit")), Point(10, 40), Size(162, 0), &setspeed);
	check_speed->onAction.Connect(&ConfigureCDRip::ToggleSetSpeed, this);

	check_spinup		= new CheckBox(i18n->AddColon(i18n->TranslateString("Spin up before ripping")), Point(10, 67), Size(162, 0), &spinup);
	check_spinup->onAction.Connect(&ConfigureCDRip::ToggleSpinUp, this);

	check_offset		= new CheckBox(i18n->AddColon(i18n->TranslateString("Use read offset")), Point(10, 94), Size(162, 0), &useoffset);
	check_offset->onAction.Connect(&ConfigureCDRip::ToggleUseOffset, this);

	Int	 maxTextSize = Math::Max(Math::Max(check_speed->GetUnscaledTextWidth(), check_spinup->GetUnscaledTextWidth()), check_offset->GetUnscaledTextWidth());

	check_speed->SetWidth(maxTextSize + 21);
	check_spinup->SetWidth(maxTextSize + 21);
	check_offset->SetWidth(maxTextSize + 21);

	combo_speed		= new ComboBox(Point(39 + maxTextSize, 39), Size(305 - maxTextSize, 0));
	combo_speed->onSelectEntry.Connect(&ConfigureCDRip::SelectSpeed, this);

	for (Int i = 48; i > 0; i -= 4) combo_speed->AddEntry(String::FromInt(i).Append("x"));

	text_spinup_seconds	= new Text(i18n->TranslateString("%1 seconds").Replace("%1", "00"), Point(275, 69));
	text_spinup_seconds->SetX(343 - text_spinup_seconds->GetUnscaledTextWidth());

	slider_spinup		= new Slider(Point(39 + maxTextSize, 67), Size(86, 0), OR_HORZ, NIL, 1, 30);
	slider_spinup->SetWidth(335 - slider_spinup->GetX() - text_spinup_seconds->GetUnscaledTextWidth());
	slider_spinup->onValueChange.Connect(&ConfigureCDRip::ChangeSpinUpTime, this);

	edit_offset		= new EditBox(Point(39 + maxTextSize, 93), Size(36, 0), 5);
	edit_offset->SetFlags(EDB_NUMERIC);
	edit_offset->onInput.Connect(&ConfigureCDRip::ChangeOffset, this);

	text_offset_samples	= new Text(i18n->TranslateString("samples"), Point(edit_offset->GetX() + edit_offset->GetWidth() + 8, 96));

	SelectDrive();

	group_drive->Add(combo_drive);
	group_drive->Add(check_speed);
	group_drive->Add(combo_speed);
	group_drive->Add(check_spinup);
	group_drive->Add(slider_spinup);
	group_drive->Add(text_spinup_seconds);
	group_drive->Add(check_offset);
	group_drive->Add(edit_offset);
	group_drive->Add(text_offset_samples);

	group_cdinfo		= new GroupBox(i18n->TranslateString("CD information"), Point(7, 144), Size(354, 65));

	check_readCDText	= new CheckBox(i18n->TranslateString("Read CD-Text"), Point(10, 11), Size(162, 0), &readCDText);
	check_readCDPlayerIni	= new CheckBox(i18n->TranslateString("Read cdplayer.ini"), Point(181, 11), Size(162, 0), &readCDPlayerIni);
	check_readISRC		= new CheckBox(i18n->TranslateString("Read ISRC when adding tracks to joblist"), Point(10, 37), Size(333, 0), &readISRC);

	check_readCDText->SetWidth(Math::Max(162, check_readCDText->GetUnscaledTextWidth() + 21));
	check_readCDPlayerIni->SetX(check_readCDText->GetWidth() + 19);
	check_readCDPlayerIni->SetWidth(Math::Max(162, check_readCDPlayerIni->GetUnscaledTextWidth() + 21));
	check_readISRC->SetWidth(Math::Max(333, check_readISRC->GetUnscaledTextWidth() + 21));

	group_cdinfo->SetWidth(Math::Max(check_readCDText->GetWidth() + check_readCDPlayerIni->GetWidth() + 9, check_readISRC->GetWidth()) + 20);

	group_cdinfo->Add(check_readCDText);
	group_cdinfo->Add(check_readCDPlayerIni);
	group_cdinfo->Add(check_readISRC);

	group_ripping		= new GroupBox(i18n->TranslateString("Ripper settings"), Point(7, 221), Size(354, 68));

	check_paranoia		= new CheckBox(i18n->AddColon(i18n->TranslateString("Activate cdparanoia mode")), Point(10, 14), Size(162, 0), &cdparanoia);
	check_paranoia->onAction.Connect(&ConfigureCDRip::ToggleParanoia, this);

	combo_paranoia_mode	= new ComboBox(Point(181, 13), Size(162, 0));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("Overlap only"));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("No verify"));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("No scratch repair"));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("Full cdparanoia mode"));
	combo_paranoia_mode->SelectNthEntry(config->GetIntValue(ConfigID, "CDParanoiaMode", 3));

	ToggleParanoia();

	check_jitter		= new CheckBox(i18n->TranslateString("Activate jitter correction"), Point(10, 40), Size(162, 0), &jitter);
	check_swapchannels	= new CheckBox(i18n->TranslateString("Switch stereo channels"), Point(181, 40), Size(162, 0), &swapchannels);

	maxTextSize = Math::Max(check_paranoia->GetUnscaledTextWidth(), check_jitter->GetUnscaledTextWidth());

	check_paranoia->SetWidth(Math::Max(162, maxTextSize + 21));
	check_jitter->SetWidth(check_paranoia->GetWidth());
	combo_paranoia_mode->SetX(check_paranoia->GetWidth() + 19);
	check_swapchannels->SetX(combo_paranoia_mode->GetX());
	check_swapchannels->SetWidth(Math::Max(162, check_swapchannels->GetUnscaledTextWidth() + 21));

	group_ripping->SetWidth(check_paranoia->GetWidth() + check_swapchannels->GetWidth() + 29);

	group_ripping->Add(check_paranoia);
	group_ripping->Add(combo_paranoia_mode);
	group_ripping->Add(check_jitter);
	group_ripping->Add(check_swapchannels);

	group_drive->SetWidth(Math::Max(group_cdinfo->GetWidth(), group_ripping->GetWidth()));
	group_cdinfo->SetWidth(group_drive->GetWidth());
	group_ripping->SetWidth(group_drive->GetWidth());

	combo_drive->SetWidth(group_drive->GetWidth() - 20);
	combo_speed->SetWidth(group_drive->GetWidth() - check_speed->GetWidth() - 28);
	slider_spinup->SetWidth(group_drive->GetWidth() - 24 - slider_spinup->GetX() - text_spinup_seconds->GetUnscaledTextWidth());
	text_spinup_seconds->SetX(group_drive->GetWidth() - 16 - text_spinup_seconds->GetUnscaledTextWidth());

	check_readCDPlayerIni->SetWidth(group_cdinfo->GetWidth() - check_readCDText->GetWidth() - 29);
	check_readISRC->SetWidth(group_cdinfo->GetWidth() - 20);

	check_swapchannels->SetWidth(group_ripping->GetWidth() - check_paranoia->GetWidth() - 29);
	combo_paranoia_mode->SetWidth(check_swapchannels->GetWidth());

	group_automatization	= new GroupBox(i18n->TranslateString("Automatization"), Point(group_drive->GetWidth() + 15, 11), Size(190, 94));

	check_autoRead	= new CheckBox(i18n->TranslateString("Read CD contents on insert"), Point(10, 14), Size(170, 0), &autoRead);
	check_autoRead->onAction.Connect(&ConfigureCDRip::ToggleAutoRead, this);

	if (!notificationAvailable) check_autoRead->Deactivate();

	check_autoRip	= new CheckBox(i18n->TranslateString("Start ripping automatically"), check_autoRead->GetPosition() + Point(0, 26), Size(170, 0), &autoRip);
	check_autoEject	= new CheckBox(i18n->TranslateString("Eject disk after ripping"), check_autoRip->GetPosition() + Point(0, 26), Size(170, 0), &autoEject);

	group_automatization->Add(check_autoRead);
	group_automatization->Add(check_autoRip);
	group_automatization->Add(check_autoEject);

	maxTextSize = Math::Max(Math::Max(check_autoRead->GetUnscaledTextWidth(), check_autoRip->GetUnscaledTextWidth()), check_autoEject->GetUnscaledTextWidth());

	check_autoRead->SetWidth(Math::Max(149, maxTextSize + 21));
	check_autoRip->SetWidth(check_autoRead->GetWidth());
	check_autoEject->SetWidth(check_autoRead->GetWidth());

	group_automatization->SetWidth(check_autoRead->GetWidth() + 20);

	ToggleAutoRead();

	Add(group_drive);
	Add(group_ripping);
	Add(group_automatization);
	Add(group_cdinfo);

	SetSize(Size(group_drive->GetWidth() + group_automatization->GetWidth() + 22, 296));
}

BoCA::ConfigureCDRip::~ConfigureCDRip()
{
	DeleteObject(group_drive);
	DeleteObject(combo_drive);
	DeleteObject(check_speed);
	DeleteObject(combo_speed);
	DeleteObject(check_spinup);
	DeleteObject(slider_spinup);
	DeleteObject(text_spinup_seconds);
	DeleteObject(check_offset);
	DeleteObject(edit_offset);
	DeleteObject(text_offset_samples);

	DeleteObject(group_ripping);
	DeleteObject(check_paranoia);
	DeleteObject(combo_paranoia_mode);
	DeleteObject(check_jitter);
	DeleteObject(check_swapchannels);

	DeleteObject(group_automatization);
	DeleteObject(check_autoRead);
	DeleteObject(check_autoRip);
	DeleteObject(check_autoEject);

	DeleteObject(group_cdinfo);
	DeleteObject(check_readCDText);
	DeleteObject(check_readCDPlayerIni);
	DeleteObject(check_readISRC);
}

Void BoCA::ConfigureCDRip::SelectDrive()
{
	edit_offset->SetText(String::FromInt(driveOffsets.GetNth(combo_drive->GetSelectedEntryNumber())));
	combo_speed->SelectNthEntry((48 - driveSpeeds.GetNth(combo_drive->GetSelectedEntryNumber())) / 4);

	check_offset->SetChecked(driveOffsetUsed.GetNth(combo_drive->GetSelectedEntryNumber()));
	check_speed->SetChecked(driveSpeeds.GetNth(combo_drive->GetSelectedEntryNumber()));
	check_spinup->SetChecked(driveSpinUpTimes.GetNth(combo_drive->GetSelectedEntryNumber()) > 0);

	slider_spinup->SetValue(Math::Abs(driveSpinUpTimes.GetNth(combo_drive->GetSelectedEntryNumber())));

	ToggleUseOffset();
	ToggleSetSpeed();
	ToggleSpinUp();
}

Void BoCA::ConfigureCDRip::ToggleUseOffset()
{
	driveOffsetUsed.SetNth(combo_drive->GetSelectedEntryNumber(), useoffset);

	if (useoffset)
	{
		edit_offset->Activate();
		text_offset_samples->Activate();
	}
	else
	{
		edit_offset->Deactivate();
		text_offset_samples->Deactivate();
	}
}

Void BoCA::ConfigureCDRip::ChangeOffset()
{
	if (!useoffset) return;

	driveOffsets.SetNth(combo_drive->GetSelectedEntryNumber(), edit_offset->GetText().ToInt());
}

Void BoCA::ConfigureCDRip::ToggleSpinUp()
{
	driveSpinUpTimes.SetNth(combo_drive->GetSelectedEntryNumber(), slider_spinup->GetValue() * (spinup ? 1 : -1));

	if (spinup)
	{
		slider_spinup->Activate();
		text_spinup_seconds->Activate();
	}
	else
	{
		slider_spinup->Deactivate();
		text_spinup_seconds->Deactivate();
	}
}

Void BoCA::ConfigureCDRip::ChangeSpinUpTime()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Ripper");

	text_spinup_seconds->SetText(i18n->TranslateString("%1 seconds").Replace("%1", String::FromInt(slider_spinup->GetValue())));

	driveSpinUpTimes.SetNth(combo_drive->GetSelectedEntryNumber(), slider_spinup->GetValue() * (spinup ? 1 : -1));
}

Void BoCA::ConfigureCDRip::ToggleSetSpeed()
{
	if (setspeed)
	{
		combo_speed->Activate();

		driveSpeeds.SetNth(combo_drive->GetSelectedEntryNumber(), 48 - (combo_speed->GetSelectedEntryNumber() * 4));
	}
	else
	{
		combo_speed->Deactivate();

		driveSpeeds.SetNth(combo_drive->GetSelectedEntryNumber(), 0);
	}
}

Void BoCA::ConfigureCDRip::SelectSpeed()
{
	if (!setspeed) return;

	driveSpeeds.SetNth(combo_drive->GetSelectedEntryNumber(), 48 - (combo_speed->GetSelectedEntryNumber() * 4));
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

	if (driveSpeeds.Length() >= 1) config->SetIntValue(ConfigID, "ActiveDrive", combo_drive->GetSelectedEntryNumber());

	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*info = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdrip-info");

	if (info != NIL)
	{
		for (Int i = 0; i < info->GetNumberOfDevices(); i++)
		{
			const Device	&device	  = info->GetNthDeviceInfo(i);
			String		 deviceID = device.GetID();

			config->SetIntValue(ConfigID, String("UseOffsetDrive-").Append(deviceID), driveOffsetUsed.GetNth(i));
			config->SetIntValue(ConfigID, String("ReadOffsetDrive-").Append(deviceID), driveOffsets.GetNth(i));
			config->SetIntValue(ConfigID, String("RippingSpeedDrive-").Append(deviceID), driveSpeeds.GetNth(i));
			config->SetIntValue(ConfigID, String("SpinUpTimeDrive-").Append(deviceID), driveSpinUpTimes.GetNth(i));
		}

		boca.DeleteComponent(info);
	}

	if (config->GetIntValue("Settings", "NotificationAvailable", False))
	{
		config->SetIntValue(ConfigID, "AutoReadContents", autoRead);
		config->SetIntValue(ConfigID, "AutoRip", autoRip);
	}

	config->SetIntValue(ConfigID, "JitterCorrection", jitter);
	config->SetIntValue(ConfigID, "SwapChannels", swapchannels);

	config->SetIntValue(ConfigID, "EjectAfterRipping", autoEject);

	config->SetIntValue(ConfigID, "ReadCDText", readCDText);
	config->SetIntValue(ConfigID, "ReadCDPlayerIni", readCDPlayerIni);
	config->SetIntValue(ConfigID, "ReadISRC", readISRC);

	config->SetIntValue(ConfigID, "CDParanoia", cdparanoia);
	config->SetIntValue(ConfigID, "CDParanoiaMode", combo_paranoia_mode->GetSelectedEntryNumber());

	return Success();
}
