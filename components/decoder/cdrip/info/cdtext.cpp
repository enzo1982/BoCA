 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "cdtext.h"
#include "../dllinterface.h"

typedef struct
{
	unsigned char	 packType;
	unsigned char	 trackNumber;
	unsigned char	 sequenceNumber;

	unsigned char	 characterPosition	:4;
	unsigned char	 block			:3;
	unsigned char	 bDBC			:1;

	unsigned char	 data[12];
	unsigned char	 crc0;
	unsigned char	 crc1;
}
cdTextPackage;

BoCA::CDText::CDText()
{
}

BoCA::CDText::~CDText()
{
}

Int BoCA::CDText::ReadCDText(Int drive)
{
	CDROMDRIVE	*cd = ex_CR_OpenCDROM(drive);

	if (cd == NIL) return Error();

	cdInfo.Clear();

	const int	 nBufferSize	= 4 + 8 * sizeof(cdTextPackage) * 256;
	unsigned char	*pbtBuffer	= new unsigned char [nBufferSize];
	int		 nCDTextSize	= 0;
	char		*lpZero		= NIL;

	ex_CR_ReadCDText(cd, pbtBuffer, nBufferSize, &nCDTextSize);

	if (nCDTextSize < 4) { delete [] pbtBuffer; ex_CR_CloseCDROM(cd); return Error(); }

	int		 nNumPacks		= (nCDTextSize - 4) / sizeof(cdTextPackage);
	cdTextPackage	*pCDtextPacks		= NIL;
	char		 lpszBuffer[1024]	= {'\0',};
	int		 nInsertPos		= 0;

	for (Int i = 0; i < nNumPacks; i++)
	{
		pCDtextPacks = (cdTextPackage *) &pbtBuffer[i * sizeof(cdTextPackage) + 4];

		if (pCDtextPacks->block == 0)
		{
			for (Int j = 0; j < 12; j++) lpszBuffer[nInsertPos++] = pCDtextPacks->data[j];

			while (nInsertPos > 0 && (lpZero = (char *) memchr(lpszBuffer, '\0', nInsertPos)) != NIL)
			{
				Int	 nOut = (lpZero - lpszBuffer) + 1;

				if	(pCDtextPacks->packType == 0x80) // Album/Track title
				{
					if	(pCDtextPacks->trackNumber == 0) cdInfo.SetTitle(lpszBuffer);
					else if (pCDtextPacks->trackNumber != 0) cdInfo.SetTrackTitle(pCDtextPacks->trackNumber, lpszBuffer);
				}
				else if (pCDtextPacks->packType == 0x81) // Artist name
				{
					if	(pCDtextPacks->trackNumber == 0) cdInfo.SetArtist(lpszBuffer);
					else if (pCDtextPacks->trackNumber != 0) cdInfo.SetTrackArtist(pCDtextPacks->trackNumber, lpszBuffer);
				}

				nInsertPos -= nOut;

				memmove(lpszBuffer, lpZero + 1, 1024 - nOut -1);

				pCDtextPacks->trackNumber++;

				while (nInsertPos > 0 && lpszBuffer[ 0 ] == '\0')
				{
					memmove(lpszBuffer, lpszBuffer + 1, 1024 -1);
					nInsertPos--;
				}
			}
		}
	}

	delete [] pbtBuffer;

	ex_CR_CloseCDROM(cd);

	return Success();
}

Int BoCA::CDText::ClearCDInfo()
{
	cdInfo.Clear();

	return Success();
}

const BoCA::CDInfo &BoCA::CDText::GetCDInfo() const
{
	return cdInfo;
}
