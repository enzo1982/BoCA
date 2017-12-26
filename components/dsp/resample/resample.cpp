 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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
		    <name>Sample Rate Converter</name>		\
		    <version>1.0</version>			\
		    <id>resample-dsp</id>			\
		    <type>dsp</type>				\
		    <input float=\"true\"/>			\
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
	const Config	*config = GetConfiguration();
	const Format	&format = track.GetFormat();

	this->format	  = format;
	this->format.rate = config->GetIntValue(ConfigureResample::ConfigID, "Samplerate", 44100);

	/* Init the resampler only if we actually need to resample.
	 */
	if (this->format.rate != format.rate)
	{
		int	 error;

		ratio = Float(this->format.rate) / Float(format.rate);
		state = ex_src_new(config->GetIntValue(ConfigureResample::ConfigID, "Converter", SRC_SINC_MEDIUM_QUALITY), format.channels, &error);

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

Int BoCA::DSPResample::TransformData(Buffer<UnsignedByte> &data)
{
	if (state == NIL) return data.Size();

	SRC_DATA	 src_data = { 0 };

	src_data.end_of_input	= 0;
	src_data.src_ratio	= ratio;
	src_data.input_frames	= data.Size() / sizeof(float) / format.channels;
	src_data.output_frames	= src_data.input_frames * src_data.src_ratio + 2;

	output.Resize(src_data.output_frames * format.channels);

	src_data.data_in	= (float *) (UnsignedByte *) data;
	src_data.data_out	= output;

	/* Process input and copy to output.
	 */
	ex_src_process(state, &src_data);

	data.Resize(src_data.output_frames_gen * sizeof(float) * format.channels);

	memcpy(data, src_data.data_out, data.Size());

	return data.Size();
}

Int BoCA::DSPResample::Flush(Buffer<UnsignedByte> &data)
{
	if (state == NIL) return 0;

	SRC_DATA	 src_data = { 0 };

	src_data.end_of_input	= 1;
	src_data.src_ratio	= ratio;
	src_data.input_frames	= 0;
	src_data.output_frames	= format.rate;

	output.Resize(src_data.output_frames * format.channels);

	src_data.data_in	= NIL;
	src_data.data_out	= output;

	/* Flush input and copy to output.
	 */
	ex_src_process(state, &src_data);

	data.Resize(src_data.output_frames_gen * sizeof(float) * format.channels);

	memcpy(data, src_data.data_out, data.Size());

	return data.Size();
}

ConfigLayer *BoCA::DSPResample::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureResample();

	return configLayer;
}
