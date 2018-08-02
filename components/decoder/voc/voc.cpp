 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

	if (in.InputString(19) == "Creative Voice File" && in.InputNumber(1) == 0x1A)
	{
		/* Skip rest of header.
		 */
		in.RelSeek(6);

		/* Read block type.
		 */
		Int	 blockType = in.InputNumber(1);

		if (blockType == 0x09) return True;
	}

	return False;
}

Error BoCA::DecoderVoc::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	track.fileSize = in.Size();

	/* Skip main header.
	 */
	in.RelSeek(26);

	/* Read block type.
	 */
	Int	 blockType = in.InputNumber(1);

	if (blockType == 0x09)
	{
		/* Read block size.
		 */
		Int	 blockSize = in.InputNumber(3);

		/* Read format data.
		 */
		Format	 format = track.GetFormat();

		format.order	= BYTE_INTEL;
		format.rate	= UnsignedInt32(in.InputNumber(4));
		format.bits	= UnsignedInt8(in.InputNumber(1));
		format.channels	= UnsignedInt8(in.InputNumber(1));

		if (format.bits == 8) format.sign = False;

		track.SetFormat(format);

		in.RelSeek(6);
		in.RelSeek(blockSize - 12);

		/* Get track length.
		 */
		track.length = (blockSize - 12) / format.channels / (format.bits / 8);

		if (in.InputNumber(1) == 0x02)
		{
			do
			{
				Int	 blockSize = in.InputNumber(3);

				track.length += blockSize / format.channels / (format.bits / 8);

				in.RelSeek(blockSize);
			}
			while (in.InputNumber(1) == 0x02);
		}
		else
		{
			track.length = (track.fileSize - 42) / format.channels / (format.bits / 8);
		}
	}

	return Success();
}

BoCA::DecoderVoc::DecoderVoc()
{
	bytesLeft = 0;
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
	Int	 blockType = in.InputNumber(1);

	if (blockType == 0x09)
	{
		/* Read block size and continue.
		 */
		bytesLeft = in.InputNumber(3) - 12;

		driver->Seek(in.GetPos() + 12);
	}

	return True;
}

Int BoCA::DecoderVoc::ReadData(Buffer<UnsignedByte> &data)
{
	Int	 size	 = driver->ReadData(data, data.Size());

	if (size <= 0) return -1;

	Int	 outSize = size;

	if (size > bytesLeft)
	{
		if ((data + bytesLeft)[0] == 0x02)
		{
			Int newBytesLeft =	   (data + bytesLeft)[1] +
					     256 * (data + bytesLeft)[2] +
					   65536 * (data + bytesLeft)[3];

			outSize = size - 4;

			memmove(data + bytesLeft, data + bytesLeft + 4, size - bytesLeft - 4);

			data.Resize(outSize);

			bytesLeft = newBytesLeft - (size - bytesLeft - 4);
		}
		else
		{
			/* Possibly broken format.
			 */
			bytesLeft = driver->GetSize();
		}
	}
	else
	{
		bytesLeft -= size;
	}

	return outSize;
}
