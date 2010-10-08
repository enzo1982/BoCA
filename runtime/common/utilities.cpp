 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/utilities.h>
#include <boca/common/config.h>
#include <boca/common/i18n.h>

using namespace smooth::System;
using namespace smooth::GUI::Dialogs;

Void BoCA::Utilities::WarningMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Messages");

	if (!config->enable_console) QuickMessage(String(i18n->TranslateString(message)).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Warning"), MB_OK, IDI_EXCLAMATION);
	else			     Console::OutputString(String("\n").Append(i18n->TranslateString("Warning")).Append(": ").Append(String(i18n->TranslateString(message)).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

Void BoCA::Utilities::ErrorMessage(const String &message, const String &replace1, const String &replace2)
{
	Config	*config	= Config::Get();
	I18n	*i18n	= I18n::Get();

	i18n->SetContext("Messages");

	if (!config->enable_console) QuickMessage(String(i18n->TranslateString(message)).Replace("%1", replace1).Replace("%2", replace2), i18n->TranslateString("Error"), MB_OK, IDI_HAND);
	else			     Console::OutputString(String("\n").Append(i18n->TranslateString("Error")).Append(": ").Append(String(i18n->TranslateString(message)).Replace("%1", replace1).Replace("%2", replace2)).Append("\n"));
}

String BoCA::Utilities::GetNonUnicodeTempFileName(const String &fileName)
{
	String	 rVal	= fileName;
	Int	 lastBs	= -1;

	/* Replace Unicode characters in input file name
	 */
	for (Int i = 0; i < rVal.Length(); i++)
	{
		if (rVal[i] > 127)			rVal[i] = '#';
		if (rVal[i] == '\\' || rVal[i] == '/')	lastBs = i;
	}

	String	 tempDir = S::System::System::GetTempDirectory();

	for (Int j = lastBs + 1; j < rVal.Length(); j++)
	{
		tempDir[tempDir.Length()] = rVal[j];
	}

	return tempDir.Append(".temp");
}

String BoCA::Utilities::ReplaceIncompatibleCharacters(const String &string)
{
	String	 rVal;

	for (Int k = 0, b = 0; k < string.Length(); k++)
	{
		if	(string[k] == '\"')			      { rVal[k + b] = '\''; rVal[k + ++b] = '\''; }
		else if (string[k] == '?')				b--;
		else if (string[k] == '|')				rVal[k + b] = '_';
		else if (string[k] == '*')				b--;
		else if (string[k] == '<')				rVal[k + b] = '(';
		else if (string[k] == '>')				rVal[k + b] = ')';
		else if (string[k] == ':')				b--;
		else if (string[k] == '/')				rVal[k + b] = '-';
		else if (string[k] == '\\')				rVal[k + b] = '-';
		else if (string[k] >= 256  && !Setup::enableUnicode)	rVal[k + b] = '#';
		else							rVal[k + b] = string[k];
	}

	return rVal;
}
