 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

#include "format.h"
#include "config.h"

#include <stdlib.h>

static Buffer<Float32>	 dither;
static const Int	 ditherSize = 32768;

const String &BoCA::DSPFormat::GetComponentSpecs()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Components::DSP");

	static String	 componentSpecs = String("							\
													\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
		  <component>										\
		    <name>").Append(i18n->TranslateString("Sample Format Converter")).Append("</name>	\
		    <version>1.0</version>								\
		    <id>format-dsp</id>									\
		    <type>dsp</type>									\
		  </component>										\
													\
		");

	return componentSpecs;
}

BoCA::DSPFormat::DSPFormat()
{
	configLayer = NIL;

	applyDither = True;
	ditherType  = 1;
	ditherIndex = 0;
}

BoCA::DSPFormat::~DSPFormat()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DSPFormat::Activate()
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	Bool	 fp   = config->GetIntValue(ConfigureFormat::ConfigID, "Float", False);
	Int	 bits = config->GetIntValue(ConfigureFormat::ConfigID, "Bits", 16);
	Bool	 sign = config->GetIntValue(ConfigureFormat::ConfigID, "Signed", True);

	applyDither = config->GetIntValue(ConfigureFormat::ConfigID, "ApplyDither", True);
	ditherType  = config->GetIntValue(ConfigureFormat::ConfigID, "DitherType", 1);

	/* Set output format.
	 */
	const Format	&format = track.GetFormat();

	this->format = format;

	this->format.fp	  = fp;
	this->format.bits = fp ? 32   : bits;
	this->format.sign = fp ? True : sign;

	/* Generate dither table if needed.
	 */
	if (applyDither && this->format.bits < format.bits) GenerateDither();

	return True;
}

Int BoCA::DSPFormat::TransformData(Buffer<UnsignedByte> &data)
{
	/* Check if we need to do anything.
	 */
	const Format	&format = track.GetFormat();

	if (format.fp	== this->format.fp   &&
	    format.bits == this->format.bits &&
	    format.sign == this->format.sign) return data.Size();

	/* Convert samples.
	 */
	Int	 inBytesPerSample  = (	    format.bits / 8);
	Int	 outBytesPerSample = (this->format.bits / 8);

	Int	 numSamples	   = data.Size() / inBytesPerSample;

	if (format.bits < this->format.bits) data.Resize(numSamples * outBytesPerSample);

	TransformSamples(data, format, data, this->format, numSamples);

	if (format.bits > this->format.bits) data.Resize(numSamples * outBytesPerSample);

	return data.Size();
}

Void BoCA::DSPFormat::TransformSamples(const UnsignedByte *in, const Format &inFormat, UnsignedByte *out, const Format &outFormat, Int numSamples)
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Shortcuts for common conversions.
	 */
	if (inFormat.bits == 16 && outFormat.fp)
	{
		for (Int i = numSamples - 1; i >= 0; i--) ((Float32 *) out)[i] = (((Int16 *) in)[i] + 32768) / 32768.0 - 1.0;

		return;
	}

	if (inFormat.bits == 8 && outFormat.bits == 8 && inFormat.sign != outFormat.sign)
	{
		for (Int i = 0; i < numSamples; i++) out[i] = (in[i] + 128) & 0xFF;

		return;
	}

	/* Resize intermediate buffer.
	 */
	samplesBuffer.Resize(numSamples);

	Int64	*samples = samplesBuffer;

	/* Read samples.
	 */
	for (Int i = 0; i < numSamples; i++)
	{
		if	(inFormat.fp					   ) samples[i] = (((Float32 *) in)[i] + 1.0) * (1U << 31) - 0.5 - (1U << 31);

		else if	(inFormat.bits	==  8 && !inFormat.sign		   ) samples[i] = (		in [i] - 128) << 24;
		else if	(inFormat.bits	==  8 &&  inFormat.sign		   ) samples[i] =  ((Int8  *)	in)[i]	      << 24;
		else if	(inFormat.bits	== 16				   ) samples[i] =  ((Int16 *)	in)[i]	      << 16;
		else if	(inFormat.bits	== 32				   ) samples[i] =  ((Int32 *)	in)[i];

		else if	(inFormat.bits	== 24 && endianness == EndianLittle) samples[i] = in[i * 3 + 2] << 24 | in[i * 3 + 1] << 16 | in[i * 3	  ] << 8;
		else if	(inFormat.bits	== 24 && endianness == EndianBig   ) samples[i] = in[i * 3    ] << 24 | in[i * 3 + 1] << 16 | in[i * 3 + 2] << 8;
	}

	/* Apply dither.
	 */
	if (applyDither && outFormat.bits < inFormat.bits)
	{
		if (ditherType == 0) // RPDF
		{
			for (Int i = 0; i < numSamples; i++)
			{
				Float32	 r = dither[ditherIndex++ % ditherSize] - 0.5;

				samples[i] += Int32(r * (1 << (32 - outFormat.bits)));
			}
		}
		else if (ditherType == 1) // TPDF
		{
			Float32	 p = dither[ditherIndex++ % ditherSize];

			for (Int i = 0; i < numSamples; i++)
			{
				Float32	 r = dither[ditherIndex++ % ditherSize];

				samples[i] += Int32((r - p) * (1 << (32 - outFormat.bits)));

				p = r;
			}
		}
	}

	/* Apply rounding and clipping.
	 */
	if (inFormat.fp || outFormat.bits < inFormat.bits)
	{
		for (Int i = 0; i < numSamples; i++)
		{
			samples[i] += 1 << (31 - outFormat.bits);

			/* Clip out of range samples.
			 */
			if	(samples[i] >  0x7FFFFFFF) samples[i] =  0x7FFFFFFF;
			else if	(samples[i] < ~0x7FFFFFFF) samples[i] = ~0x7FFFFFFF;
		}
	}

	/* Write samples.
	 */
	for (Int i = 0; i < numSamples; i++)
	{
		if	(outFormat.fp					   ) ((Float32 *) out)[i] = (samples[i] + (1U << 31)) / ((1U << 31) + 0.5) - 1.0;

		else if	(outFormat.bits	==  8 && !outFormat.sign	   )		  out [i] = (samples[i] + (1U << 31)) >> 24;
		else if	(outFormat.bits	==  8 &&  outFormat.sign	   ) ((Int8  *)	  out)[i] =  samples[i]		      >> 24;
		else if	(outFormat.bits	== 16				   ) ((Int16 *)	  out)[i] =  samples[i]		      >> 16;
		else if	(outFormat.bits	== 32				   ) ((Int32 *)	  out)[i] =  samples[i];

		else if	(outFormat.bits	== 24 && endianness == EndianLittle) { out[i * 3 + 2] = (samples[i] >> 24) & 0xFF; out[i * 3 + 1] = (samples[i] >> 16) & 0xFF; out[i * 3    ] = (samples[i] >> 8) & 0xFF; }
		else if	(outFormat.bits	== 24 && endianness == EndianBig   ) { out[i * 3    ] = (samples[i] >> 24) & 0xFF; out[i * 3 + 1] = (samples[i] >> 16) & 0xFF; out[i * 3 + 2] = (samples[i] >> 8) & 0xFF; }
	}
}

Void BoCA::DSPFormat::GenerateDither()
{
	static Threads::Mutex	 mutex;

	Threads::Lock	 lock(mutex);

	if (dither.Size() != 0) return;

	dither.Resize(ditherSize);

	for (Int i = 0; i < ditherSize; i++) dither[i] = Float32(rand()) / RAND_MAX;
}

ConfigLayer *BoCA::DSPFormat::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureFormat();

	return configLayer;
}
