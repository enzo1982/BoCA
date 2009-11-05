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

#ifdef __WIN32__
	if (!personalDir.EndsWith(Directory::GetDirectoryDelimiter())) personalDir.Append(Directory::GetDirectoryDelimiter());

	personalDir.Append("My Music");
#endif

	firstStart			= config->GetIntValue("Settings", "FirstStart", 1);
	language			= config->GetStringValue("Settings", "Language", "");
	wndPos.x			= config->GetIntValue("Settings", "WindowPosX", 100);
	wndPos.y			= config->GetIntValue("Settings", "WindowPosY", 100);
	wndSize.cx			= config->GetIntValue("Settings", "WindowSizeX", 800);
	wndSize.cy			= config->GetIntValue("Settings", "WindowSizeY", 600);
	maximized			= config->GetIntValue("Settings", "WindowMaximized", 0);
	showTitleInfo			= config->GetIntValue("Settings", "ShowTitleInfo", 1);
	showTooltips			= config->GetIntValue("Settings", "ShowTooltips", 1);
	encoderID			= config->GetStringValue("Settings", "Encoder", "wave-out");
	enc_outdir			= config->GetStringValue("Settings", "EncoderOutDir", personalDir);
	enc_filePattern			= config->GetStringValue("Settings", "EncoderFilenamePattern", "<artist> - <album>\\<artist> - <album> - <track> - <title>");
	enc_onTheFly			= config->GetIntValue("Settings", "EncodeOnTheFly", 1);
	enc_keepWaves			= config->GetIntValue("Settings", "KeepWaveFiles", 0);
	useUnicodeNames			= config->GetIntValue("Settings", "UseUnicodeFilenames", 1);
	writeToInputDir			= config->GetIntValue("Settings", "WriteToInputDirectory", 0);
	allowOverwrite			= config->GetIntValue("Settings", "AllowOverwriteSource", 0);
	encodeToSingleFile		= config->GetIntValue("Settings", "EncodeToSingleFile", 0);
	checkUpdatesAtStartup		= config->GetIntValue("Settings", "CheckUpdatesAtStartup", 1);

	cdrip_activedrive		= config->GetIntValue("CDRip", "ActiveCDROM", 0);
	cdrip_debuglevel		= config->GetIntValue("CDRip", "DebugCDRip", 0);
	cdrip_paranoia			= config->GetIntValue("CDRip", "CDParanoia", 0);
	cdrip_paranoia_mode		= config->GetIntValue("CDRip", "CDParanoiaMode", 3);
	cdrip_detectJitterErrors	= config->GetIntValue("CDRip", "DetectJitterErrors", 1);
	cdrip_detectC2Errors		= config->GetIntValue("CDRip", "DetectC2Errors", 1);
	cdrip_jitter			= config->GetIntValue("CDRip", "JitterCorrection", 0);
	cdrip_swapchannels		= config->GetIntValue("CDRip", "SwapChannels", 0);
	cdrip_numdrives			= 0;

	enable_auto_cddb		= config->GetIntValue("freedb", "AutoCDDBQueries", 1);
	enable_overwrite_cdtext		= config->GetIntValue("freedb", "OverwriteCDText", 1);
	enable_cddb_cache		= config->GetIntValue("freedb", "EnableCDDBCache", 1);
	enable_local_cddb		= config->GetIntValue("freedb", "EnableLocalCDDB", 0);
	freedb_dir			= config->GetStringValue("freedb", "Directory", "freedb\\");
	enable_remote_cddb		= config->GetIntValue("freedb", "EnableRemoteCDDB", 1);
	freedb_server			= config->GetStringValue("freedb", "Server", "freedb.freedb.org");
	freedb_mode			= config->GetIntValue("freedb", "Mode", 0);
	freedb_cddbp_port		= config->GetIntValue("freedb", "CDDBPPort", 8880);
	freedb_http_port		= 80;
	freedb_query_path		= config->GetStringValue("freedb", "QueryPath", "/~cddb/cddb.cgi");
	freedb_submit_path		= config->GetStringValue("freedb", "SubmitPath", "/~cddb/submit.cgi");
	freedb_email			= config->GetStringValue("freedb", "eMail", "cddb@bonkenc.org");
	freedb_proxy_mode		= config->GetIntValue("freedb", "ProxyMode", 0);
	freedb_proxy			= config->GetStringValue("freedb", "Proxy", "localhost");
	freedb_proxy_port		= config->GetIntValue("freedb", "ProxyPort", 1080);
	freedb_proxy_user		= config->GetStringValue("freedb", "ProxyUserName", NIL);
	freedb_proxy_password		= config->GetStringValue("freedb", "ProxyPassword", NIL);
	update_joblist			= config->GetIntValue("freedb", "UpdateJoblistOnSubmit", 1);

	if (!enc_outdir.EndsWith(Directory::GetDirectoryDelimiter())) enc_outdir.Append(Directory::GetDirectoryDelimiter());
	if (!freedb_dir.EndsWith(Directory::GetDirectoryDelimiter())) freedb_dir.Append(Directory::GetDirectoryDelimiter());

	if (encodeToSingleFile && !enc_onTheFly) enc_onTheFly = True;

	return True;
}

Bool BoCA::Config::SaveSettings()
{
	config->SetIntValue("Settings", "FirstStart", 0);
	config->SetStringValue("Settings", "Language", language);
	config->SetIntValue("Settings", "WindowPosX", wndPos.x);
	config->SetIntValue("Settings", "WindowPosY", wndPos.y);
	config->SetIntValue("Settings", "WindowSizeX", wndSize.cx);
	config->SetIntValue("Settings", "WindowSizeY", wndSize.cy);
	config->SetIntValue("Settings", "WindowMaximized", maximized);
	config->SetIntValue("Settings", "ShowTitleInfo", showTitleInfo);
	config->SetIntValue("Settings", "ShowTooltips", showTooltips);
	config->SetStringValue("Settings", "Encoder", encoderID);
	config->SetStringValue("Settings", "EncoderOutDir", enc_outdir);
	config->SetStringValue("Settings", "EncoderFilenamePattern", enc_filePattern);
	config->SetIntValue("Settings", "EncodeOnTheFly", enc_onTheFly);
	config->SetIntValue("Settings", "KeepWaveFiles", enc_keepWaves);
	config->SetIntValue("Settings", "UseUnicodeFilenames", useUnicodeNames);
	config->SetIntValue("Settings", "WriteToInputDirectory", writeToInputDir);
	config->SetIntValue("Settings", "AllowOverwriteSource", allowOverwrite);
	config->SetIntValue("Settings", "EncodeToSingleFile", encodeToSingleFile);
	config->SetIntValue("Settings", "CheckUpdatesAtStartup", checkUpdatesAtStartup);

	config->SetIntValue("CDRip", "ActiveCDROM", cdrip_activedrive);
	config->SetIntValue("CDRip", "DebugCDRip", cdrip_debuglevel);
	config->SetIntValue("CDRip", "CDParanoia", cdrip_paranoia);
	config->SetIntValue("CDRip", "CDParanoiaMode", cdrip_paranoia_mode);
	config->SetIntValue("CDRip", "DetectJitterErrors", cdrip_detectJitterErrors);
	config->SetIntValue("CDRip", "DetectC2Errors", cdrip_detectC2Errors);
	config->SetIntValue("CDRip", "JitterCorrection", cdrip_jitter);
	config->SetIntValue("CDRip", "SwapChannels", cdrip_swapchannels);

	config->SetIntValue("freedb", "AutoCDDBQueries", enable_auto_cddb);
	config->SetIntValue("freedb", "OverwriteCDText", enable_overwrite_cdtext);
	config->SetIntValue("freedb", "EnableCDDBCache", enable_cddb_cache);
	config->SetIntValue("freedb", "EnableLocalCDDB", enable_local_cddb);
	config->SetStringValue("freedb", "Directory", freedb_dir);
	config->SetIntValue("freedb", "EnableRemoteCDDB", enable_remote_cddb);
	config->SetStringValue("freedb", "Server", freedb_server);
	config->SetIntValue("freedb", "Mode", freedb_mode);
	config->SetIntValue("freedb", "CDDBPPort", freedb_cddbp_port);
	config->SetStringValue("freedb", "QueryPath", freedb_query_path);
	config->SetStringValue("freedb", "SubmitPath", freedb_submit_path);
	config->SetStringValue("freedb", "eMail", freedb_email);
	config->SetIntValue("freedb", "ProxyMode", freedb_proxy_mode);
	config->SetStringValue("freedb", "Proxy", freedb_proxy);
	config->SetIntValue("freedb", "ProxyPort", freedb_proxy_port);
	config->SetStringValue("freedb", "ProxyUserName", freedb_proxy_user);
	config->SetStringValue("freedb", "ProxyPassword", freedb_proxy_password);
	config->SetIntValue("freedb", "UpdateJoblistOnSubmit", update_joblist);

	config->Save();

	return True;
}
