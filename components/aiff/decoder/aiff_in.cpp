 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "aiff_in.h"

using namespace smooth::IO;

const String &BoCA::AIFFIn::GetComponentSpecs()
{

	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Apple Audio File Decoder</name>	\
	    <version>1.0</version>			\
	    <id>aiff-in</id>				\
	    <type>decoder</type>			\
	    <format>					\
	      <name>Apple Audio Files</name>		\
	      <extension>aif</extension>		\
	      <extension>aiff</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

Bool BoCA::AIFFIn::CanOpenStream(const String &streamURI)
{
	InStream	*f_in	 = new InStream(STREAM_FILE, streamURI, IS_READONLY);
	Int		 magic = f_in->InputNumber(4);

	delete f_in;

	return (magic == 1297239878);
}

Error BoCA::AIFFIn::GetStreamInfo(const String &streamURI, Track &format)
{
	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READONLY);

	// TODO: Add more checking to this!

	format.fileSize = f_in->Size();
	format.order = BYTE_RAW;

	// Read magic number
	for (Int i = 0; i < 20; i++)
		f_in->InputNumber(1);

	format.channels = UnsignedInt16(f_in->InputNumberRaw(2));

	for (Int j = 0; j < 4; j++)
		f_in->InputNumber(1);

	format.bits = UnsignedInt16(f_in->InputNumberRaw(2));

	for (Int k = 0; k < 2; k++)
		f_in->InputNumber(1);

	format.rate = UnsignedInt16(f_in->InputNumberRaw(2));

	for (Int l = 0; l < 10; l++)
		f_in->InputNumber(1);

	format.length = UnsignedInt32(f_in->InputNumberRaw(4) - 8) / (format.bits / 8);

	delete f_in;

	return Success();
}

BoCA::AIFFIn::AIFFIn()
{
	packageSize = 0;
}

BoCA::AIFFIn::~AIFFIn()
{
}

Bool BoCA::AIFFIn::Activate()
{
	driver->Seek(54); // Skip the header

	return True;
}

Bool BoCA::AIFFIn::Deactivate()
{
	return True;
}

Int BoCA::AIFFIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	data.Resize(size);

	driver->ReadData(data, size);

	return size;
}
