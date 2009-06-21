 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/config.h>

BoCA::Config *BoCA::Config::instance = NIL;

BoCA::Config::Config()
{
	languageChanged		= False;

	cdrip_autoRead_active	= False;
	cdrip_timeout		= 0;

	enable_id3		= False;
	enable_mp4		= False;
	enable_ogg		= False;
	enable_wma		= False;

	saveSettingsOnExit	= True;

	String	 programsDir = S::System::System::GetProgramFilesDirectory();

	if (Application::GetApplicationDirectory().ToUpper().StartsWith(programsDir.ToUpper()))
	{
		configDir = S::System::System::GetApplicationDataDirectory();

		if (configDir != "") configDir.Append("BonkEnc").Append(Directory::GetDirectoryDelimiter());

		Directory(configDir).Create();
	}
	else
	{
		configDir = Application::GetApplicationDirectory();
	}

	config = new Configuration(String(configDir).Append("boca").Append(Directory::GetDirectoryDelimiter()).Append("boca.xml"), True);

	LoadSettings();
}

BoCA::Config::~Config()
{
	if (saveSettingsOnExit) SaveSettings();

	delete config;
}

BoCA::Config *BoCA::Config::Get()
{
	if (instance == NIL)
	{
		instance = new Config();
	}

	return instance;
}

Void BoCA::Config::Free()
{
	if (instance != NIL)
	{
		delete instance;

		instance = NIL;
	}
}

Void BoCA::Config::SetSaveSettingsOnExit(Bool nSaveSettingsOnExit)
{
	saveSettingsOnExit = nSaveSettingsOnExit;
}

Int BoCA::Config::SetIntValue(const String &section, const String &name, Int value)
{
	return config->SetIntValue(section, name, value);
}

Int BoCA::Config::SetStringValue(const String &section, const String &name, const String &value)
{
	return config->SetStringValue(section, name, value);
}

Int BoCA::Config::GetIntValue(const String &section, const String &name, Int defaultValue)
{
	return config->GetIntValue(section, name, defaultValue);
}

String BoCA::Config::GetStringValue(const String &section, const String &name, const String &defaultValue)
{
	return config->GetStringValue(section, name, defaultValue);
}

Bool BoCA::Config::LoadSettings()
{
	String	 personalDir = S::System::System::GetPersonalFilesDirectory();

	firstStart				= config->GetIntValue("Settings", "FirstStart", 1);
	language				= config->GetStringValue("Settings", "Language", "");
	enc_filePattern				= config->GetStringValue("Settings", "EncoderFilenamePattern", "<artist> - <title>");
	enc_onTheFly				= config->GetIntValue("Settings", "EncodeOnTheFly", 1);
	enc_keepWaves				= config->GetIntValue("Settings", "KeepWaveFiles", 0);
	playlist_useEncOutdir			= config->GetIntValue("Settings", "PlaylistUseEncOutDir", 1);
	playlist_filePattern			= config->GetStringValue("Settings", "PlaylistFilenamePattern", "<artist> - <album>");
	useUnicodeNames				= config->GetIntValue("Settings", "UseUnicodeFilenames", 1);
	lastAddedDir				= config->GetStringValue("Settings", "LastAddedDir", "");
	lastAddedPattern			= config->GetStringValue("Settings", "LastAddedPattern", "");
	writeToInputDir				= config->GetIntValue("Settings", "WriteToInputDirectory", 0);
	allowOverwrite				= config->GetIntValue("Settings", "AllowOverwriteSource", 0);

	cdrip_activedrive			= config->GetIntValue("CDRip", "ActiveCDROM", 0);
	cdrip_debuglevel			= config->GetIntValue("CDRip", "DebugCDRip", 0);
	cdrip_paranoia				= config->GetIntValue("CDRip", "CDParanoia", 0);
	cdrip_paranoia_mode			= config->GetIntValue("CDRip", "CDParanoiaMode", 3);
	cdrip_detectJitterErrors		= config->GetIntValue("CDRip", "DetectJitterErrors", 1);
	cdrip_detectC2Errors			= config->GetIntValue("CDRip", "DetectC2Errors", 1);
	cdrip_jitter				= config->GetIntValue("CDRip", "JitterCorrection", 0);
	cdrip_swapchannels			= config->GetIntValue("CDRip", "SwapChannels", 0);
	cdrip_numdrives				= 0;

	return True;
}

Bool BoCA::Config::SaveSettings()
{
	config->SetIntValue("Settings", "FirstStart", 0);
	config->SetStringValue("Settings", "Language", language);
	config->SetStringValue("Settings", "EncoderFilenamePattern", enc_filePattern);
	config->SetIntValue("Settings", "EncodeOnTheFly", enc_onTheFly);
	config->SetIntValue("Settings", "KeepWaveFiles", enc_keepWaves);
	config->SetIntValue("Settings", "PlaylistUseEncOutDir", playlist_useEncOutdir);
	config->SetStringValue("Settings", "PlaylistFilenamePattern", playlist_filePattern);
	config->SetIntValue("Settings", "UseUnicodeFilenames", useUnicodeNames);
	config->SetStringValue("Settings", "LastAddedDir", lastAddedDir);
	config->SetStringValue("Settings", "LastAddedPattern", lastAddedPattern);
	config->SetIntValue("Settings", "WriteToInputDirectory", writeToInputDir);
	config->SetIntValue("Settings", "AllowOverwriteSource", allowOverwrite);

	config->SetIntValue("CDRip", "ActiveCDROM", cdrip_activedrive);
	config->SetIntValue("CDRip", "DebugCDRip", cdrip_debuglevel);
	config->SetIntValue("CDRip", "CDParanoia", cdrip_paranoia);
	config->SetIntValue("CDRip", "CDParanoiaMode", cdrip_paranoia_mode);
	config->SetIntValue("CDRip", "DetectJitterErrors", cdrip_detectJitterErrors);
	config->SetIntValue("CDRip", "DetectC2Errors", cdrip_detectC2Errors);
	config->SetIntValue("CDRip", "JitterCorrection", cdrip_jitter);
	config->SetIntValue("CDRip", "SwapChannels", cdrip_swapchannels);

	config->Save();

	return True;
}
