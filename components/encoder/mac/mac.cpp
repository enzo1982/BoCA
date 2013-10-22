 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "mac.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderMAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (macdll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>Monkey's Audio Encoder</name>				\
		    <version>1.0</version>					\
		    <id>mac-enc</id>						\
		    <type>encoder</type>					\
		    <format>							\
		      <name>Monkey's Audio</name>				\
		      <extension>ape</extension>				\
		      <extension>mac</extension>				\
		      <tag id=\"apev2-tag\" mode=\"append\">APEv2</tag>		\
		    </format>							\
		  </component>							\
										\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadMACDLL();
}

Void smooth::DetachDLL()
{
	FreeMACDLL();
}


BoCA::EncoderMAC::EncoderMAC()
{
	configLayer = NIL;

	hAPECompress = NIL;
}

BoCA::EncoderMAC::~EncoderMAC()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderMAC::Activate()
{
	int	 nRetVal = 0;

	const Format	&format = track.GetFormat();

	/* Create encoder and retrieve handle.
	 */
	hAPECompress = ex_APECompress_Create(&nRetVal);

	/* Init encoder using output file name and format info.
	 */
	WAVEFORMATEX	 waveFormat;

	waveFormat.wFormatTag		= WAVE_FORMAT_PCM;
	waveFormat.nChannels		= format.channels;
	waveFormat.nSamplesPerSec	= format.rate;
	waveFormat.nAvgBytesPerSec	= format.rate * format.channels * (format.bits / 8);
	waveFormat.nBlockAlign		= format.channels * (format.bits / 8);
	waveFormat.wBitsPerSample	= format.bits;
	waveFormat.cbSize		= 0;

	ex_APECompress_Start(hAPECompress, Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), &waveFormat, MAX_AUDIO_BYTES_UNKNOWN, (Config::Get()->GetIntValue("MAC", "CompressionMode", 2) + 1) * 1000, NIL, CREATE_WAV_HEADER_ON_DECOMPRESSION);

	return True;
}

Bool BoCA::EncoderMAC::Deactivate()
{
	/* Finish encoding and destroy the encoder.
	 */
	ex_APECompress_Finish(hAPECompress, NIL, 0, 0);

	ex_APECompress_Destroy(hAPECompress);

	/* Stream contents of created APE file to output driver
	 */
	InStream		 in(STREAM_FILE, Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), IS_READ);
	Buffer<UnsignedByte>	 buffer(1024);
	Int			 bytesLeft = in.Size();

	while (bytesLeft)
	{
		in.InputData(buffer, Math::Min(1024, bytesLeft));

		driver->WriteData(buffer, Math::Min(1024, bytesLeft));

		bytesLeft -= Math::Min(1024, bytesLeft);
	}

	in.Close();

	const Info	&info = track.GetInfo();

	if ((info.artist != NIL || info.title != NIL) && Config::Get()->GetIntValue("Tags", "EnableAPEv2", True))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("apev2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 tagBuffer;

			tagger->RenderBuffer(tagBuffer, track);

			driver->WriteData(tagBuffer, tagBuffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	File(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out")).Delete();

 	return True;
}

Int BoCA::EncoderMAC::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	/* Hand data over to the encoder.
	 */
	ex_APECompress_AddData(hAPECompress, data, size);

	return size;
}

ConfigLayer *BoCA::EncoderMAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMAC();

	return configLayer;
}
