 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "au.h"

using namespace smooth::IO;

const String &BoCA::DecoderSunAu::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Sun Audio File Decoder</name>		\
	    <version>1.0</version>			\
	    <id>au-dec</id>				\
	    <type>decoder</type>			\
	    <format>					\
	      <name>Sun Audio Files</name>		\
	      <lossless>true</lossless>			\
	      <extension>au</extension>			\
	      <extension>snd</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

Bool BoCA::DecoderSunAu::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (in.InputString(4) == ".snd") return True;

	return False;
}

Error BoCA::DecoderSunAu::GetStreamInfo(const String &streamURI, Track &track)
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

BoCA::DecoderSunAu::DecoderSunAu()
{
	packageSize = 0;

	dataOffset  = 0;
}

BoCA::DecoderSunAu::~DecoderSunAu()
{
}

Bool BoCA::DecoderSunAu::Activate()
{
	InStream	*in = new InStream(STREAM_DRIVER, driver);

	/* Read magic number.
	 */
	in->InputNumber(4);

	dataOffset = in->InputNumberRaw(4);

	delete in;

	driver->Seek(dataOffset);

	return True;
}

Bool BoCA::DecoderSunAu::Deactivate()
{
	return True;
}

Bool BoCA::DecoderSunAu::Seek(Int64 samplePosition)
{
	driver->Seek(dataOffset + samplePosition * track.GetFormat().channels * (track.GetFormat().bits / 8));

	return True;
}

Int BoCA::DecoderSunAu::ReadData(Buffer<UnsignedByte> &data)
{
	if (driver->GetPos() == driver->GetSize()) return -1;

	Int	 size = driver->ReadData(data, data.Size());

	/* Convert 8 bit samples to unsigned.
	 */
	if (track.GetFormat().bits == 8) for (Int i = 0; i < size; i++) data[i] = data[i] + 128;

	return size;
}
