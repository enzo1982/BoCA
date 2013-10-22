 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifdef __WIN32__
#	include <windows.h>
#	include <mmreg.h>
#else
#	define WAVE_FORMAT_PCM	      0x0001
#	define WAVE_FORMAT_EXTENSIBLE 0xFFFE
#endif

#include "wave.h"

using namespace smooth::IO;

const String &BoCA::EncoderWave::GetComponentSpecs()
{
	static String	 componentSpecs = "				\
									\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
	  <component>							\
	    <name>Windows Wave File Output</name>			\
	    <version>1.0</version>					\
	    <id>wave-enc</id>						\
	    <type>encoder</type>					\
	    <format>							\
	      <name>Windows Wave Files</name>				\
	      <extension>wav</extension>				\
	      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>	\
	      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>	\
	    </format>							\
	  </component>							\
									\
	";

	return componentSpecs;
}

BoCA::EncoderWave::EncoderWave()
{
}

BoCA::EncoderWave::~EncoderWave()
{
}

Bool BoCA::EncoderWave::Activate()
{
	Buffer<unsigned char>	 buffer(44);
	OutStream		*out = new OutStream(STREAM_BUFFER, buffer, 44);

	const Format	&format = track.GetFormat();

	out->OutputString("RIFF");
	out->OutputNumber(track.length * format.channels * (format.bits / 8) + 36, 4);
	out->OutputString("WAVE");

	out->OutputString("fmt ");
	out->OutputNumber(16, 4);
	out->OutputNumber(WAVE_FORMAT_PCM, 2);
	out->OutputNumber(format.channels, 2);
	out->OutputNumber(format.rate, 4);
	out->OutputNumber(format.rate * format.channels * (format.bits / 8), 4);
	out->OutputNumber(format.channels * (format.bits / 8), 2);
	out->OutputNumber(format.bits, 2);

	out->OutputString("data");
	out->OutputNumber(track.length * format.channels * (format.bits / 8), 4);

	delete out;

	driver->WriteData(buffer, 44);

	return True;
}

Bool BoCA::EncoderWave::Deactivate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	Config		*config = Config::Get();
	const Info	&info = track.GetInfo();

	/* Write data size to header.
	 */
	Int	 dataSize = (driver->GetSize() - 44) * (track.GetFormat().bits / 8);

	driver->Seek(40);

	if (endianness == EndianLittle)	for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &dataSize) + i, 1);
	else				for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &dataSize) + i, 1);

	driver->Seek(driver->GetSize());

	/* Write RIFF tag if requested.
	 */
	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableRIFFINFOTag", True))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("riff-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 tagBuffer;

			tagger->RenderBuffer(tagBuffer, track);

			driver->WriteData(tagBuffer, tagBuffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	/* Write CART tag if requested.
	 */
	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableRIFFCartTag", True))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("cart-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 tagBuffer;

			tagger->RenderBuffer(tagBuffer, track);

			driver->WriteData(tagBuffer, tagBuffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	/* Write file size to header.
	 */
	Int	 fileSize = driver->GetSize() - 8;

	driver->Seek(4);

	if (endianness == EndianLittle)	for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &fileSize) + i, 1);
	else				for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &fileSize) + i, 1);

	driver->Seek(driver->GetSize());

	return True;
}

Int BoCA::EncoderWave::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (endianness != EndianLittle) BoCA::Utilities::SwitchBufferByteOrder(data, track.GetFormat().bits / 8);

	return driver->WriteData(data, size);
}
