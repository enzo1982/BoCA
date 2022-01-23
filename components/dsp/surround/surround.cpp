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

#include "surround.h"
#include "config.h"

namespace BoCA
{
	namespace Channel
	{
		const Layout	 FreeSurround_2_1 = { FrontLeft, FrontRight, LFE };
		const Layout	 FreeSurround_3_1 = { FrontLeft, FrontCenter, FrontRight, LFE };
		const Layout	 FreeSurround_4_1 = { FrontLeft, FrontRight, RearLeft, RearRight, LFE };
		const Layout	 FreeSurround_5_1 = { FrontLeft, FrontCenter, FrontRight, RearLeft, RearRight, LFE };
		const Layout	 FreeSurround_6_1 = { FrontLeft, FrontCenter, FrontRight, SideLeft, SideRight, RearCenter, LFE };
		const Layout	 FreeSurround_7_1 = { FrontLeft, FrontCenter, FrontRight, SideLeft, SideRight, RearLeft, RearRight, LFE };
	};
};

const String &BoCA::DSPSurround::GetComponentSpecs()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Components::DSP");

	static String	 componentSpecs = String("							\
													\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
		  <component>										\
		    <name>").Append(i18n->TranslateString("Matrix Surround Decoder")).Append("</name>	\
		    <version>1.0</version>								\
		    <id>surround-dsp</id>								\
		    <type>dsp</type>									\
		    <input float=\"true\" channels=\"2\"/>						\
		    <input float=\"true\" channels=\"1,3-256\"/>					\
		    <input float=\"false\" channels=\"1,3-256\"/>					\
		  </component>										\
													\
		");

	return componentSpecs;
}

BoCA::DSPSurround::DSPSurround()
{
	configLayer   = NIL;

	decoder	      = NIL;
	first	      = True;

	blockSize     = 4096;
	channelSetup  = (Int) cs_5point1;
}

BoCA::DSPSurround::~DSPSurround()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DSPSurround::Activate()
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	channelSetup = config->GetIntValue(ConfigureSurround::ConfigID, "ChannelSetup", 6);

	Bool	 redirectBass = config->GetIntValue(ConfigureSurround::ConfigID, "RedirectBass", True);

	switch (channelSetup)
	{
		case 3: channelSetup = (Int) cs_stereo;	 break;
		case 4: channelSetup = (Int) cs_3stereo; break;
		case 5: channelSetup = (Int) cs_4point1; break;
		case 6: channelSetup = (Int) cs_5point1; break;
		case 7: channelSetup = (Int) cs_6point1; break;
		case 8: channelSetup = (Int) cs_7point1; break;
	}

	/* Instantiate FreeSurround decoder.
	 */
	const Format	&format = track.GetFormat();

	Int	 minBlockSize = Math::Pow(Int(2), Int(Math::Floor(Math::Log2(format.rate / 10))));
	Int	 maxBlockSize = Math::Pow(Int(2), Int(Math::Ceil(Math::Log2(format.rate / 10))));

	blockSize = Math::Abs(format.rate / 10 - minBlockSize) < Math::Abs(format.rate / 10 - maxBlockSize) ? minBlockSize : maxBlockSize;

	decoder	  = new freesurround_decoder(channel_setup(channelSetup), blockSize);
	decoder->bass_redirection(redirectBass);

	first	  = True;

	/* Set output channels.
	 */
	if (format.channels == 2) this->format.channels = freesurround_decoder::num_channels(channel_setup(channelSetup));

	return True;
}

Bool BoCA::DSPSurround::Deactivate()
{
	/* Free FreeSurround decoder.
	 */
	delete decoder;

	return True;
}

Int BoCA::DSPSurround::TransformData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	/* Only work on stereo input.
	 */
	if (format.channels != 2) return data.Size();

	/* Copy data to samples buffer.
	 */
	Int	 size = data.Size() / sizeof(Float32);

	samplesBuffer.Resize(samplesBuffer.Size() + size);

	memcpy(samplesBuffer + samplesBuffer.Size() - size, data, data.Size());

	data.Resize(0);

	/* Decode samples.
	 */
	Int	 numInputSamples  = blockSize * format.channels;
	Int	 numOutputSamples = blockSize * this->format.channels;

	while (samplesBuffer.Size() >= numInputSamples)
	{
		Float32	*samples = decoder->decode(samplesBuffer);
		Int	 offset	 = 0;

		if (first)
		{
			numOutputSamples /= 2;

			offset = numOutputSamples;
			first  = False;
		}

		/* Append output samples to data.
		 */
		Int	 numOutputBytes = numOutputSamples * sizeof(Float32);

		data.Resize(data.Size() + numOutputBytes);

		memcpy(data + data.Size() - numOutputBytes, samples + offset, numOutputBytes);

		/* Remove input samples from buffer.
		 */
		memmove(samplesBuffer, samplesBuffer + numInputSamples, (samplesBuffer.Size() - numInputSamples) * sizeof(Float32));

		samplesBuffer.Resize(samplesBuffer.Size() - numInputSamples);
	}

	/* Reorder channels to BoCA default order.
	 */
	if	(channelSetup == (Int) cs_stereo ) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_2_1, Channel::Default_2_1);
	else if	(channelSetup == (Int) cs_3stereo) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_3_1, Channel::Default_3_1);
	else if	(channelSetup == (Int) cs_4point1) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_4_1, Channel::Default_4_1);
	else if	(channelSetup == (Int) cs_5point1) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_5_1, Channel::Default_5_1);
	else if	(channelSetup == (Int) cs_6point1) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_6_1, Channel::Default_6_1);
	else if	(channelSetup == (Int) cs_7point1) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_7_1, Channel::Default_7_1);

	return data.Size();
}

Int BoCA::DSPSurround::Flush(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	/* Only work on stereo input.
	 */
	if (format.channels != 2) return 0;

	/* Prepare samples buffer.
	 */
	Int	 inputSamplesLeft  = samplesBuffer.Size();
	Int	 outputSamplesLeft = (inputSamplesLeft / format.channels + blockSize / 2) * this->format.channels;

	samplesBuffer.Resize(blockSize * format.channels);

	memset(samplesBuffer + inputSamplesLeft, 0, (samplesBuffer.Size() - inputSamplesLeft) * sizeof(Float32));

	/* Pass final samples to decoder.
	 */
	while (outputSamplesLeft)
	{
		Float32	*samples = decoder->decode(samplesBuffer);

		/* Append output samples to data.
		 */
		Int	 numOutputSamples = Math::Min(outputSamplesLeft, blockSize * this->format.channels);
		Int	 numOutputBytes	  = numOutputSamples * sizeof(Float32);

		data.Resize(data.Size() + numOutputBytes);

		memcpy(data + data.Size() - numOutputBytes, samples, numOutputBytes);

		outputSamplesLeft -= numOutputSamples;

		/* Clear input buffer.
		 */
		memset(samplesBuffer, 0, samplesBuffer.Size() * sizeof(Float32));
	}

	/* Reorder channels to BoCA default order.
	 */
	if	(channelSetup == (Int) cs_stereo ) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_2_1, Channel::Default_2_1);
	else if	(channelSetup == (Int) cs_3stereo) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_3_1, Channel::Default_3_1);
	else if	(channelSetup == (Int) cs_4point1) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_4_1, Channel::Default_4_1);
	else if	(channelSetup == (Int) cs_5point1) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_5_1, Channel::Default_5_1);
	else if	(channelSetup == (Int) cs_6point1) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_6_1, Channel::Default_6_1);
	else if	(channelSetup == (Int) cs_7point1) Utilities::ChangeChannelOrder(data, this->format, Channel::FreeSurround_7_1, Channel::Default_7_1);

	return data.Size();
}

ConfigLayer *BoCA::DSPSurround::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureSurround();

	return configLayer;
}
