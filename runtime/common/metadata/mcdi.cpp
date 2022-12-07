 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/metadata/mcdi.h>

#ifdef __WIN32__
#	include <winsock2.h>
#else
#	include <arpa/inet.h>
#endif

BoCA::MCDI::MCDI(const Buffer<UnsignedByte> &iData)
{
	SetData(iData);
}

BoCA::MCDI::MCDI(const MCDI &oMCDI)
{
	*this = oMCDI;
}

BoCA::MCDI::~MCDI()
{
}

BoCA::MCDI &BoCA::MCDI::operator =(const MCDI &oMCDI)
{
	if (&oMCDI == this) return *this;

	SetData(oMCDI.GetData());

	return *this;
}

Bool BoCA::MCDI::operator ==(const MCDI &oMCDI) const
{
	Int	 size  = data.Size();
	Int	 oSize = oMCDI.GetData().Size();

	if (size != oSize) return False;

	if (size == 0 || memcmp((UnsignedByte *) oMCDI.GetData(), (UnsignedByte *) data, size) == 0) return True;
	else											     return False;
}

Bool BoCA::MCDI::operator !=(const MCDI &oMCDI) const
{
	return !(*this == oMCDI);
}

Bool BoCA::MCDI::IsValid() const
{
	if (GetNumberOfEntries() <= 0) return False;

	for (Int i = 1; i < GetNumberOfEntries(); i++)
	{
		if (GetNthEntryOffset(i - 1) >= GetNthEntryOffset(i)) return False;
	}

	return True;
}

Int BoCA::MCDI::GetNumberOfEntries() const
{
	if (data.Size() < 2) return 0;

	return (ntohs(((UnsignedInt16 *) (UnsignedByte *) data)[0]) - 2 - 8) / 8;
}

Int BoCA::MCDI::GetNthEntryOffset(Int n) const
{
	if (data.Size() < 2 + (8 * (n + 1))) return 0;

	long	 lba = ntohl(((UnsignedInt32 *) (UnsignedByte *) data)[1 + 2 * n + 1]);

	/* Convert 24 bit signed value to 32 bit.
	 */
	if (lba & (1 << 23)) lba = -((~lba & ((1 << 24) - 1)) + 1);

	return lba;
}

Int BoCA::MCDI::GetNthEntryType(Int n) const
{
	if (data.Size() < 2 + (8 * (n + 1))) return 0;

	if (GetNthEntryTrackNumber(n) == 0xAA) return ENTRY_LEADOUT;

	if (data[4 + 8 * n + 1] & (1 << 2)) return ENTRY_DATA;
	else				    return ENTRY_AUDIO;
}

Bool BoCA::MCDI::GetNthEntryPreEmphasis(Int n) const
{
	if (data.Size() < 2 + (8 * (n + 1))) return False;

	if (GetNthEntryType(n) != ENTRY_AUDIO) return False;

	if (data[4 + 8 * n + 1] & 1) return True;
	else			     return False;
}

Int BoCA::MCDI::GetNthEntryTrackNumber(Int n) const
{
	if (data.Size() < 2 + (8 * (n + 1))) return 0;

	return data[4 + 8 * n + 2];
}

Int BoCA::MCDI::GetNthEntryTrackLength(Int n) const
{
	if (data.Size() < 2 + (8 * (n + 2))) return 0;

	long	 sectors = GetNthEntryOffset(n + 1) - GetNthEntryOffset(n);

	/* Strip 11400 sectors off of the track length if
	 * it is the last track before a new session.
	 */
	if ((GetNthEntryType(n) != GetNthEntryType(n + 1) && GetNthEntryType(n + 1) != ENTRY_LEADOUT) ||
	    (n < GetNumberOfEntries() - 1 && GetNthEntryOffset(n + 1) >= GetNthEntryOffset(n + 2))) sectors -= 11400;

	return sectors;
}

Int BoCA::MCDI::GetNumberOfAudioTracks() const
{
	Int	 numAudioTracks = 0;

	for (Int i = 0; i < GetNumberOfEntries(); i++)
	{
		/* Count only tracks with a positive length
		 * to avoid being fooled by invalid TOCs.
		 */
		if (GetNthEntryType(i) == ENTRY_AUDIO && GetNthEntryOffset(i + 1) - GetNthEntryOffset(i) > 0) numAudioTracks++;
	}

	return numAudioTracks;
}

Int BoCA::MCDI::GetNumberOfDataTracks() const
{
	Int	 numDataTracks = 0;

	for (Int i = 0; i < GetNumberOfEntries(); i++)
	{
		/* Count only tracks with a positive length
		 * to avoid being fooled by invalid TOCs.
		 */
		if (GetNthEntryType(i) == ENTRY_DATA && GetNthEntryOffset(i + 1) - GetNthEntryOffset(i) > 0) numDataTracks++;
	}

	return numDataTracks;
}

String BoCA::MCDI::GetOffsetString() const
{
	String	 offsets = Number((Int64) GetNumberOfAudioTracks()).ToHexString();

	for (Int i = 0; i <= GetNumberOfEntries(); i++)
	{
		offsets.Append("+").Append(Number((Int64) GetNthEntryOffset(i) + 150).ToHexString());
	}

	return offsets.ToUpper();
}

Bool BoCA::MCDI::SetData(const Buffer<UnsignedByte> &nData)
{
	data.Resize(nData.Size());

	if (data.Size() > 0) memcpy((UnsignedByte *) data, (UnsignedByte *) nData, nData.Size());

	return True;
}

const Buffer<UnsignedByte> &BoCA::MCDI::GetData() const
{
	return data;
}
