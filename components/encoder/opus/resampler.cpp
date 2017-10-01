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

#include "resampler.h"

BoCA::Resampler::Resampler(const Config *config, const Track &track, Int sampleRate)
{
	process	   = False;
	flush	   = False;
	quit	   = False;

	errorState = False;

	threadMain.Connect(&Resampler::Run, this);

	/* Create and init resampler component.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	resampler = (AS::DSPComponent *) boca.CreateComponentByID("resample-dsp");

	if (resampler == NIL)
	{
		errorString = "Could not create resampler component!";
		errorState  = True;

		return;
	}

	resamplerConfig = Config::Copy(config);
	resamplerConfig->SetIntValue("Resample", "Converter", 2);
	resamplerConfig->SetIntValue("Resample", "Samplerate", sampleRate);

	resampler->SetConfiguration(resamplerConfig);
	resampler->SetAudioTrackInfo(track);
	resampler->Activate();

	if (resampler->GetErrorState() == True)
	{
		errorString = resampler->GetErrorString();
		errorState  = resampler->GetErrorState();

		boca.DeleteComponent(resampler);

		Config::Free(resamplerConfig);

		return;
	}
}

BoCA::Resampler::~Resampler()
{
	/* Clean up resampler component.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	resampler->Deactivate();

	boca.DeleteComponent(resampler);

	Config::Free(resamplerConfig);
}

Int BoCA::Resampler::Run()
{
	while (!quit)
	{
		while (!quit && !process) S::System::System::Sleep(1);

		Threads::Lock	 lock(resamplerMutex);

		resampler->TransformData(samplesBuffer);

		if (flush)
		{
			Buffer<UnsignedByte>	 remaining;

			resampler->Flush(remaining);

			samplesBuffer.Resize(samplesBuffer.Size() + remaining.Size());

			memcpy(samplesBuffer + samplesBuffer.Size() - remaining.Size(), remaining, remaining.Size());
		}

		process	= False;
	}

	return Success();
}

Void BoCA::Resampler::Resample(const Buffer<UnsignedByte> &buffer)
{
	Threads::Lock	 lock(resamplerMutex);

	samplesBuffer.Resize(buffer.Size());

	memcpy(samplesBuffer, buffer, buffer.Size());

	process = True;
}

Void BoCA::Resampler::Flush()
{
	Threads::Lock	 lock(resamplerMutex);

	samplesBuffer.Resize(0);

	flush	= True;
	process = True;
}

Int BoCA::Resampler::Quit()
{
	quit = True;

	return Success();
}
