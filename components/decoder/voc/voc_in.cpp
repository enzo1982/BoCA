 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "voc_in.h"

using namespace smooth::IO;

const String &BoCA::VocIn::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Creative Voice File Decoder</name>	\
	    <version>1.0</version>			\
	    <id>voc-in</id>				\
	    <type>decoder</type>			\
	    <format>					\
	      <name>Creative Voice Files</name>		\
	      <extension>voc</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

Bool BoCA::VocIn::CanOpenStream(const String &streamURI)
{
	InStream	*f_in	 = new InStream(STREAM_FILE, streamURI, IS_READONLY);
	Int		 magic = f_in->InputNumber(4);

	delete f_in;

	return (magic == 1634038339);
}

Error BoCA::VocIn::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READONLY);

	// TODO: Add more checking to this!

	Format	&format = track.GetFormat();

	track.fileSize = f_in->Size();
	format.order = BYTE_INTEL;

	// Read magic number
	for (Int i = 0; i < 30; i++) f_in->InputNumber(1);

	format.rate = UnsignedInt32(f_in->InputNumber(4));
	format.bits = UnsignedInt8(f_in->InputNumber(1));
	format.channels = UnsignedInt8(f_in->InputNumber(1));

	track.length = (track.fileSize - 42 - 4 * Int((track.fileSize - 42) / 7340032)) / (format.bits / 8);

	delete f_in;

	return Success();
}

BoCA::VocIn::VocIn()
{
	bytesLeft	= 0;
	packageSize	= 0;
}

BoCA::VocIn::~VocIn()
{
}

Bool BoCA::VocIn::Activate()
{
	InStream	*in = new InStream(STREAM_DRIVER, driver);
    
	for (Int i = 0; i < 27; i++) in->InputNumber(1); // Read magic number

	bytesLeft = in->InputNumber(3) - 12;

	delete in;

	driver->Seek(42);

	return True;
}

Bool BoCA::VocIn::Deactivate()
{
	return True;
}

Int BoCA::VocIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	data.Resize(size);

	size = driver->ReadData(data, size);

	int	 outSize = size;

	if (size > bytesLeft)
	{
		if (((unsigned char *) data + bytesLeft)[0] == 2)
		{
			Int newBytesLeft = ((unsigned char *) data + bytesLeft + 1)[0] + 256 * ((unsigned char *) data + bytesLeft + 2)[0] + 65536 * ((unsigned char *) data + bytesLeft + 3)[0];

			outSize = size - 4;

			memmove((unsigned char *) data + bytesLeft, (unsigned char *) data + bytesLeft + 4, size - bytesLeft - 4);

			data.Resize(outSize);

			bytesLeft = newBytesLeft - (size - bytesLeft - 4);
		}
	}
	else
	{
		bytesLeft -= size;
	}

	return outSize;
}
