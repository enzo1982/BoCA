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

#include "resample_dsp.h"
#include "config.h"
#include "dllinterface.h"

const String &BoCA::ResampleDSP::GetComponentSpecs()
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

BoCA::ResampleDSP::ResampleDSP()
{
	state	    = NIL;

	configLayer = NIL;

	ratio	    = 1;
}

BoCA::ResampleDSP::~ResampleDSP()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::ResampleDSP::Activate()
{
	Config	*config = Config::Get();

	const Format	&format = track.GetFormat();

	this->format	  = format;
	this->format.rate = config->GetIntValue("Resample", "Samplerate", 44100);

	/* Init the resampler only if we actually need to resample.
	 */
	if (this->format.rate != format.rate)
	{
		if (this->format.bits != 16 && this->format.bits != 32)
		{
			errorState  = True;
			errorString = "Resampler input format not supported!";

			return False;
		}

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

Bool BoCA::ResampleDSP::Deactivate()
{
	if (state != NIL)
	{
		ex_src_delete(state);

		state = NIL;
	}

	return True;
}

Int BoCA::ResampleDSP::TransformData(Buffer<UnsignedByte> &data, Int size)
{
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

	if	(format.bits == 16) ex_src_short_to_float_array((short *) (UnsignedByte *) data, src_data.data_in, src_data.input_frames * format.channels);
	else if (format.bits == 32) ex_src_int_to_float_array((int *) (UnsignedByte *) data, src_data.data_in, src_data.input_frames * format.channels);

	ex_src_process(state, &src_data);

	data.Resize(src_data.output_frames_gen * (format.bits / 8) * format.channels);

	if	(format.bits == 16) ex_src_float_to_short_array(src_data.data_out, (short *) (UnsignedByte *) data, src_data.output_frames_gen * format.channels);
	else if (format.bits == 32) ex_src_float_to_int_array(src_data.data_out, (int *) (UnsignedByte *) data, src_data.output_frames_gen * format.channels);

	return data.Size();
}

Int BoCA::ResampleDSP::Flush(Buffer<UnsignedByte> &data)
{
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

	ex_src_process(state, &src_data);

	data.Resize(src_data.output_frames_gen * (format.bits / 8) * format.channels);

	if	(format.bits == 16) ex_src_float_to_short_array(src_data.data_out, (short *) (UnsignedByte *) data, src_data.output_frames_gen * format.channels);
	else if (format.bits == 32) ex_src_float_to_int_array(src_data.data_out, (int *) (UnsignedByte *) data, src_data.output_frames_gen * format.channels);

	return data.Size();
}

ConfigLayer *BoCA::ResampleDSP::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureResample();

	return configLayer;
}
