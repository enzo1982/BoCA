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

Array<Buffer<UnsignedByte> *, Void *>	 BoCA::PictureData::dataStore;
Array<Short>				 BoCA::PictureData::referenceStore;

Threads::Mutex				 BoCA::PictureData::mutex;

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
	if (crc != 0)
	{
		mutex.Lock();

		Int	 size = dataStore.Get(crc)->Size();

		mutex.Release();

		return size;
	}

	return 0;
}

Bool BoCA::PictureData::Clean()
{
	if (crc != 0)
	{
		mutex.Lock();

		Short	&referenceCount = referenceStore.GetReference(crc);

		if (referenceCount == 1)
		{
			delete dataStore.Get(crc);

			dataStore.Remove(crc);
			referenceStore.Remove(crc);
		}
		else
		{
			referenceCount--;
		}

		mutex.Release();

		crc = 0;
	}

	return True;
}

Bool BoCA::PictureData::Set(Void *data, Int size)
{
	Clean();

	Buffer<UnsignedByte>	*buffer = new Buffer<UnsignedByte>(size);

	memcpy(*buffer, data, size);

	crc = Hash::CRC32::Compute(*buffer);

	mutex.Lock();

	if (referenceStore.Get(crc) == 0)
	{
		dataStore.Add(buffer, crc);
		referenceStore.Add(1, crc);
	}
	else
	{
		Short	&referenceCount = referenceStore.GetReference(crc);

		referenceCount++;

		delete buffer;
	}

	mutex.Release();

	return True;
}

BoCA::PictureData &BoCA::PictureData::operator =(const PictureData &oPictureData)
{
	if (&oPictureData == this) return *this;

	crc = oPictureData.crc;

	mutex.Lock();

	Short	&referenceCount = referenceStore.GetReference(crc);

	referenceCount++;

	mutex.Release();

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
	mutex.Lock();

	const Buffer<UnsignedByte>	&buffer = *dataStore.Get(crc);

	mutex.Release();

	return buffer;
}

BoCA::PictureData::operator const UnsignedByte *() const
{
	mutex.Lock();

	const UnsignedByte	*data = *dataStore.Get(crc);

	mutex.Release();

	return data;
}
