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

#include "voc.h"

using namespace smooth::IO;

const String &BoCA::DecoderVoc::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Creative Voice File Decoder</name>	\
	    <version>1.0</version>			\
	    <id>voc-dec</id>				\
	    <type>decoder</type>			\
	    <format>					\
	      <name>Creative Voice Files</name>		\
	      <lossless>true</lossless>			\
	      <extension>voc</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

Bool BoCA::DecoderVoc::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (in.InputString(19) == "Creative Voice File")
	{
		if (in.InputNumber(1) == 0x1A) return True;
	}

	return False;
}

Error BoCA::DecoderVoc::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	/* ToDo: Add more checking to this!
	 */
	track.fileSize = in.Size();

	/* Skip main header.
	 */
	in.RelSeek(26);

	/* Skip block type and size.
	 */
	in.RelSeek(4);

	/* Read format data.
	 */
	Format	 format = track.GetFormat();

	format.order	= BYTE_INTEL;
	format.rate	= UnsignedInt32(in.InputNumber(4));
	format.bits	= UnsignedInt8(in.InputNumber(1));
	format.channels	= UnsignedInt8(in.InputNumber(1));

	track.SetFormat(format);

	track.length = (track.fileSize - 42 - 4 * Int((track.fileSize - 42) / 7340032)) / format.channels / (format.bits / 8);

	return Success();
}

BoCA::DecoderVoc::DecoderVoc()
{
	bytesLeft	= 0;
	packageSize	= 0;
}

BoCA::DecoderVoc::~DecoderVoc()
{
}

Bool BoCA::DecoderVoc::Activate()
{
	InStream	 in(STREAM_DRIVER, driver);

	/* Skip main header.
	 */
	in.RelSeek(26);

	/* Read block type.
	 */
	in.InputNumber(1);

	/* Read block size and continue.
	 */
	bytesLeft = in.InputNumber(3) - 12;

	driver->Seek(42);

	return True;
}

Int BoCA::DecoderVoc::ReadData(Buffer<UnsignedByte> &data)
{
	Int	 size	 = driver->ReadData(data, data.Size());

	if (size <= 0) return -1;

	Int	 outSize = size;

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
