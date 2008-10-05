 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
	configLayer = NIL;
}

BoCA::ResampleDSP::~ResampleDSP()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::ResampleDSP::Activate()
{
	int	 error;

	state = ex_src_new(SRC_SINC_BEST_QUALITY, track.GetFormat().channels, &error);

	return True;
}

Bool BoCA::ResampleDSP::Deactivate()
{
	ex_src_delete(state);

	return True;
}

Int BoCA::ResampleDSP::TransformData(Buffer<UnsignedByte> &data, Int size)
{
	SRC_DATA	 src_data;

//	src_data.data_in	= fdata_in;
//	src_data.data_out	= fdata_out;
	src_data.input_frames	= 0;
	src_data.output_frames	= 0;
	src_data.src_ratio	= 2;
	src_data.end_of_input	= 0;

	ex_src_process(state, &src_data);

	return src_data.output_frames_gen;
}

Int BoCA::ResampleDSP::Flush(Buffer<UnsignedByte> &data)
{
	SRC_DATA	 src_data;

//	src_data.data_in	= fdata_in;
//	src_data.data_out	= fdata_out;
	src_data.input_frames	= 0;
	src_data.output_frames	= 0;
	src_data.src_ratio	= 2;
	src_data.end_of_input	= 1;

	ex_src_process(state, &src_data);

	return src_data.output_frames_gen;
}

ConfigLayer *BoCA::ResampleDSP::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureResample();

	return configLayer;
}

Void BoCA::ResampleDSP::FreeConfigurationLayer()
{
	if (configLayer != NIL)
	{
		delete configLayer;

		configLayer = NIL;
	}
}
