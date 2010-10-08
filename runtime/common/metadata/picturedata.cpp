 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/metadata/picturedata.h>

Array<Buffer<UnsignedByte> *>	 BoCA::PictureData::dataStore;
Array<Short>			 BoCA::PictureData::referenceStore;

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
		return dataStore.Get(crc)->Size();
	}

	return 0;
}

Bool BoCA::PictureData::Clean()
{
	if (crc != 0)
	{
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

		crc = 0;
	}

	return True;
}

Bool BoCA::PictureData::Set(Void *data, Int size)
{
	Clean();

	Buffer<UnsignedByte>	*buffer = new Buffer<UnsignedByte>(size);

	memcpy(*buffer, data, size);

	Hash::CRC32	 crcProcessor(*buffer);

	crc = crcProcessor.Compute();

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

	return True;
}

BoCA::PictureData &BoCA::PictureData::operator =(const PictureData &oPictureData)
{
	if (&oPictureData == this) return *this;

	crc = oPictureData.crc;

	Short	&referenceCount = referenceStore.GetReference(crc);

	referenceCount++;

	return *this;
}

BoCA::PictureData &BoCA::PictureData::operator =(const Buffer<UnsignedByte> &buffer)
{
	Set(buffer, buffer.Size());

	return *this;
}

BoCA::PictureData::operator const Buffer<UnsignedByte> &() const
{
	return *dataStore.Get(crc);
}

BoCA::PictureData::operator const UnsignedByte *() const
{
	return *dataStore.Get(crc);
}
