 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

#include "rnnoise.h"
#include "config.h"

const String &BoCA::DSPRNNoise::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (rnnoisedll != NIL)
	{
		I18n	*i18n = I18n::Get();

		i18n->SetContext("Components::DSP");

		componentSpecs = String("								\
													\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
		  <component>										\
		    <name>").Append(i18n->TranslateString("RNNoise Noise Reduction")).Append("</name>	\
		    <version>1.0</version>								\
		    <id>rnnoise-dsp</id>								\
		    <type>dsp</type>									\
		    <input bits=\"16\" rate=\"48000\"/>							\
		  </component>										\
													\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadRNNoiseDLL();
}

Void smooth::DetachDLL()
{
	FreeRNNoiseDLL();
}

namespace BoCA
{
	/* Constants.
	 */
	static const Int	 frameSize = 480;
	static const String	 uncPrefix = "\\\\?\\";

	/* Models.
	 */
	static const char	*models[2][3] = { { "mp.rnnn", "lq.rnnn", NIL }, { "cb.rnnn", "bd.rnnn", "sh.rnnn" } };
};

BoCA::DSPRNNoise::DSPRNNoise()
{
	configLayer = NIL;

	model	    = NIL;
}

BoCA::DSPRNNoise::~DSPRNNoise()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DSPRNNoise::Activate()
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	Int	 signal = config->GetIntValue(ConfigureRNNoise::ConfigID, "SignalType", 2);
	Int	 noise	= config->GetIntValue(ConfigureRNNoise::ConfigID, "NoiseType", 0);

	/* Create model.
	 */
	if (models[noise][signal] != NIL)
	{
		String	 modelFileName = Utilities::GetBoCADirectory().Append("boca.dsp.rnnoise/").Append(models[noise][signal]);

#if defined __WIN32__
		String	 uncPath       = String(modelFileName.StartsWith("\\\\") ? "" : uncPrefix).Append(modelFileName);
		FILE	*modelFile     = _wfopen(uncPath, L"rbN");
#else
		FILE	*modelFile     = fopen(modelFileName.ConvertTo("UTF-8"), "rb");
#endif

		model = ex_rnnoise_model_from_file(modelFile);

		fclose(modelFile);
	}

	/* Create RNNoise states.
	 */
	for (Int c = 0; c < format.channels; c++) states.Add(ex_rnnoise_create(model));

	return True;
}

Bool BoCA::DSPRNNoise::Deactivate()
{
	/* Destroy RNNoise states.
	 */
	for (Int c = 0; c < format.channels; c++) ex_rnnoise_destroy(states.GetNth(c));

	states.RemoveAll();

	/* Free model.
	 */
	if (model != NIL)
	{
		ex_rnnoise_model_free(model);

		model = NIL;
	}

	return True;
}

Int BoCA::DSPRNNoise::TransformData(Buffer<UnsignedByte> &data)
{
	/* Append data to samples buffer.
	 */
	samples.Resize(samples.Size() + data.Size() / 2);

	memcpy(samples + samples.Size() - data.Size() / 2, data, data.Size());

	/* Process frames.
	 */
	for (Int i = 0; i < samples.Size() / format.channels / frameSize; i++)
	{
		for (Int c = 0; c < format.channels; c++)
		{
			float	 frame[frameSize];

			for (Int n = 0; n < frameSize; n++) frame[n] = samples[i * frameSize * format.channels + n * format.channels + c];

			ex_rnnoise_process_frame(states.GetNth(c), frame, frame);

			for (Int n = 0; n < frameSize; n++) samples[i * frameSize * format.channels + n * format.channels + c] = frame[n];
		}
	}

	/* Copy processed frames to data buffer.
	 */
	Int	 samplesLeft = samples.Size() % (frameSize * format.channels);

	data.Resize((samples.Size() - samplesLeft) * 2);

	memcpy(data, samples, data.Size());

	/* Keep remaining samples.
	 */
	memmove(samples, samples + samples.Size() - samplesLeft, samplesLeft * 2);

	samples.Resize(samplesLeft);

	return data.Size();
}

Int BoCA::DSPRNNoise::Flush(Buffer<UnsignedByte> &data)
{
	/* Copy remaining samples to data buffer.
	 */
	Int	 numSamples = samples.Size();

	data.Resize(frameSize * format.channels * 2);
	data.Zero();

	memcpy(data, samples, numSamples * 2);

	samples.Resize(0);

	/* Transform samples and cut data buffer at actual end of audio.
	 */
	TransformData(data);

	data.Resize(numSamples * 2);

	return data.Size();
}

ConfigLayer *BoCA::DSPRNNoise::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureRNNoise();

	return configLayer;
}
