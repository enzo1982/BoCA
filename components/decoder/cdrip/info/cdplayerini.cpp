 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "cdplayerini.h"
#include "../dllinterface.h"

using namespace smooth::IO;

BoCA::CDPlayerIni::CDPlayerIni()
{
}

BoCA::CDPlayerIni::~CDPlayerIni()
{
}

Int BoCA::CDPlayerIni::ReadCDInfo()
{
	cdInfo.RemoveAll();

	Int	 numTocEntries = ex_CR_GetNumTocEntries();
	Int	 discID = 0;

	for (Int i = 0; i < numTocEntries; i++)
	{
		int	 startSector = ex_CR_GetTocEntry(i).dwStartSector + 150;

		int	 minutes = startSector / 75 / 60;
		int	 seconds = startSector / 75 % 60;
		int	 frames	 = startSector % 75;

		discID += minutes * 0x10000 + seconds * 0x100 + frames;
	}

	if (numTocEntries < 3)
	{
		discID += ex_CR_GetTocEntry(0).dwStartSector;
		discID += ex_CR_GetTocEntry(numTocEntries).dwStartSector;
	}

	String	 discIDString = DiscIDToString(discID);

	/* Strip leading zeroes.
	 */
	while (discIDString.StartsWith("0")) discIDString = discIDString.Tail(discIDString.Length() - 1);

	/* Open cdplayer.ini
	 */
	InStream	*in = new InStream(STREAM_FILE, S::System::System::GetWindowsRootDirectory().Append("cdplayer.ini"), IS_READ);
	String		 idString = String("[").Append(discIDString).Append("]");
	String		 result;

	while (in->GetPos() < in->Size())
	{
		if (in->InputLine().ToLower() == idString)
		{
			while (in->GetPos() < in->Size())
			{
				String	 line = in->InputLine();

				if (line.ToLower().StartsWith("artist="))
				{
					String	 artist;

					for (Int i = 7; i < line.Length(); i++) artist[i - 7] = line[i];

					cdInfo.Add(artist, 0);
				}
				else if (line.ToLower().StartsWith("title="))
				{
					String	 title;

					for (Int i = 6; i < line.Length(); i++) title[i - 6] = line[i];

					cdInfo.Add(title, 100);
				}
				else if (line.StartsWith("0=") || line.ToInt() > 0)
				{
					String	 title;
					Int	 length = 2 + (line.ToInt() >= 10 ? 1 : 0);

					for (Int i = length; i < line.Length(); i++) title[i - length] = line[i];

					cdInfo.Add(title, line.ToInt() + 1);
				}
				else if (line.StartsWith("["))
				{
					break;
				}
			}

			break;
		}
	}

	delete in;

	return Success();
}

Int BoCA::CDPlayerIni::ClearCDInfo()
{
	cdInfo.RemoveAll();

	return Success();
}

const Array<String> &BoCA::CDPlayerIni::GetCDInfo()
{
	return cdInfo;
}

String BoCA::CDPlayerIni::DiscIDToString(Int discID)
{
	String	 result;

	for (Int i = 28; i >= 0; i -= 4)
	{
		if (((discID >> i) & 15) <= 9)	result[(28 - i) / 4] = '0' + ((discID >> i) & 15);
		else				result[(28 - i) / 4] = 'a' + ((discID >> i) & 15) - 10;
	}

	return result;
}
