 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "au_in.h"

using namespace smooth::IO;

const String &BoCA::SunAuIn::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Sun Audio File Decoder</name>		\
	    <version>1.0</version>			\
	    <id>au-in</id>				\
	    <type>decoder</type>			\
	    <format>					\
	      <name>Sun Audio Files</name>		\
	      <extension>au</extension>			\
	      <extension>snd</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

Bool BoCA::SunAuIn::CanOpenStream(const String &streamURI)
{
	InStream	*f_in	 = new InStream(STREAM_FILE, streamURI, IS_READ);
	Int		 magic = f_in->InputNumber(4);

	delete f_in;

	return (magic == 1684960046);
}

Error BoCA::SunAuIn::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*f_in	= new InStream(STREAM_FILE, streamURI, IS_READ);
	Format		 format = track.GetFormat();

	track.fileSize	= f_in->Size();
	format.order	= BYTE_RAW;

	/* Skip magic number and data offset.
	 */
	f_in->RelSeek(8);

	track.length	= UnsignedInt32(f_in->InputNumberRaw(4));
	format.bits	= UnsignedInt32(f_in->InputNumberRaw(4));

	if	(format.bits == 2) format.bits =  8;
	else if (format.bits == 3) format.bits = 16;
	else if (format.bits == 4) format.bits = 24;
	else if (format.bits == 5) format.bits = 32;
	else			   { errorState = True; errorString = "Unsupported audio format"; }

	if (!errorState)
	{
		format.rate	= UnsignedInt32(f_in->InputNumberRaw(4));
		format.channels	= UnsignedInt32(f_in->InputNumberRaw(4));

		track.length	= track.length / format.channels / (format.bits / 8);
	}

	track.SetFormat(format);

	delete f_in;

	if (errorState)	return Error();
	else		return Success();
}

BoCA::SunAuIn::SunAuIn()
{
	packageSize = 0;
}

BoCA::SunAuIn::~SunAuIn()
{
}

Bool BoCA::SunAuIn::Activate()
{
	InStream	*in = new InStream(STREAM_DRIVER, driver);
    
	/* Read magic number.
	 */
	in->InputNumber(4);

	Int		 dataOffset = in->InputNumberRaw(4);

	delete in;

	driver->Seek(dataOffset);

	return True;
}

Bool BoCA::SunAuIn::Deactivate()
{
	return True;
}

Int BoCA::SunAuIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (driver->GetPos() == driver->GetSize()) return -1;

	data.Resize(size);

	size = driver->ReadData(data, size);

	/* Convert 8 bit samples to unsigned.
	 */
	if (track.GetFormat().bits == 8) for (Int i = 0; i < size; i++) data[i] = data[i] + 128;

	return size;
}
