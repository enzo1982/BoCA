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

static const String	 cdTextGenre[28] =
      { "Not Used", "Not Defined", "Adult Contemporary", "Alternative Rock", "Childrens Music",
	"Classical", "Contemporary Christian", "Country", "Dance", "Easy Listening", "Erotic", "Folk",
	"Gospel", "Hip Hop", "Jazz", "Latin", "Musical", "New Age", "Opera", "Operetta", "Pop Music",
	"Rap", "Reggae", "Rock Music", "Rhythm & Blues", "Sound Effects", "Spoken Word", "World Music" };

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

	/* Read CD-Text.
	 */
	Buffer<unsigned char>	 cdTextBuffer(4 + 8 * sizeof(cdTextPackage) * 256);
	Int			 cdTextSize = 0;

	ex_CR_ReadCDText(cd, cdTextBuffer, cdTextBuffer.Size(), &cdTextSize);

	if (cdTextSize < 4) { ex_CR_CloseCDROM(cd); return Error(); }

	/* Process CD-Text packs.
	 */
	Int		 numPacks	  = (cdTextSize - 4) / sizeof(cdTextPackage);
	cdTextPackage	*currentPack	  = NIL;

	String		 data;
	char		 dataBuffer[3036] = { 0 };
	Int		 dataBufferPos	  = 0;
	Int		 genreCode	  = 0;

	for (Int i = 0; i < numPacks; i++)
	{
		currentPack = (cdTextPackage *) &cdTextBuffer[i * sizeof(cdTextPackage) + 4];

		if (currentPack->block != 0) continue;

		/* Append pack data to buffer.
		 */
		for (Int j = 0; j < 12; j++)
		{
			if (j == 0 && currentPack->packType == 0x87 && currentPack->data[j] < 31) // Genre code
			{
				genreCode = currentPack->data[0] << 8 | currentPack->data[1];
				j++;
				continue;
			}

			dataBuffer[dataBufferPos++] = currentPack->data[j];
		}

		/* Process pack data.
		 */
		if (String(dataBuffer) != "\t") data = dataBuffer;

		while (dataBufferPos > 0)
		{
			/* Update text data.
			 */
			if	(currentPack->packType == 0x80) // Album/Track title
			{
				if (currentPack->trackNumber == 0) cdInfo.SetTitle(data);
				else				   cdInfo.SetTrackTitle(currentPack->trackNumber, data);
			}
			else if (currentPack->packType == 0x81) // Artist name
			{
				if (currentPack->trackNumber == 0) cdInfo.SetArtist(data);
				else				   cdInfo.SetTrackArtist(currentPack->trackNumber, data);
			}
			else if (currentPack->packType == 0x82) // Songwriter
			{
				if (currentPack->trackNumber == 0) cdInfo.SetSongwriter(data);
				else				   cdInfo.SetTrackSongwriter(currentPack->trackNumber, data);
			}
			else if (currentPack->packType == 0x83) // Composer
			{
				if (currentPack->trackNumber == 0) cdInfo.SetComposer(data);
				else				   cdInfo.SetTrackComposer(currentPack->trackNumber, data);
			}
			else if (currentPack->packType == 0x84) // Arranger
			{
				if (currentPack->trackNumber == 0) cdInfo.SetArranger(data);
				else				   cdInfo.SetTrackArranger(currentPack->trackNumber, data);
			}
			else if (currentPack->packType == 0x85) // Message
			{
				if (currentPack->trackNumber == 0) cdInfo.SetComment(data);
				else				   cdInfo.SetTrackComment(currentPack->trackNumber, data);
			}
			else if (currentPack->packType == 0x86) // Catalog number
			{
				cdInfo.SetCatalog(data);
			}
			else if (currentPack->packType == 0x87) // Genre
			{
				cdInfo.SetGenre(data);
			}
			else if (currentPack->packType == 0x8e) // UPC/EAN and ISRC
			{
				if (currentPack->trackNumber == 0) cdInfo.SetBarcode(data);
				else				   cdInfo.SetTrackISRC(currentPack->trackNumber, data);
			}

			/* Look for string terminator.
			 */
			char	*lpZero = (char *) memchr(dataBuffer, 0, dataBufferPos);

			if (lpZero == NIL) break;

			/* Shift buffer if terminated.
			 */
			Int	 nRemove = (lpZero - dataBuffer) + 1;

			memmove(dataBuffer, dataBuffer + nRemove, sizeof(dataBuffer) - nRemove);
			dataBufferPos -= nRemove;

			/* Skip zero bytes.
			 */
			while (dataBufferPos > 0 && dataBuffer[0] == 0)
			{
				memmove(dataBuffer, dataBuffer + 1, sizeof(dataBuffer) - 1);
				dataBufferPos--;
			}

			/* Bump track number and update data.
			 */
			currentPack->trackNumber++;

			if (dataBufferPos && String(dataBuffer) != "\t") data = dataBuffer;
		}
	}

	ex_CR_CloseCDROM(cd);

	if (cdInfo.GetGenre().Trim() == NIL && genreCode != 0) cdInfo.SetGenre(cdTextGenre[genreCode]);

	return Success();
}

const BoCA::CDInfo &BoCA::CDText::GetCDInfo() const
{
	return cdInfo;
}
