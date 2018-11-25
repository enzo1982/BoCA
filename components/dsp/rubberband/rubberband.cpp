 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

#include "rubberband.h"
#include "config.h"

const String &BoCA::DSPRubberBand::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (rbdll != NIL)
	{
		I18n	*i18n = I18n::Get();

		i18n->SetContext("Components::DSP");

		componentSpecs = String("									\
														\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>							\
		  <component>											\
		    <name>").Append(i18n->TranslateString("Rubber Band Tempo/Pitch Changer")).Append("</name>	\
		    <version>1.0</version>									\
		    <id>rubberband-dsp</id>									\
		    <type>dsp</type>										\
		    <input float=\"true\"/>									\
		  </component>											\
														\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadRubberBandDLL();
}

Void smooth::DetachDLL()
{
	FreeRubberBandDLL();
}

BoCA::DSPRubberBand::DSPRubberBand()
{
	state	    = NIL;

	configLayer = NIL;
}

BoCA::DSPRubberBand::~DSPRubberBand()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DSPRubberBand::Activate()
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	Float64	 timeStretch = config->GetIntValue(ConfigureRubberBand::ConfigID, "Stretch", 1000) / 1000.0;
	Float64	 pitchScale  = config->GetIntValue(ConfigureRubberBand::ConfigID, "Pitch", 1000) / 1000.0;

	Int	 detector    = config->GetIntValue(ConfigureRubberBand::ConfigID, "Detector", 0);
	Int	 transients  = config->GetIntValue(ConfigureRubberBand::ConfigID, "Transients", 0);
	Int	 window	     = config->GetIntValue(ConfigureRubberBand::ConfigID, "Window", 0);
	Int	 phase	     = config->GetIntValue(ConfigureRubberBand::ConfigID, "Phase", 0);
	Int	 formant     = config->GetIntValue(ConfigureRubberBand::ConfigID, "Formant", 0);
	Int	 pitchmode   = config->GetIntValue(ConfigureRubberBand::ConfigID, "PitchMode", 0);
	Int	 channels    = config->GetIntValue(ConfigureRubberBand::ConfigID, "Channels", 0);

	Bool	 smoothing   = config->GetIntValue(ConfigureRubberBand::ConfigID, "Smoothing", False);

	/* Configure and init Rubber Band state.
	 */
	RubberBandOptions	 options = RubberBandOptionProcessRealTime;

	options |= (detector   == 0 ? RubberBandOptionDetectorCompound : (detector   == 1 ? RubberBandOptionDetectorPercussive : RubberBandOptionDetectorSoft	     ));
	options |= (transients == 0 ? RubberBandOptionTransientsCrisp  : (transients == 1 ? RubberBandOptionTransientsMixed    : RubberBandOptionTransientsSmooth    ));
	options |= (window     == 0 ? RubberBandOptionWindowStandard   : (window     == 1 ? RubberBandOptionWindowShort	       : RubberBandOptionWindowLong	     ));
	options |= (smoothing  == 0 ? RubberBandOptionSmoothingOff     :		    RubberBandOptionSmoothingOn						      );
	options |= (phase      == 0 ? RubberBandOptionPhaseLaminar     :		    RubberBandOptionPhaseIndependent					      );
	options |= (formant    == 0 ? RubberBandOptionFormantShifted   :		    RubberBandOptionFormantPreserved					      );
	options |= (pitchmode  == 0 ? RubberBandOptionPitchHighQuality : (pitchmode  == 1 ? RubberBandOptionPitchHighSpeed     : RubberBandOptionPitchHighConsistency));
	options |= (channels   == 0 ? RubberBandOptionChannelsApart    :		    RubberBandOptionChannelsTogether					      );

	state = ex_rubberband_new(format.rate, format.channels, options, timeStretch, pitchScale);

	return True;
}

Bool BoCA::DSPRubberBand::Deactivate()
{
	ex_rubberband_delete(state);

	state = NIL;

	return True;
}

Int BoCA::DSPRubberBand::TransformData(Buffer<UnsignedByte> &data)
{
	/* Append data to samples buffer.
	 */
	samplesBuffer.Resize(samplesBuffer.Size() + data.Size() / 4);

	memcpy(samplesBuffer + samplesBuffer.Size() - data.Size() / 4, data, data.Size());

	data.Resize(0);

	/* Loop as long as enough samples are available.
	 */
	UnsignedInt	 samplesProcessed = 0;

	while (samplesBuffer.Size() / format.channels - samplesProcessed >= ex_rubberband_get_samples_required(state))
	{
		/* Process samples.
		 */
		Int	 samplesToProcess = ex_rubberband_get_samples_required(state);
		float  **samples	  = new float * [format.channels];

		for (Int i = 0; i < format.channels; i++)
		{
			samples[i] = new float [samplesToProcess];

			for (Int n = 0; n < samplesToProcess; n++) samples[i][n] = samplesBuffer[samplesProcessed * format.channels + n * format.channels + i];
		}

		ex_rubberband_process(state, samples, samplesToProcess, false);

		for (Int i = 0; i < format.channels; i++) delete [] samples[i];

		delete [] samples;

		samplesProcessed += samplesToProcess;

		/* Retrieve available data.
		 */
		if (ex_rubberband_available(state) > 0)
		{
			Int	 samplesToRead = ex_rubberband_available(state);
			float  **samples       = new float * [format.channels];

			for (Int i = 0; i < format.channels; i++) samples[i] = new float [samplesToRead];

			ex_rubberband_retrieve(state, samples, samplesToRead);

			data.Resize(data.Size() + samplesToRead * format.channels * 4);

			for (Int i = 0; i < format.channels; i++)
			{
				for (Int n = 0; n < samplesToRead; n++) ((float *) (UnsignedByte *) (data + data.Size() - samplesToRead * format.channels * 4))[n * format.channels + i] = samples[i][n];

				delete [] samples[i];
			}

			delete [] samples;
		}
	}

	/* Adjust samples buffer.
	 */
	Int	 samplesLeft = samplesBuffer.Size() - samplesProcessed * format.channels;

	memmove(samplesBuffer, samplesBuffer + samplesProcessed * format.channels, samplesLeft * 4);

	samplesBuffer.Resize(samplesLeft);

	return data.Size();
}

Int BoCA::DSPRubberBand::Flush(Buffer<UnsignedByte> &data)
{
	/* Process remaining samples.
	 */
	Int	 samplesToProcess = samplesBuffer.Size() / format.channels;
	float  **samples	  = new float * [format.channels];

	for (Int i = 0; i < format.channels; i++)
	{
		samples[i] = new float [samplesToProcess];

		for (Int n = 0; n < samplesToProcess; n++) samples[i][n] = samplesBuffer[n * format.channels + i];
	}

	ex_rubberband_process(state, samples, samplesToProcess, true);

	for (Int i = 0; i < format.channels; i++) delete [] samples[i];

	delete [] samples;

	/* Retrieve available data.
	 */
	if (ex_rubberband_available(state) > 0)
	{
		Int	 samplesToRead = ex_rubberband_available(state);
		float  **samples       = new float * [format.channels];

		for (Int i = 0; i < format.channels; i++) samples[i] = new float [samplesToRead];

		ex_rubberband_retrieve(state, samples, samplesToRead);

		data.Resize(samplesToRead * format.channels * 4);

		for (Int i = 0; i < format.channels; i++)
		{
			for (Int n = 0; n < samplesToRead; n++) ((float *) (UnsignedByte *) data)[n * format.channels + i] = samples[i][n];

			delete [] samples[i];
		}

		delete [] samples;
	}

	samplesBuffer.Resize(0);

	return data.Size();
}

ConfigLayer *BoCA::DSPRubberBand::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureRubberBand();

	return configLayer;
}
