 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2023 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include <smooth/io/drivers/driver_zero.h>

#include "hdcd.h"

using namespace smooth::IO;

const String &BoCA::DSPHDCD::GetComponentSpecs()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Components::DSP");

	static String	 componentSpecs = String("						\
												\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>					\
		  <component>									\
		    <name>").Append(i18n->TranslateString("HDCD Decoder")).Append("</name>	\
		    <version>1.0</version>							\
		    <id>hdcd-dsp</id>								\
		    <type>dsp</type>								\
		  </component>									\
												\
		");

	return componentSpecs;
}

BoCA::DSPHDCD::DSPHDCD()
{
	context = NIL;
}

BoCA::DSPHDCD::~DSPHDCD()
{
}

Bool BoCA::DSPHDCD::Activate()
{
	/* Do nothing if no HDCD content.
	 */
	if (!IsHDCDContent()) return True;

	context = hdcd_new();

	/* Set output format.
	 */
	const Format	&format = track.GetFormat();

	this->format = format;

	this->format.bits = 24;

	return True;
}

Bool BoCA::DSPHDCD::Deactivate()
{
	if (context != NIL) hdcd_free(context);

	return True;
}

Int BoCA::DSPHDCD::TransformData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (context == NIL) return data.Size();

	/* Write samples to 32 bit processing buffer.
	 */
	Int16	*inSamples  = (Int16 *) (UnsignedByte *) data;
	Int	 numSamples = data.Size() / 2;

	samplesBuffer.Resize(numSamples);

	for (Int i = 0; i < numSamples; i++) samplesBuffer[i] = inSamples[i];

	/* Process samples.
	 */
	hdcd_process(context, samplesBuffer, numSamples / 2);

	/* Write processed samples back to data buffer.
	 */
	data.Resize(numSamples * 3);

	for (Int i = 0; i < numSamples; i++)
	{
		if	(endianness == EndianLittle) { data[i * 3 + 2] = (samplesBuffer[i] >> 24) & 0xFF; data[i * 3 + 1] = (samplesBuffer[i] >> 16) & 0xFF; data[i * 3    ] = (samplesBuffer[i] >> 8) & 0xFF; }
		else if	(endianness == EndianBig   ) { data[i * 3    ] = (samplesBuffer[i] >> 24) & 0xFF; data[i * 3 + 1] = (samplesBuffer[i] >> 16) & 0xFF; data[i * 3 + 2] = (samplesBuffer[i] >> 8) & 0xFF; }
	}

	return data.Size();
}

Bool BoCA::DSPHDCD::IsHDCDContent()
{
	/* Check input format.
	 */
	const Format	&format = track.GetFormat();

	if (!track.lossless || format.bits != 16 || format.channels != 2 || format.rate != 44100) return False;

	/* Create decoder for stream.
	 */
	AS::Registry		&boca	 = AS::Registry::Get();
	AS::DecoderComponent	*decoder = boca.CreateDecoderForStream(track.fileName, GetConfiguration());

	if (decoder == NIL) return False;

	decoder->SetAudioTrackInfo(track);

	static DriverZero	 zero_in;
	InStream		*stream = NIL;

	if (track.fileName.StartsWith("device://")) stream = new InStream(STREAM_DRIVER, &zero_in);
	else					    stream = new InStream(STREAM_FILE, track.fileName, IS_READ);

	stream->SetPackageSize(track.length >= 0 ? 131072 : 16384);
	stream->SetFilter(decoder);

	/* Seek to sampleOffset if necessary.
	 */
	if (track.sampleOffset > 0 && !decoder->Seek(track.sampleOffset))
	{
		Int64			 bytesLeft = track.sampleOffset * format.channels * (format.bits / 8);
		Buffer<UnsignedByte>	 buffer;

		while (bytesLeft)
		{
			buffer.Resize(Math::Min(Int64(1024), bytesLeft));

			bytesLeft -= stream->InputData(buffer, buffer.Size());
		}
	}

	/* Read beginning of audio data.
	 */
	Buffer<UnsignedByte>	 buffer(2352);

	Int		 numAudioSectors = 0;
	Bool		 hdcdDetected	 = False;
	hdcd_simple	*context	 = hdcd_new();

	while (!hdcdDetected && numAudioSectors < 75)
	{
		Int16	*inSamples  = (Int16 *) (UnsignedByte *) buffer;
		Int	 numSamples = stream->InputData(buffer, buffer.Size()) / 2;
		Int64	 energy	    = 0;

		samplesBuffer.Resize(numSamples);

		for (Int i = 0; i < numSamples; i++)
		{
			samplesBuffer[i] = inSamples[i];

			energy += Math::Abs(inSamples[i]);
		}

		if (energy > numSamples * 256) numAudioSectors++;

		/* Check for HDCD signaling codes in input.
		 */
		if (hdcd_scan(context, samplesBuffer, numSamples / 2, 0) != HDCD_NONE) hdcdDetected = True;
	}

	hdcd_free(context);

	delete stream;

	boca.DeleteComponent(decoder);

	return hdcdDetected;
}
