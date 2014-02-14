 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"

BoCA::ConfigureCDIO::ConfigureCDIO()
{
	Config	*config = Config::Get();

	autoRead	= config->GetIntValue("Ripper", "AutoReadContents", True);
	autoRip		= config->GetIntValue("Ripper", "AutoRip", False);

	jitter		= False;
	swapchannels	= False;

	readISRC	= config->GetIntValue("Ripper", "ReadISRC", False);

	cdparanoia	= config->GetIntValue("Ripper", "CDParanoia", False);

	I18n	*i18n = I18n::Get();

	i18n->SetContext("Ripper");

	group_drive	= new GroupBox(i18n->TranslateString("Active CD-ROM drive"), Point(7, 11), Size(344, 94));

	combo_drive	= new ComboBox(Point(10, 12), Size(324, 0));

	AS::Registry		&boca = AS::Registry::Get();
	AS::DeviceInfoComponent	*info = (AS::DeviceInfoComponent *) boca.CreateComponentByID("cdio-info");

	if (info != NIL)
	{
		for (Int i = 0; i < info->GetNumberOfDevices(); i++)
		{
			combo_drive->AddEntry(info->GetNthDeviceInfo(i).name);

			driveOffsetUsed.Add(config->GetIntValue("Ripper", String("UseOffsetDrive").Append(String::FromInt(i)), 0));
			driveOffsets.Add(config->GetIntValue("Ripper", String("ReadOffsetDrive").Append(String::FromInt(i)), 0));
			driveSpeeds.Add(config->GetIntValue("Ripper", String("RippingSpeedDrive").Append(String::FromInt(i)), 0));
		}

		boca.DeleteComponent(info);
	}

	combo_drive->SelectNthEntry(config->GetIntValue("Ripper", "ActiveDrive", 0));
	combo_drive->onSelectEntry.Connect(&ConfigureCDIO::SelectDrive, this);

	useoffset = driveOffsetUsed.GetNth(config->GetIntValue("Ripper", "ActiveDrive", 0));
	setspeed  = driveSpeeds.GetNth(config->GetIntValue("Ripper", "ActiveDrive", 0));

	check_speed		= new CheckBox(i18n->TranslateString("Set drive speed limit:"), Point(10, 40), Size(157, 0), &setspeed);
	check_speed->onAction.Connect(&ConfigureCDIO::ToggleSetSpeed, this);

	combo_speed		= new ComboBox(Point(176, 39), Size(158, 0));
	combo_speed->onSelectEntry.Connect(&ConfigureCDIO::SelectSpeed, this);

	for (Int i = 48; i > 0; i -= 4) combo_speed->AddEntry(String::FromInt(i).Append("x"));

	check_offset		= new CheckBox(i18n->TranslateString("Use read offset:"), Point(10, 67), Size(157, 0), &useoffset);
	check_offset->onAction.Connect(&ConfigureCDIO::ToggleUseOffset, this);

	edit_offset		= new EditBox(NIL, Point(176, 66), Size(36, 0), 5);
	edit_offset->SetFlags(EDB_NUMERIC);
	edit_offset->onInput.Connect(&ConfigureCDIO::ChangeOffset, this);

	text_offset_samples	= new Text(i18n->TranslateString("samples"), Point(220, 69));

	SelectDrive();

	group_drive->Add(combo_drive);
	group_drive->Add(check_speed);
	group_drive->Add(combo_speed);
	group_drive->Add(check_offset);
	group_drive->Add(edit_offset);
	group_drive->Add(text_offset_samples);

	group_cdinfo		= new GroupBox(i18n->TranslateString("CD information"), Point(7, 117), Size(344, 39));

	check_readISRC		= new CheckBox(i18n->TranslateString("Read ISRC when adding tracks to joblist"), Point(10, 11), Size(323, 0), &readISRC);

	group_cdinfo->Add(check_readISRC);

	group_ripping		= new GroupBox(i18n->TranslateString("Ripper settings"), Point(7, 117/*168*/), Size(344, 68));

	check_paranoia		= new CheckBox(i18n->TranslateString("Activate cdparanoia mode:"), Point(10, 14), Size(157, 0), &cdparanoia);
	check_paranoia->onAction.Connect(&ConfigureCDIO::ToggleParanoia, this);

	combo_paranoia_mode	= new ComboBox(Point(176, 13), Size(158, 0));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("Overlap only"));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("No verify"));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("No scratch repair"));
	combo_paranoia_mode->AddEntry(i18n->TranslateString("Full cdparanoia mode"));
	combo_paranoia_mode->SelectNthEntry(config->GetIntValue("Ripper", "CDParanoiaMode", 3));

	ToggleParanoia();

	check_jitter		= new CheckBox(i18n->TranslateString("Activate jitter correction"), Point(10, 40), Size(157, 0), &jitter);
	check_jitter->Deactivate();

	check_swapchannels	= new CheckBox(i18n->TranslateString("Swap left/right channel"), Point(176, 40), Size(157, 0), &swapchannels);
	check_swapchannels->Deactivate();

	group_ripping->Add(check_paranoia);
	group_ripping->Add(combo_paranoia_mode);
	group_ripping->Add(check_jitter);
	group_ripping->Add(check_swapchannels);

	group_automatization	= new GroupBox(i18n->TranslateString("Automatization"), Point(359, 11), Size(178, 68));

	check_autoRead	= new CheckBox(i18n->TranslateString("Read CD contents on insert"), Point(10, 14), Size(157, 0), &autoRead);
	check_autoRead->onAction.Connect(&ConfigureCDIO::ToggleAutoRead, this);

	check_autoRip	= new CheckBox(i18n->TranslateString("Start ripping automatically"), check_autoRead->GetPosition() + Point(0, 26), Size(157, 0), &autoRip);

	group_automatization->Add(check_autoRead);
	group_automatization->Add(check_autoRip);

	ToggleAutoRead();

	Add(group_drive);
	Add(group_ripping);
	Add(group_automatization);
//	Add(group_cdinfo);

	SetSize(Size(544, 192/*243*/));
}

BoCA::ConfigureCDIO::~ConfigureCDIO()
{
	DeleteObject(group_drive);
	DeleteObject(combo_drive);
	DeleteObject(check_speed);
	DeleteObject(combo_speed);
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

	DeleteObject(group_cdinfo);
	DeleteObject(check_readISRC);
}

Void BoCA::ConfigureCDIO::SelectDrive()
{
	edit_offset->SetText(String::FromInt(driveOffsets.GetNth(combo_drive->GetSelectedEntryNumber())));
	combo_speed->SelectNthEntry((48 - driveSpeeds.GetNth(combo_drive->GetSelectedEntryNumber())) / 4);

	check_offset->SetChecked(driveOffsetUsed.GetNth(combo_drive->GetSelectedEntryNumber()));
	check_speed->SetChecked(driveSpeeds.GetNth(combo_drive->GetSelectedEntryNumber()));

	ToggleUseOffset();
	ToggleSetSpeed();
}

Void BoCA::ConfigureCDIO::ToggleUseOffset()
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

Void BoCA::ConfigureCDIO::ChangeOffset()
{
	if (!useoffset) return;

	driveOffsets.SetNth(combo_drive->GetSelectedEntryNumber(), edit_offset->GetText().ToInt());
}

Void BoCA::ConfigureCDIO::ToggleSetSpeed()
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

Void BoCA::ConfigureCDIO::SelectSpeed()
{
	if (!setspeed) return;

	driveSpeeds.SetNth(combo_drive->GetSelectedEntryNumber(), 48 - (combo_speed->GetSelectedEntryNumber() * 4));
}

Void BoCA::ConfigureCDIO::ToggleParanoia()
{
	if (cdparanoia)	combo_paranoia_mode->Activate();
	else		combo_paranoia_mode->Deactivate();
}

Void BoCA::ConfigureCDIO::ToggleAutoRead()
{
	if (autoRead)	check_autoRip->Activate();
	else		check_autoRip->Deactivate();
}

Int BoCA::ConfigureCDIO::SaveSettings()
{
	Config	*config = Config::Get();

	if (driveSpeeds.Length() >= 1) config->SetIntValue("Ripper", "ActiveDrive", combo_drive->GetSelectedEntryNumber());

	for (Int i = 0; i < driveSpeeds.Length(); i++)
	{
		config->SetIntValue("Ripper", String("UseOffsetDrive").Append(String::FromInt(i)), driveOffsetUsed.GetNth(i));
		config->SetIntValue("Ripper", String("ReadOffsetDrive").Append(String::FromInt(i)), driveOffsets.GetNth(i));
		config->SetIntValue("Ripper", String("RippingSpeedDrive").Append(String::FromInt(i)), driveSpeeds.GetNth(i));
	}

	config->SetIntValue("Ripper", "AutoReadContents", autoRead);
	config->SetIntValue("Ripper", "AutoRip", autoRip);

	config->SetIntValue("Ripper", "ReadISRC", readISRC);

	config->SetIntValue("Ripper", "CDParanoia", cdparanoia);
	config->SetIntValue("Ripper", "CDParanoiaMode", combo_paranoia_mode->GetSelectedEntryNumber());

	return Success();
}
