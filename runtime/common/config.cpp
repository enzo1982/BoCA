 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
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
	enable_console		= False;

	cdrip_numdrives		= 0;
	cdrip_timeout		= 0;

	saveSettingsOnExit	= True;

	String	 programsDir = S::System::System::GetProgramFilesDirectory();

	if (Application::GetApplicationDirectory().ToUpper().StartsWith(programsDir.ToUpper()))
	{
		configDir = S::System::System::GetApplicationDataDirectory();

		if (configDir != NIL)
		{
#if defined __WIN32__ || defined __HAIKU__
			configDir.Append("freac").Append(Directory::GetDirectoryDelimiter());
#else
			configDir.Append(".freac").Append(Directory::GetDirectoryDelimiter());
#endif
		}

		Directory(configDir).Create();
	}
	else
	{
		configDir = Application::GetApplicationDirectory();
	}

	config = new Configuration(String(configDir).Append("boca").Append(Directory::GetDirectoryDelimiter()).Append("boca.xml"), True);

	LoadSettings();

	persistentIntIDs.EnableLocking();
	persistentIntValues.EnableLocking();
}

BoCA::Config::~Config()
{
	if (saveSettingsOnExit) SaveSettings();

	delete config;

	foreach (Int *value, persistentIntValues) delete value;

	persistentIntValues.RemoveAll();
	persistentIntIDs.RemoveAll();
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
	Int	 index = FindPersistentIntValueIndex(section, name);

	if (index >= 0) *(persistentIntValues.GetNth(index)) = value;

	return config->SetIntValue(section, name, value);
}

Int BoCA::Config::SetStringValue(const String &section, const String &name, const String &value)
{
	return config->SetStringValue(section, name, value);
}

Int BoCA::Config::GetIntValue(const String &section, const String &name, Int defaultValue)
{
	Int	 index = FindPersistentIntValueIndex(section, name);

	if (index >= 0) return *(persistentIntValues.GetNth(index));

	return config->GetIntValue(section, name, defaultValue);
}

String BoCA::Config::GetStringValue(const String &section, const String &name, const String &defaultValue)
{
	return config->GetStringValue(section, name, defaultValue);
}

Int BoCA::Config::FindPersistentIntValueIndex(const String &section, const String &name)
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
	return String(config->GetNthConfigurationName(n)).Replace("BoCA::", NIL);
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
	return String(config->GetConfigurationName()).Replace("BoCA::", NIL);
}

Int BoCA::Config::SetConfigurationName(const String &nConfig)
{
	return config->SetConfigurationName(String("BoCA::").Append(nConfig));
}
