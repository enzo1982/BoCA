 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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

Int BoCA::CDPlayerIni::ReadCDInfo(Int drive)
{
	CDROMDRIVE	*cd = ex_CR_OpenCDROM(drive);

	if (cd == NIL) return Error();

	cdInfo.Clear();

	Int		 numTocEntries = ex_CR_GetNumTocEntries(cd);
	UnsignedInt32	 discID = 0;

	for (Int i = 0; i < numTocEntries; i++)
	{
		Int	 startSector = ex_CR_GetTocEntry(cd, i).dwStartSector + 150;

		Int	 minutes = startSector / 75 / 60;
		Int	 seconds = startSector / 75 % 60;
		Int	 frames	 = startSector % 75;

		discID += minutes * 0x10000 + seconds * 0x100 + frames;
	}

	if (numTocEntries < 3)
	{
		discID += ex_CR_GetTocEntry(cd, 0).dwStartSector;
		discID += ex_CR_GetTocEntry(cd, numTocEntries).dwStartSector;
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

					cdInfo.SetArtist(artist);
				}
				else if (line.ToLower().StartsWith("title="))
				{
					String	 title;

					for (Int i = 6; i < line.Length(); i++) title[i - 6] = line[i];

					cdInfo.SetTitle(title);
				}
				else if (line.StartsWith("0=") || line.ToInt() > 0)
				{
					String	 title;
					Int	 length = 2 + (line.ToInt() >= 10 ? 1 : 0);

					for (Int i = length; i < line.Length(); i++) title[i - length] = line[i];

					cdInfo.SetTrackTitle(line.ToInt() + 1, title);
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

	ex_CR_CloseCDROM(cd);

	return Success();
}

const BoCA::CDInfo &BoCA::CDPlayerIni::GetCDInfo() const
{
	return cdInfo;
}

String BoCA::CDPlayerIni::DiscIDToString(UnsignedInt32 discID)
{
	String	 result;

	for (Int i = 28; i >= 0; i -= 4)
	{
		if (((discID >> i) & 15) <= 9)	result[(28 - i) / 4] = '0' + ((discID >> i) & 15);
		else				result[(28 - i) / 4] = 'a' + ((discID >> i) & 15) - 10;
	}

	return result;
}
