 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/formatconverter.h>
#include <boca/application/dspcomponent.h>
#include <boca/application/registry.h>

namespace BoCA
{
	class FormatConverterData
	{
		public:
						 FormatConverterData(Config *config) : processSignal(1), readySignal(1)	{ this->config = Config::Copy(config); }
						~FormatConverterData()							{ Config::Free(config); }

			Config			*config;

			Threads::Semaphore	 processSignal;
			Threads::Semaphore	 readySignal;
	};
}

BoCA::FormatConverter::FormatConverter(const Format &source, const Format &target)
{
	converterData	= NIL;

	errorState	= False;
	errorString	= "Unknown error";

	finish		= False;
	process		= False;

	if (source == target || source == Format() || target == Format()) return;

	/* Create dummy track and converter data.
	 */
	Track	 converterTrack;

	converterTrack.SetFormat(source);
	converterData = new FormatConverterData(Config::Get());
	converterData->processSignal.Wait();

	/* Create converter components.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	if (source.channels != target.channels)
	{
		/* Create and init channel converter.
		 */
		AS::DSPComponent	*channels = (AS::DSPComponent *) boca.CreateComponentByID("channels-dsp");

		if (channels == NIL)
		{
			errorState  = True;
			errorString = "Unable to create channel converter component.";

			return;
		}

		converterData->config->SetIntValue("Channels", "Channels", target.channels);
		converterData->config->SetIntValue("Channels", "SwapChannels", False);

		channels->SetConfiguration(converterData->config);
		channels->SetAudioTrackInfo(converterTrack);
		channels->Activate();

		if (channels->GetErrorState())
		{
			errorState  = True;
			errorString = channels->GetErrorString();

			boca.DeleteComponent(channels);

			return;
		}

		converterTrack.SetFormat(channels->GetFormatInfo());

		converters.Add(channels);
	}

	if (source.rate != target.rate)
	{
		/* Create and init resampler.
		 */
		AS::DSPComponent	*resampler = (AS::DSPComponent *) boca.CreateComponentByID("resample-dsp");

		if (resampler == NIL)
		{
			errorState  = True;
			errorString = "Unable to create resampler component.";

			return;
		}

		converterData->config->SetIntValue("Resample", "Converter", 2);
		converterData->config->SetIntValue("Resample", "Samplerate", target.rate);

		resampler->SetConfiguration(converterData->config);
		resampler->SetAudioTrackInfo(converterTrack);
		resampler->Activate();

		if (resampler->GetErrorState())
		{
			errorState  = True;
			errorString = resampler->GetErrorString();

			boca.DeleteComponent(resampler);

			return;
		}

		converterTrack.SetFormat(resampler->GetFormatInfo());

		converters.Add(resampler);
	}

	if (converterTrack.GetFormat() != target)
	{
		/* Create and init format converter.
		 */
		AS::DSPComponent	*converter = (AS::DSPComponent *) boca.CreateComponentByID("format-dsp");

		if (converter == NIL)
		{
			errorState  = True;
			errorString = "Unable to create sample format converter component.";

			return;
		}

		converterData->config->SetIntValue("Format", "Bits", target.bits);
		converterData->config->SetIntValue("Format", "Signed", target.sign);
		converterData->config->SetIntValue("Format", "Float", target.fp);

		converter->SetConfiguration(converterData->config);
		converter->SetAudioTrackInfo(converterTrack);
		converter->Activate();

		if (converter->GetErrorState())
		{
			errorState  = True;
			errorString = converter->GetErrorString();

			boca.DeleteComponent(converter);

			return;
		}

		converterTrack.SetFormat(converter->GetFormatInfo());

		converters.Add(converter);
	}

	/* Start converter thread.
	 */
	converterThread.threadMain.Connect(&FormatConverter::ConverterThread, this);
	converterThread.Start();
}

BoCA::FormatConverter::~FormatConverter()
{
	if (converterData == NIL) return;

	/* Wait for converter thread to finish.
	 */
	finish = True;

	converterData->processSignal.Release();

	converterThread.Wait();

	/* Free converter components.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	foreach (AS::DSPComponent *converter, converters)
	{
		converter->Deactivate();

		boca.DeleteComponent(converter);
	}

	converters.RemoveAll();

	/* Free converter data.
	 */
	delete converterData;
}

Int BoCA::FormatConverter::Transform(Buffer<UnsignedByte> &buffer)
{
	if (converterData == NIL) return buffer.Size();

	/* Move new data out of the way.
	 */
	backBuffer.Resize(buffer.Size());

	if (buffer.Size() > 0)
	{
		memcpy(backBuffer, buffer, buffer.Size());

		buffer.Resize(0);
	}

	/* Aquire lock to handle data.
	 */
	converterData->readySignal.Wait();

	/* Query data from converter thread.
	 */
	buffer.Resize(samplesBuffer.Size());

	if (samplesBuffer.Size() > 0) memcpy(buffer, samplesBuffer, samplesBuffer.Size());

	/* Pass new samples to converter thread.
	 */
	samplesBuffer.Resize(backBuffer.Size());

	if (backBuffer.Size() > 0)
	{
		memcpy(samplesBuffer, backBuffer, backBuffer.Size());

		backBuffer.Resize(0);
	}

	/* Signal availability of new samples.
	 */
	converterData->processSignal.Release();

	return buffer.Size();
}

Int BoCA::FormatConverter::Finish(Buffer<UnsignedByte> &buffer)
{
	if (buffer.Size() != 0) return -1;

	if (converterData == NIL) return buffer.Size();

	/* Aquire lock to handle data.
	 */
	converterData->readySignal.Wait();

	/* Query data from converter thread.
	 */
	buffer.Resize(samplesBuffer.Size());

	if (samplesBuffer.Size() > 0)
	{
		memcpy(buffer, samplesBuffer, samplesBuffer.Size());

		samplesBuffer.Resize(0);
	}

	/* Flush all converter components.
	 */
	foreach (AS::DSPComponent *converter, converters)
	{
		/* Transform data already in return buffer.
		 */
		if (backBuffer.Size() != 0) converter->TransformData(backBuffer);

		/* Flush converter component.
		 */
		Buffer<UnsignedByte>	 flush;

		converter->Flush(flush);

		/* Append remaining data to return buffer.
		 */
		if (flush.Size() > 0)
		{
			backBuffer.Resize(backBuffer.Size() + flush.Size());

			memcpy(backBuffer + backBuffer.Size() - flush.Size(), flush, flush.Size());
		}
	}

	/* Append remaining data in case there is any.
	 */
	if (backBuffer.Size() > 0)
	{
		buffer.Resize(buffer.Size() + backBuffer.Size());

		memcpy(buffer + buffer.Size() - backBuffer.Size(), backBuffer, backBuffer.Size());

		backBuffer.Resize(0);
	}

	return buffer.Size();
}

Int BoCA::FormatConverter::ConverterThread()
{
	while (!finish)
	{
		converterData->processSignal.Wait();

		if (finish) break;

		/* Call transform for every converter component.
		 */
		foreach (AS::DSPComponent *converter, converters) converter->TransformData(samplesBuffer);

		converterData->readySignal.Release();
	}

	return Success();
}

Bool BoCA::FormatConverter::GetErrorState() const
{
	return errorState;
}

const String &BoCA::FormatConverter::GetErrorString() const
{
	return errorString;
}

BoCA::Format BoCA::FormatConverter::GetBestTargetFormat(const Format &source, const AS::Component *component)
{
	if (component->GetInputSpecs().Length() == 0) return source;

	/* Weights for format changes.
	 */
	const Int32	 weightSignedness   = 0x00000001;
	const Int32	 weightMoreBits	    = 0x00000002;
	const Int32	 weightUpsample	    = 0x00000004;
	const Int32	 weightNumberFormat = 0x00000008;
	const Int32	 weightDownsample   = 0x00000010;
	const Int32	 weightLessBits	    = 0x00000020;
	const Int32	 weightUpmix	    = 0x00000040;
	const Int32	 weightDownmix	    = 0x00000080;

	/* Calculate weight for each possible input format.
	 */
	const Array<AS::InputSpec *>	&inputs = component->GetInputSpecs();
	Array<Int32>			 weights;

	foreach (const AS::InputSpec *input, inputs)
	{
		Int32	 weight = 0;

		if (source.sign != input->GetSigned()) weight += weightSignedness;
		if (source.fp	!= input->GetFloat())  weight += weightNumberFormat;

		/* Find best bit resolution.
		 */
		Int	 bestBits = FindBestValue(source.bits, input->GetBits());

		if	(bestBits > source.bits) weight += weightMoreBits;
		else if (bestBits < source.bits) weight += weightLessBits;

		/* Find best sample rate.
		 */
		Int	 bestRate = FindBestValue(source.rate, input->GetRate());

		if	(bestRate > source.rate) weight += weightUpsample;
		else if (bestRate < source.rate) weight += weightDownsample;

		/* Find best number of channels.
		 */
		Int	 bestChannels = FindBestValue(source.channels, input->GetChannels());

		if	(bestChannels > source.channels) weight += weightUpmix;
		else if (bestChannels < source.channels) weight += weightDownmix;

		weights.Add(weight);
	}

	/* Find input format with lowest weight.
	 */
	Int	 bestInput = 0;

	for (Int i = 0; i < weights.Length(); i++) if (weights.GetNth(i) < weights.GetNth(bestInput)) bestInput = i;

	/* Compute target format.
	 */
	Format			 target = source;
	const AS::InputSpec	*input	= inputs.GetNth(bestInput);

	target.sign	= input->GetSigned();
	target.fp	= input->GetFloat();

	target.bits	= FindBestValue(source.bits, input->GetBits());
	target.rate	= FindBestValue(source.rate, input->GetRate());
	target.channels = FindBestValue(source.channels, input->GetChannels());

	return target;
}

Int BoCA::FormatConverter::FindBestValue(Int reference, const String &spec)
{
	/* Spec format is x1[-y1][,xn[-yn]]* e.g. 1-2,6,8 for a component that supports 1,2,6 or 8 channels.
	 */
	const Array<String>	&ranges	   = spec.Explode(",");
	Int			 bestValue = -1;

	foreach (const String &range, ranges)
	{
		Int	 low  = range.ToInt();
		Int	 high = range.Tail(range.Length() - range.Find("-") - 1).ToInt();

		if (low <= reference && high >= reference) { bestValue = reference; break; }

		if (low  > reference && (bestValue > low || bestValue < reference)) bestValue = low;
		if (high < reference &&  bestValue < high			  ) bestValue = high;
	}

	return bestValue;
}
