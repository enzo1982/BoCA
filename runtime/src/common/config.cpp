 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
	cdrip_autoRead_active	= False;
	cdrip_timeout		= 0;

	String	 programsDir = S::System::System::GetProgramFilesDirectory();

	if (Application::GetApplicationDirectory().ToUpper().StartsWith(programsDir.ToUpper()))
	{
		configDir = S::System::System::GetApplicationDataDirectory();

		if (configDir != "") configDir.Append("BonkEnc\\");

		Directory(configDir).Create();
	}
	else
	{
		configDir = Application::GetApplicationDirectory();
	}

	config = new Configuration(String(configDir).Append("boca\\boca.xml"), True);
}

BoCA::Config::~Config()
{
	delete config;
}

BoCA::Config *BoCA::Config::Get()
{
	if (instance == NIL)
	{
		instance = new Config();

		instance->LoadSettings();
	}

	return instance;
}

Void BoCA::Config::Free()
{
	if (instance != NIL)
	{
		instance->SaveSettings();

		delete instance;
	}
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

	enable_id3v1				= config->GetIntValue("Settings", "EnableID3V1", 0);
	enable_id3v2				= config->GetIntValue("Settings", "EnableID3V2", 1);
	enable_vctags				= config->GetIntValue("Settings", "EnableVorbisCommentTags", 1);
	enable_mp4meta				= config->GetIntValue("Settings", "EnableMP4Metadata", 1);
	id3v1_encoding				= config->GetStringValue("Settings", "ID3V1Encoding", "ISO-8859-1");
	id3v2_encoding				= config->GetStringValue("Settings", "ID3V2Encoding", "UTF-16LE");
	vctag_encoding				= config->GetStringValue("Settings", "VorbisCommentTagEncoding", "UTF-8");
	mp4meta_encoding			= config->GetStringValue("Settings", "MP4MetadataEncoding", "UTF-8");
	default_comment				= config->GetStringValue("Settings", "DefaultComment", String("BonkEnc Audio Encoder <http://www.bonkenc.org/>"));
	copy_picture_tags			= config->GetIntValue("Settings", "CopyPictureTags", 1);

	cdrip_activedrive			= config->GetIntValue("CDRip", "ActiveCDROM", 0);
	cdrip_debuglevel			= config->GetIntValue("CDRip", "DebugCDRip", 0);
	cdrip_paranoia				= config->GetIntValue("CDRip", "CDParanoia", 0);
	cdrip_paranoia_mode			= config->GetIntValue("CDRip", "CDParanoiaMode", 3);
	cdrip_detectJitterErrors		= config->GetIntValue("CDRip", "DetectJitterErrors", 1);
	cdrip_detectC2Errors			= config->GetIntValue("CDRip", "DetectC2Errors", 1);
	cdrip_jitter				= config->GetIntValue("CDRip", "JitterCorrection", 0);
	cdrip_swapchannels			= config->GetIntValue("CDRip", "SwapChannels", 0);
	cdrip_locktray				= config->GetIntValue("CDRip", "LockTray", 1);
	cdrip_read_cdtext			= config->GetIntValue("CDRip", "ReadCDText", 1);
	cdrip_read_cdplayerini			= config->GetIntValue("CDRip", "ReadCDPlayerIni", 1);
	cdrip_ntscsi				= config->GetIntValue("CDRip", "UseNTSCSI", 1);
	cdrip_autoRead				= config->GetIntValue("CDRip", "AutoReadContents", 1);
	cdrip_autoRip				= config->GetIntValue("CDRip", "AutoRip", 0);
	cdrip_autoEject				= config->GetIntValue("CDRip", "EjectAfterRipping", 0);
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

	config->SetIntValue("Settings", "EnableID3V1", enable_id3v1);
	config->SetIntValue("Settings", "EnableID3V2", enable_id3v2);
	config->SetIntValue("Settings", "EnableVorbisCommentTags", enable_vctags);
	config->SetIntValue("Settings", "EnableMP4Metadata", enable_mp4meta);
	config->SetStringValue("Settings", "ID3V1Encoding", id3v1_encoding);
	config->SetStringValue("Settings", "ID3V2Encoding", id3v2_encoding);
	config->SetStringValue("Settings", "VorbisCommentTagEncoding", vctag_encoding);
	config->SetStringValue("Settings", "MP4MetadataEncoding", mp4meta_encoding);
	config->SetStringValue("Settings", "DefaultComment", default_comment);
	config->SetIntValue("Settings", "CopyPictureTags", copy_picture_tags);

	config->SetIntValue("CDRip", "ActiveCDROM", cdrip_activedrive);
	config->SetIntValue("CDRip", "DebugCDRip", cdrip_debuglevel);
	config->SetIntValue("CDRip", "CDParanoia", cdrip_paranoia);
	config->SetIntValue("CDRip", "CDParanoiaMode", cdrip_paranoia_mode);
	config->SetIntValue("CDRip", "DetectJitterErrors", cdrip_detectJitterErrors);
	config->SetIntValue("CDRip", "DetectC2Errors", cdrip_detectC2Errors);
	config->SetIntValue("CDRip", "JitterCorrection", cdrip_jitter);
	config->SetIntValue("CDRip", "SwapChannels", cdrip_swapchannels);
	config->SetIntValue("CDRip", "LockTray", cdrip_locktray);
	config->SetIntValue("CDRip", "ReadCDText", cdrip_read_cdtext);
	config->SetIntValue("CDRip", "ReadCDPlayerIni", cdrip_read_cdplayerini);
	config->SetIntValue("CDRip", "UseNTSCSI", cdrip_ntscsi);
 	config->SetIntValue("CDRip", "AutoReadContents", cdrip_autoRead);
 	config->SetIntValue("CDRip", "AutoRip", cdrip_autoRip);
	config->SetIntValue("CDRip", "EjectAfterRipping", cdrip_autoEject);

	config->Save();

	return True;
}
