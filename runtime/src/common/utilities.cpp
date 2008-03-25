 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/utilities.h>

using namespace smooth::GUI::Dialogs;

Void BoCA::Utilities::WarningMessage(const String &message, const String &replace)
{
	QuickMessage(String(message).Replace("%1", replace), "Warning", MB_OK, IDI_EXCLAMATION);
}

Void BoCA::Utilities::ErrorMessage(const String &message, const String &replace)
{
	QuickMessage(String(message).Replace("%1", replace), "Error", MB_OK, IDI_HAND);
}

String BoCA::Utilities::GetNonUnicodeTempFileName(const String &fileName)
{
	String	 rVal	= fileName;
	Int	 lastBs	= -1;

	/* Replace Unicode characters in input file name
	 */
	for (Int i = 0; i < rVal.Length(); i++)
	{
		if (rVal[i] > 127)	rVal[i] = '#';
		if (rVal[i] == '\\')	lastBs = i;
	}

	String	 tempDir = S::System::System::GetTempDirectory();

	for (Int j = lastBs + 1; j < rVal.Length(); j++)
	{
		tempDir[tempDir.Length()] = rVal[j];
	}

	return tempDir.Append(".temp");
}
