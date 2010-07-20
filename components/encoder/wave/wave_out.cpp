 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "wave_out.h"

using namespace smooth::IO;

const String &BoCA::WaveOut::GetComponentSpecs()
{
	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>Windows Wave File Output</name>	\
	    <version>1.0</version>			\
	    <id>wave-out</id>				\
	    <type>encoder</type>			\
	    <format>					\
	      <name>Windows Wave Files</name>		\
	      <extension>wav</extension>		\
	    </format>					\
	  </component>					\
							\
	";

	return componentSpecs;
}

BoCA::WaveOut::WaveOut()
{
	nOfSamples = 0;
}

BoCA::WaveOut::~WaveOut()
{
}

Bool BoCA::WaveOut::Activate()
{
	Buffer<unsigned char>	 buffer(44);
	OutStream		*out = new OutStream(STREAM_BUFFER, buffer, 44);

	const Format	&format = track.GetFormat();

	out->OutputString("RIFF");
	out->OutputNumber(track.length * (format.bits / 8) + 36, 4);
	out->OutputString("WAVE");
	out->OutputString("fmt ");

	out->OutputNumber(16, 4);
	out->OutputNumber(1, 2);
	out->OutputNumber(format.channels, 2);
	out->OutputNumber(format.rate, 4);
	out->OutputNumber(format.rate * format.channels * (format.bits / 8), 4);
	out->OutputNumber(format.channels * (format.bits / 8), 2);

	out->OutputNumber(format.bits, 2);
	out->OutputString("data");
	out->OutputNumber(track.length * (format.bits / 8), 4);

	delete out;

	driver->WriteData(buffer, 44);

	return True;
}

Bool BoCA::WaveOut::Deactivate()
{
	Config		*config = Config::Get();
	const Info	&info = track.GetInfo();

	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableRIFFINFOTag", False))
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

	/* Write file and data size to header.
	 */
	Int	 fileSize = driver->GetSize() - 8;
	Int	 dataSize = nOfSamples * (track.GetFormat().bits / 8);

	driver->Seek(4);
	driver->WriteData((unsigned char *) &fileSize, 4);

	driver->Seek(40);
	driver->WriteData((unsigned char *) &dataSize, 4);

	return True;
}

Int BoCA::WaveOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	nOfSamples += (size / (track.GetFormat().bits / 8));

	return driver->WriteData(data, size);
}
