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

#include <boca/common/formatconverter.h>
#include <boca/application/dspcomponent.h>
#include <boca/application/registry.h>

BoCA::FormatConverter::FormatConverter(const Format &source, const Format &target)
{
	converterConfig = NIL;

	errorState	= False;
	errorString	= "Unknown error";

	if (source == target) return;

	/* Create dummy track and converter config.
	 */
	Track	 converterTrack;

	converterTrack.SetFormat(source);
	converterConfig = Config::Copy(Config::Get());

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

		converterConfig->SetIntValue("Channels", "Channels", target.channels);

		channels->SetConfiguration(converterConfig);
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

		converterConfig->SetIntValue("Resample", "Converter", 2);
		converterConfig->SetIntValue("Resample", "Samplerate", target.rate);

		resampler->SetConfiguration(converterConfig);
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

		converterConfig->SetIntValue("Format", "Bits", target.bits);
		converterConfig->SetIntValue("Format", "Signed", target.sign);
		converterConfig->SetIntValue("Format", "Float", target.fp);

		converter->SetConfiguration(converterConfig);
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
}

BoCA::FormatConverter::~FormatConverter()
{
	/* Free converter components.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	foreach (AS::DSPComponent *converter, converters)
	{
		converter->Deactivate();

		boca.DeleteComponent(converter);
	}

	converters.RemoveAll();

	/* Free converter config.
	 */
	if (converterConfig != NIL) Config::Free(converterConfig);
}

Int BoCA::FormatConverter::Transform(Buffer<UnsignedByte> &buffer)
{
	/* Call transform for every converter component.
	 */
	foreach (AS::DSPComponent *converter, converters) converter->TransformData(buffer);

	return buffer.Size();
}

Int BoCA::FormatConverter::Finish(Buffer<UnsignedByte> &buffer)
{
	if (buffer.Size() != 0) return -1;

	foreach (AS::DSPComponent *converter, converters)
	{
		/* Transform data already in return buffer.
		 */
		if (buffer.Size() != 0) converter->TransformData(buffer);

		/* Flush converter component.
		 */
		Buffer<UnsignedByte>	 flush;

		converter->Flush(flush);

		/* Append remaining data to return buffer.
		 */
		buffer.Resize(buffer.Size() + flush.Size());

		memcpy(buffer + buffer.Size() - flush.Size(), flush, flush.Size());
	}

	return buffer.Size();
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

	String::ExplodeFinish();

	return bestValue;
}
