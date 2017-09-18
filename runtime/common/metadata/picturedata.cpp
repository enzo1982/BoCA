 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/metadata/picturedata.h>

using namespace smooth::Threads;

Array<Buffer<UnsignedByte> *, Void *>	 BoCA::PictureData::dataStore;
Array<Short>				 BoCA::PictureData::referenceStore;

Mutex					 BoCA::PictureData::mutex;

BoCA::PictureData::PictureData()
{
	crc = 0;
}

BoCA::PictureData::PictureData(const PictureData &iPictureData)
{
	crc = 0;

	*this = iPictureData;
}

BoCA::PictureData::~PictureData()
{
	Clean();
}

Int BoCA::PictureData::Size() const
{
	if (crc == 0) return 0;

	Lock	 lock(mutex);

	return dataStore.Get(crc)->Size();
}

Bool BoCA::PictureData::Clean()
{
	if (crc == 0) return True;

	UnsignedInt32	 pCRC = crc;

	crc = 0;

	Lock	 lock(mutex);
	Short	&referenceCount = referenceStore.GetReference(pCRC);

	if (--referenceCount == 0)
	{
		delete dataStore.Get(pCRC);

		dataStore.Remove(pCRC);
		referenceStore.Remove(pCRC);
	}

	return True;
}

Bool BoCA::PictureData::Set(Void *data, Int size)
{
	Clean();

	if (size == 0) return False;

	Buffer<UnsignedByte>	*buffer = new Buffer<UnsignedByte>(size);

	memcpy(*buffer, data, size);

	UnsignedInt32	 nCRC = Hash::CRC32::Compute(*buffer);

	Lock	 lock(mutex);

	if (referenceStore.Get(nCRC) == 0)
	{
		dataStore.Add(buffer, nCRC);
		referenceStore.Add(1, nCRC);
	}
	else
	{
		Short	&referenceCount = referenceStore.GetReference(nCRC);

		referenceCount++;

		delete buffer;
	}

	crc = nCRC;

	return True;
}

BoCA::PictureData &BoCA::PictureData::operator =(const PictureData &oPictureData)
{
	if (&oPictureData == this) return *this;

	Clean();

	if (oPictureData.crc == 0) return *this;

	Lock	 lock(mutex);
	Short	&referenceCount = referenceStore.GetReference(oPictureData.crc);

	referenceCount++;

	crc = oPictureData.crc;

	return *this;
}

BoCA::PictureData &BoCA::PictureData::operator =(const Buffer<UnsignedByte> &buffer)
{
	Set(buffer, buffer.Size());

	return *this;
}

Bool BoCA::PictureData::operator ==(const PictureData &oPictureData) const
{
	return (crc == oPictureData.crc);
}

Bool BoCA::PictureData::operator !=(const PictureData &oPictureData) const
{
	return (crc != oPictureData.crc);
}

BoCA::PictureData::operator const Buffer<UnsignedByte> &() const
{
	static Buffer<UnsignedByte>	 zero;

	if (crc == 0) return zero;

	Lock	 lock(mutex);

	return *dataStore.Get(crc);
}

BoCA::PictureData::operator const UnsignedByte *() const
{
	if (crc == 0) return NIL;

	Lock	 lock(mutex);

	return *dataStore.Get(crc);
}
