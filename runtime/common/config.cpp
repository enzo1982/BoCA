 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
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

	enable_console		= False;

	cdrip_numdrives		= 0;
	cdrip_timeout		= 0;
	cdrip_autoRead_active	= False;

	saveSettingsOnExit	= True;

	String	 programsDir = S::System::System::GetProgramFilesDirectory();

	if (Application::GetApplicationDirectory().ToUpper().StartsWith(programsDir.ToUpper()))
	{
		configDir = S::System::System::GetApplicationDataDirectory();

		if (configDir != NIL) configDir.Append("freac").Append(Directory::GetDirectoryDelimiter());

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
		const String &nthID = persistentIntIDs.GetNthReference(i);

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
	return True;
}

Bool BoCA::Config::SaveSettings()
{
	/* Save persistent values first.
	 */
	for (Int i = 0; i < persistentIntIDs.Length(); i++)
	{
		const String &nthID = persistentIntIDs.GetNthReference(i);

		config->SetIntValue(nthID.Head(nthID.Find("::")), nthID.Tail(nthID.Length() - nthID.Find("::") - 2), *persistentIntValues.GetNth(i));
	}

	config->Save();

	return True;
}
