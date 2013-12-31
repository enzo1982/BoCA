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

#include "resample.h"
#include "config.h"

const String &BoCA::DSPResample::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (srcdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Resampling DSP Component</name>	\
		    <version>1.0</version>			\
		    <id>resample-dsp</id>			\
		    <type>dsp</type>				\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadSRCDLL();
}

Void smooth::DetachDLL()
{
	FreeSRCDLL();
}

BoCA::DSPResample::DSPResample()
{
	state	    = NIL;

	configLayer = NIL;

	ratio	    = 1;
}

BoCA::DSPResample::~DSPResample()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DSPResample::Activate()
{
	Config	*config = Config::Get();

	const Format	&format = track.GetFormat();

	this->format	  = format;
	this->format.rate = config->GetIntValue("Resample", "Samplerate", 44100);

	/* Init the resampler only if we actually need to resample.
	 */
	if (this->format.rate != format.rate)
	{
		int	 error;

		ratio = Float(this->format.rate) / Float(format.rate);
		state = ex_src_new(config->GetIntValue("Resample", "Converter", SRC_SINC_MEDIUM_QUALITY), format.channels, &error);

		if (state == NIL)
		{
			errorState  = True;
			errorString = ex_src_strerror(error);

			return False;
		}
	}

	return True;
}

Bool BoCA::DSPResample::Deactivate()
{
	if (state != NIL)
	{
		ex_src_delete(state);

		state = NIL;
	}

	return True;
}

Int BoCA::DSPResample::TransformData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (state == NIL) return size;

	const Format	&format = track.GetFormat();

	SRC_DATA	 src_data;

	src_data.end_of_input	= 0;
	src_data.src_ratio	= ratio;
	src_data.input_frames	= size / (format.bits / 8) / format.channels;
	src_data.output_frames	= src_data.input_frames * src_data.src_ratio + 2;

	inBuffer.Resize(src_data.input_frames * format.channels);
	outBuffer.Resize(src_data.output_frames * format.channels);

	src_data.data_in	= inBuffer;
	src_data.data_out	= outBuffer;

	/* Scale up 8 and 24 bit samples.
	 */
	if (format.bits == 8)
	{
		shortBuffer.Resize(size);

		for (Int i = 0; i < size; i++) shortBuffer[i] = (data[i] - 128) * 256;
	}

	if (format.bits == 24)
	{
		intBuffer.Resize(size / (format.bits / 8));

		for (Int i = 0; i < size / (format.bits / 8); i++)
		{
			if (endianness == EndianLittle) intBuffer[i] = (int) (data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8);
			if (endianness == EndianBig   ) intBuffer[i] = (int) (data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8);
		}
	}

	/* Convert samples to float.
	 */
	if	(format.bits ==  8) ex_src_short_to_float_array((short *) (SignedInt16 *) shortBuffer, src_data.data_in, src_data.input_frames * format.channels);
	else if	(format.bits == 16) ex_src_short_to_float_array((short *) (UnsignedByte *) data, src_data.data_in, src_data.input_frames * format.channels);
	else if (format.bits == 24) ex_src_int_to_float_array((int *) (SignedInt32 *) intBuffer, src_data.data_in, src_data.input_frames * format.channels);
	else if (format.bits == 32) ex_src_int_to_float_array((int *) (UnsignedByte *) data, src_data.data_in, src_data.input_frames * format.channels);

	/* Process input and adjust buffers.
	 */
	ex_src_process(state, &src_data);

	data.Resize(src_data.output_frames_gen * (format.bits / 8) * format.channels);

	if 	(format.bits ==  8) shortBuffer.Resize(src_data.output_frames_gen * format.channels);
	else if (format.bits == 24) intBuffer.Resize(src_data.output_frames_gen * format.channels);

	/* Convert samples back to short/int.
	 */
	if	(format.bits ==  8) ex_src_float_to_short_array(src_data.data_out, (short *) (SignedInt16 *) shortBuffer, src_data.output_frames_gen * format.channels);
	else if	(format.bits == 16) ex_src_float_to_short_array(src_data.data_out, (short *) (UnsignedByte *) data, src_data.output_frames_gen * format.channels);
	else if (format.bits == 24) ex_src_float_to_int_array(src_data.data_out, (int *) (SignedInt32 *) intBuffer, src_data.output_frames_gen * format.channels);
	else if (format.bits == 32) ex_src_float_to_int_array(src_data.data_out, (int *) (UnsignedByte *) data, src_data.output_frames_gen * format.channels);

	/* Convert back to original sample resolution.
	 */
	if (format.bits == 8)
	{
		for (Int i = 0; i < src_data.output_frames_gen * format.channels; i++) data[i] = shortBuffer[i] / 256 + 128;
	}

	if (format.bits == 24)
	{
		for (Int i = 0; i < src_data.output_frames_gen * format.channels; i++)
		{
			if (endianness == EndianLittle) { data[3 * i + 2] = (intBuffer[i] >> 24) & 0xFF; data[3 * i + 1] = (intBuffer[i] >> 16) & 0xFF; data[3 * i    ] = (intBuffer[i] >> 8) & 0xFF; }
			if (endianness == EndianBig   ) { data[3 * i    ] = (intBuffer[i] >> 24) & 0xFF; data[3 * i + 1] = (intBuffer[i] >> 16) & 0xFF; data[3 * i + 2] = (intBuffer[i] >> 8) & 0xFF; }
		}
	}

	return data.Size();
}

Int BoCA::DSPResample::Flush(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (state == NIL) return 0;

	const Format	&format = track.GetFormat();

	SRC_DATA	 src_data;

	src_data.end_of_input	= 1;
	src_data.src_ratio	= ratio;
	src_data.input_frames	= 0;
	src_data.output_frames	= format.rate * src_data.src_ratio + 1;

	inBuffer.Resize(src_data.input_frames * format.channels);
	outBuffer.Resize(src_data.output_frames * format.channels);

	src_data.data_in	= inBuffer;
	src_data.data_out	= outBuffer;

	/* Flush input and adjust buffers.
	 */
	ex_src_process(state, &src_data);

	data.Resize(src_data.output_frames_gen * (format.bits / 8) * format.channels);

	if 	(format.bits ==  8) shortBuffer.Resize(src_data.output_frames_gen * format.channels);
	else if (format.bits == 24) intBuffer.Resize(src_data.output_frames_gen * format.channels);

	/* Convert samples back to short/int.
	 */
	if	(format.bits ==  8) ex_src_float_to_short_array(src_data.data_out, (short *) (SignedInt16 *) shortBuffer, src_data.output_frames_gen * format.channels);
	else if	(format.bits == 16) ex_src_float_to_short_array(src_data.data_out, (short *) (UnsignedByte *) data, src_data.output_frames_gen * format.channels);
	else if (format.bits == 24) ex_src_float_to_int_array(src_data.data_out, (int *) (SignedInt32 *) intBuffer, src_data.output_frames_gen * format.channels);
	else if (format.bits == 32) ex_src_float_to_int_array(src_data.data_out, (int *) (UnsignedByte *) data, src_data.output_frames_gen * format.channels);

	/* Convert back to original sample resolution.
	 */
	if (format.bits == 8)
	{
		for (Int i = 0; i < src_data.output_frames_gen * format.channels; i++) data[i] = shortBuffer[i] / 256 + 128;
	}

	if (format.bits == 24)
	{
		for (Int i = 0; i < src_data.output_frames_gen * format.channels; i++)
		{
			if (endianness == EndianLittle) { data[3 * i + 2] = (intBuffer[i] >> 24) & 0xFF; data[3 * i + 1] = (intBuffer[i] >> 16) & 0xFF; data[3 * i    ] = (intBuffer[i] >> 8) & 0xFF; }
			if (endianness == EndianBig   ) { data[3 * i    ] = (intBuffer[i] >> 24) & 0xFF; data[3 * i + 1] = (intBuffer[i] >> 16) & 0xFF; data[3 * i + 2] = (intBuffer[i] >> 8) & 0xFF; }
		}
	}

	return data.Size();
}

ConfigLayer *BoCA::DSPResample::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureResample();

	return configLayer;
}
