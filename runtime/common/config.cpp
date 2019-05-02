 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/config.h>
#include <boca/core/core.h>

using namespace smooth::GUI;

namespace BoCA
{
	class ApplicationConfig : public Config
	{
		public:
			 ApplicationConfig();
			~ApplicationConfig();
	};
};

BoCA::Config			*BoCA::Config::instance = NIL;
Array<BoCA::Config *, Void *>	 BoCA::Config::copies;

BoCA::Config::Config()
{
	saveSettingsOnExit = True;

	config = NIL;

	copies.EnableLocking();

	persistentIntIDs.EnableLocking();
	persistentIntValues.EnableLocking();
}

BoCA::Config::Config(const Config &oConfig)
{
	saveSettingsOnExit = False;

	config = new Configuration(*oConfig.config);

	for (Int i = 0; i < oConfig.persistentIntIDs.Length(); i++)
	{
		const String &nthID = oConfig.persistentIntIDs.GetNth(i);

		String	 section = nthID.Head(nthID.Find("::"));
		String	 name	 = nthID.Tail(nthID.Length() - nthID.Find("::") - 2);

		config->SetIntValue(section, name, *oConfig.persistentIntValues.GetNth(i));
	}

	persistentIntIDs.EnableLocking();
	persistentIntValues.EnableLocking();
}

BoCA::Config::~Config()
{
	if (saveSettingsOnExit) SaveSettings();

	if (config != NIL) delete config;

	foreach (Int *value, persistentIntValues) delete value;

	persistentIntValues.RemoveAll();
	persistentIntIDs.RemoveAll();
}

BoCA::Config *BoCA::Config::Get()
{
	if (instance == NIL)
	{
		instance = new ApplicationConfig();
	}

	return instance;
}

BoCA::Config *BoCA::Config::Copy(const Config *config)
{
	Config	*copy = NIL;

	if (config != NIL) copy = new Config(*config);
	else		   copy = new Config(*Get());

	copies.Add(copy);

	return copy;
}

Void BoCA::Config::Free(Config *config)
{
	if (config != NIL)
	{
		Array<Config *>::WriteLock	 lock(copies);

		foreach (Config *copy, copies)
		{
			if (copy != config) continue;

			copies.RemoveNth(foreachindex);

			delete config;

			break;
		}
	}
	else if (instance != NIL)
	{
		delete instance;

		instance = NIL;

		foreach (Config *copy, copies) delete copy;
	}
}

Void BoCA::Config::SetSaveSettingsOnExit(Bool nSaveSettingsOnExit)
{
	saveSettingsOnExit = nSaveSettingsOnExit;
}

Int BoCA::Config::SetIntValue(const String &section, const String &name, Int value)
{
	Int	 index = FindPersistentIntValueIndex(section, name);

	if (index >= 0) *(persistentIntValues.GetNth(index)) = value;

	return config->SetIntValue(section, name, value);
}

Int BoCA::Config::SetStringValue(const String &section, const String &name, const String &value)
{
	return config->SetStringValue(section, name, value);
}

Int BoCA::Config::GetIntValue(const String &section, const String &name, Int defaultValue) const
{
	Int	 index = FindPersistentIntValueIndex(section, name);

	if (index >= 0) return *(persistentIntValues.GetNth(index));

	return config->GetIntValue(section, name, defaultValue);
}

String BoCA::Config::GetStringValue(const String &section, const String &name, const String &defaultValue) const
{
	return config->GetStringValue(section, name, defaultValue);
}

Int BoCA::Config::FindPersistentIntValueIndex(const String &section, const String &name) const
{
	String	 id = String(section).Append("::").Append(name);

	for (Int i = 0; i < persistentIntIDs.Length(); i++)
	{
		const String &nthID = persistentIntIDs.GetNth(i);

		if (nthID == id) return i;
	}

	return -1;
}

Int &BoCA::Config::GetPersistentIntValue(const String &section, const String &name, Int defaultValue)
{
	Int	 index = FindPersistentIntValueIndex(section, name);

	if (index >= 0) return *(persistentIntValues.GetNth(index));

	/* We did not find the requested value.
	 */
	Int	*value = new Int(GetIntValue(section, name, defaultValue));

	persistentIntValues.Add(value);
	persistentIntIDs.Add(String(section).Append("::").Append(name));

	return *(persistentIntValues.GetLast());
}

Bool BoCA::Config::LoadSettings()
{
	String	 configurationName = config->GetStringValue("Settings", "ActiveConfiguration", "default");

	config->SetActiveConfiguration(configurationName);

	return True;
}

Bool BoCA::Config::SaveSettings()
{
	/* Save persistent values first.
	 */
	for (Int i = 0; i < persistentIntIDs.Length(); i++)
	{
		const String	&nthID = persistentIntIDs.GetNth(i);

		config->SetIntValue(nthID.Head(nthID.Find("::")), nthID.Tail(nthID.Length() - nthID.Find("::") - 2), *persistentIntValues.GetNth(i));
	}

	String	 configurationName = config->GetConfigurationName();

	config->SetActiveConfiguration("default");
	config->SetStringValue("Settings", "ActiveConfiguration", configurationName);
	config->SetActiveConfiguration(configurationName);

	config->Save();

	return True;
}

Int BoCA::Config::GetNOfConfigurations() const
{
	return config->GetNOfConfigurations();
}

String BoCA::Config::GetNthConfigurationName(Int n) const
{
	return config->GetNthConfigurationName(n).Replace("BoCA::", NIL);
}

Int BoCA::Config::AddConfiguration(const String &nConfig)
{
	return config->AddConfiguration(String("BoCA::").Append(nConfig));
}

Int BoCA::Config::RemoveConfiguration(const String &rConfig)
{
	return config->RemoveConfiguration(String("BoCA::").Append(rConfig));
}

Int BoCA::Config::SetActiveConfiguration(const String &nConfig)
{
	if (GetConfigurationName() == nConfig) return Success();

	/* Activate the requested configuration.
	 */
	Int	 result = 0;

	if (nConfig == "default") result = config->SetActiveConfiguration("default");
	else			  result = config->SetActiveConfiguration(String("BoCA::").Append(nConfig));

	/* Update persistent values.
	 */
	if (result == Success())
	{
		for (Int i = 0; i < persistentIntIDs.Length(); i++)
		{
			const String	&nthID = persistentIntIDs.GetNth(i);

			*persistentIntValues.GetNth(i) = config->GetIntValue(nthID.Head(nthID.Find("::")), nthID.Tail(nthID.Length() - nthID.Find("::") - 2), *persistentIntValues.GetNth(i));
		}
	}

	return result;
}

String BoCA::Config::GetConfigurationName() const
{
	return config->GetConfigurationName().Replace("BoCA::", NIL);
}

Int BoCA::Config::SetConfigurationName(const String &nConfig)
{
	return config->SetConfigurationName(String("BoCA::").Append(nConfig));
}

BoCA::ApplicationConfig::ApplicationConfig()
{
	String	 applicationDir	     = Application::GetApplicationDirectory();
	String	 applicationPrefix   = BoCA::GetApplicationPrefix();

	String	 programFilesDir     = S::System::System::GetProgramFilesDirectory();
	String	 applicationDataDir  = S::System::System::GetApplicationDataDirectory();
	String	 applicationCacheDir = S::System::System::GetApplicationCacheDirectory();

	configDir = applicationDir;
	cacheDir  = applicationDir;

	/* Check if configuration file exists and try to create
	 * it to check write permissions.
	 */
	File	 configFile	   = String(configDir).Append(applicationPrefix).Append(".xml");
	Bool	 configFileCreated = False;

	if (!applicationDir.ToUpper().StartsWith(programFilesDir.ToUpper()) && !configFile.Exists())
	{
		configFile.Create();
		configFileCreated = True;
	}

	/* Use application data directory if configuration still
	 * does not exist or installed in program files directory.
	 */
	if (applicationDir.ToUpper().StartsWith(programFilesDir.ToUpper()) || !configFile.Exists())
	{
		configDir = applicationDataDir;
		cacheDir  = applicationCacheDir;

		if (configDir != NIL)
		{
#if !defined(__WIN32__) && !defined(__APPLE__) && !defined(__HAIKU__)
			if (!configDir.EndsWith("/.config/")) configDir.Append(".");
#endif
			configDir.Append(applicationPrefix).Append(Directory::GetDirectoryDelimiter());
		}

		if (cacheDir != NIL)
		{
#if !defined(__WIN32__) && !defined(__APPLE__) && !defined(__HAIKU__)
			if (!cacheDir.EndsWith("/.cache/")) cacheDir.Append(".");
#endif
			cacheDir.Append(applicationPrefix).Append(Directory::GetDirectoryDelimiter());
		}

		Directory(configDir).Create();
		Directory(cacheDir).Create();
	}

	/* Remove empty file created for testing permissions.
	 */
	if (configFileCreated) configFile.Delete();

	/* Migrate old configuration file (remove this block after some time).
	 */
#ifndef __WIN32__
	File	 configFileNew = String(configDir).Append(applicationPrefix).Append(".xml");
#ifdef __APPLE__
	File	 configFileOld = String(configDir).Append("../../../.").Append(applicationPrefix).Append("/").Append(applicationPrefix).Append(".xml");
#else
	File	 configFileOld = String(configDir).Append("../../.").Append(applicationPrefix).Append("/").Append(applicationPrefix).Append(".xml");
#endif

	if (!configFileNew.Exists() && configFileOld.Exists()) configFileOld.Copy(configFileNew);
#endif

	/* Load or create actual configuration.
	 */
	config = new Configuration(String(configDir).Append(applicationPrefix).Append(".xml"), True);

	LoadSettings();
}

BoCA::ApplicationConfig::~ApplicationConfig()
{
}
