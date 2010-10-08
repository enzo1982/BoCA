 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"

using namespace smooth::GUI::Dialogs;

BoCA::ConfigureYouTube::ConfigureYouTube()
{
	Config	*config = Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Extensions::Video Downloader::Configuration");

	autoDownload	= config->GetIntValue("YouTube", "AutoDownload", False);
	keepVideoFiles	= config->GetIntValue("YouTube", "SaveVideoFiles", False);

	String	 videoOutputDir = config->GetStringValue("YouTube", "VideoOutputDir", GetDefaultVideoOutputDirectory());

	group_auto		= new GroupBox(i18n->TranslateString("Automatization"), Point(7, 11), Size(344, 41));

	check_auto_download	= new CheckBox(i18n->TranslateString("Automatically download URLs copied to clipboard"), Point(10, 14), Size(324, 0), &autoDownload);

	group_auto->Add(check_auto_download);

	group_files		= new GroupBox(i18n->TranslateString("Video files"), Point(7, 64), Size(344, 69));

	check_keep		= new CheckBox(i18n->TranslateString("Save downloaded video files"), Point(10, 14), Size(236, 0), &keepVideoFiles);
	check_keep->onAction.Connect(&ConfigureYouTube::ToggleKeepFiles, this);

	edit_dir		= new EditBox(videoOutputDir, Point(10, 39), Size(236, 0), 0);

	button_browse		= new Button(i18n->TranslateString("Browse"), NIL, Point(254, 38), Size(0, 0));
	button_browse->onAction.Connect(&ConfigureYouTube::SelectOutputDir, this);

	group_files->Add(check_keep);
	group_files->Add(edit_dir);
	group_files->Add(button_browse);

	Add(group_auto);
	Add(group_files);

	ToggleKeepFiles();

	SetSize(Size(358, 140));
}

BoCA::ConfigureYouTube::~ConfigureYouTube()
{
	DeleteObject(group_auto);
	DeleteObject(check_auto_download);

	DeleteObject(group_files);
	DeleteObject(check_keep);

	DeleteObject(edit_dir);
	DeleteObject(button_browse);
}

Void BoCA::ConfigureYouTube::ToggleKeepFiles()
{
	if (keepVideoFiles)
	{
		edit_dir->Activate();
		button_browse->Activate();
	}
	else
	{
		edit_dir->Deactivate();
		button_browse->Deactivate();
	}
}

Void BoCA::ConfigureYouTube::SelectOutputDir()
{
	I18n		*i18n	= I18n::Get();

	i18n->SetContext("Extensions::Video Downloader::Configuration");

	DirSelection	*dialog = new DirSelection();

	dialog->SetParentWindow(GetContainerWindow());
	dialog->SetCaption(String("\n").Append(i18n->TranslateString("Select the folder to save downloaded video files to:")));
	dialog->SetDirName(edit_dir->GetText());

	if (dialog->ShowDialog() == Success())
	{
		edit_dir->SetText(dialog->GetDirName());
	}

	DeleteObject(dialog);
}

Int BoCA::ConfigureYouTube::SaveSettings()
{
	Config	*config = Config::Get();

	String	 videoOutputDir	= edit_dir->GetText();

	if (!videoOutputDir.EndsWith(Directory::GetDirectoryDelimiter())) videoOutputDir.Append(Directory::GetDirectoryDelimiter());

	config->SetStringValue("YouTube", "VideoOutputDir", videoOutputDir);

	config->SetIntValue("YouTube", "AutoDownload", autoDownload);
	config->SetIntValue("YouTube", "SaveVideoFiles", keepVideoFiles);

	return Success();
}

const String &BoCA::ConfigureYouTube::GetDefaultVideoOutputDirectory()
{
	static String	 defaultOutputDir;

	if (defaultOutputDir != NIL) return defaultOutputDir;

	defaultOutputDir = S::System::System::GetPersonalFilesDirectory();

	if (!defaultOutputDir.EndsWith(Directory::GetDirectoryDelimiter())) defaultOutputDir.Append(Directory::GetDirectoryDelimiter());

#ifdef __WIN32__
	defaultOutputDir.Append("My Videos").Append(Directory::GetDirectoryDelimiter());
#endif

	return defaultOutputDir;
}
