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

#include "channels.h"
#include "config.h"

const String &BoCA::DSPChannels::GetComponentSpecs()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Components::DSP");

	static String	 componentSpecs = String("						\
												\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>					\
		  <component>									\
		    <name>").Append(i18n->TranslateString("Channel Converter")).Append("</name>	\
		    <version>1.0</version>							\
		    <id>channels-dsp</id>							\
		    <type>dsp</type>								\
		    <input float=\"true\"/>							\
		  </component>									\
												\
		");

	return componentSpecs;
}

BoCA::DSPChannels::DSPChannels()
{
	configLayer  = NIL;

	swapChannels = False;
}

BoCA::DSPChannels::~DSPChannels()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DSPChannels::Activate()
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	Int	 channels = config->GetIntValue(ConfigureChannels::ConfigID, "Channels", 2);

	swapChannels = config->GetIntValue(ConfigureChannels::ConfigID, "SwapChannels", False);

	/* Check output format.
	 *
	 *   Supported: 7.1 => 5.1 => 2.1 => Stereo => Mono
	 *		7.1 => 5.1 => 4.0 => Stereo => Mono
	 *		Mono => Stereo
	 */
	if (!(format.channels == 8 && (channels == 6 || channels <= 3)) &&
	    !(format.channels == 6 &&			channels <= 3 ) &&
	    !(format.channels == 3 &&			channels <= 2 ) &&
	    !(format.channels <= 2 &&			channels <= 2 ))
	{
		errorState  = True;
		errorString = String("Requested conversion from ").Append(String::FromInt(format.channels)).Append(" to ").Append(String::FromInt(channels)).Append(" channels is not supported.");

		return False;
	}

	/* Set output channels.
	 */
	this->format.channels = channels;

	return True;
}

Int BoCA::DSPChannels::TransformData(Buffer<UnsignedByte> &data)
{
	/* Check if we need to do anything.
	 */
	const Format	&format = track.GetFormat();

	Int	 source = format.channels;
	Int	 target = this->format.channels;

	if (source != target)
	{
		/* Channel conversion routines.
		 */
		Int	 numSamples = data.Size() / format.channels / (format.bits / 8);
		Float32	*samples    = (Float32 *) (UnsignedByte *) data;

		/* 7.1 source.
		 */
		if (source == 8 && target <= 6)
		{
			/* Convert 7.1 to 5.1.
			 */
			for (Int i = 0; i < numSamples; i++)
			{
				Float32	 fl = samples[i * 8    ], fr  = samples[i * 8 + 1];
				Float32	 fc = samples[i * 8 + 2], lfe = samples[i * 8 + 3];
				Float32	 rl = samples[i * 8 + 4], rr  = samples[i * 8 + 5];
				Float32	 sl = samples[i * 8 + 6], sr  = samples[i * 8 + 7];

				samples[i * 6	 ] = fl;
				samples[i * 6 + 1] = fr;
				samples[i * 6 + 2] = fc;
				samples[i * 6 + 3] = lfe;
				samples[i * 6 + 4] = Math::Min(1.0, Math::Max(-1.0, sl + rl * 0.871));
				samples[i * 6 + 5] = Math::Min(1.0, Math::Max(-1.0, sr + rr * 0.871));
			}

			data.Resize(data.Size() / 4 * 3);

			source = 6;
		}

		/* 5.1 source.
		 */
		if (source == 6 && target <= 2)
		{
			/* Convert 5.1 to Stereo.
			 */
			for (Int i = 0; i < numSamples; i++)
			{
				Float32	 fl = samples[i * 6    ], fr  = samples[i * 6 + 1];
				Float32	 fc = samples[i * 6 + 2], lfe = samples[i * 6 + 3];
				Float32	 rl = samples[i * 6 + 4], rr  = samples[i * 6 + 5];

				samples[i * 2	 ] = Math::Min(1.0, Math::Max(-1.0, fl + (fc + lfe + rl) * 0.708));
				samples[i * 2 + 1] = Math::Min(1.0, Math::Max(-1.0, fr + (fc + lfe + rr) * 0.708));
			}

			data.Resize(data.Size() / 3);

			source = 2;
		}
		else if (source == 6 && target == 3)
		{
			/* Convert 5.1 to 2.1.
			 */
			for (Int i = 0; i < numSamples; i++)
			{
				Float32	 fl = samples[i * 6    ], fr  = samples[i * 6 + 1];
				Float32	 fc = samples[i * 6 + 2], lfe = samples[i * 6 + 3];
				Float32	 rl = samples[i * 6 + 4], rr  = samples[i * 6 + 5];

				samples[i * 3	 ] = Math::Min(1.0, Math::Max(-1.0, fl + (fc + rl) * 0.708));
				samples[i * 3 + 1] = Math::Min(1.0, Math::Max(-1.0, fr + (fc + rr) * 0.708));
				samples[i * 3 + 2] = lfe;
			}

			data.Resize(data.Size() / 2);
		}
		else if (source == 6 && target == 4)
		{
			/* Convert 5.1 to 4.0.
			 */
			for (Int i = 0; i < numSamples; i++)
			{
				Float32	 fl = samples[i * 6    ], fr  = samples[i * 6 + 1];
				Float32	 fc = samples[i * 6 + 2], lfe = samples[i * 6 + 3];
				Float32	 rl = samples[i * 6 + 4], rr  = samples[i * 6 + 5];

				samples[i * 4	 ] = Math::Min(1.0, Math::Max(-1.0, fl + (fc + lfe) * 0.708));
				samples[i * 4 + 1] = Math::Min(1.0, Math::Max(-1.0, fr + (fc + lfe) * 0.708));
				samples[i * 4 + 2] = rl;
				samples[i * 4 + 3] = rr;
			}

			data.Resize(data.Size() / 3 * 2);
		}

		/* 4.0 source.
		 */
		if (source == 4 && target <= 2)
		{
			/* Convert 4.0 to Stereo.
			 */
			for (Int i = 0; i < numSamples; i++)
			{
				Float32	 fl = samples[i * 4    ], fr = samples[i * 4 + 1];
				Float32	 rl = samples[i * 4 + 2], rr = samples[i * 4 + 3];

				samples[i * 2	 ] = Math::Min(1.0, Math::Max(-1.0, fl + rl * 0.708));
				samples[i * 2 + 1] = Math::Min(1.0, Math::Max(-1.0, fr + rr * 0.708));
			}

			data.Resize(data.Size() / 2);

			source = 2;
		}

		/* 2.1 source.
		 */
		if (source == 3 && target <= 2)
		{
			/* Convert 2.1 to Stereo.
			 */
			for (Int i = 0; i < numSamples; i++)
			{
				Float32	 l   = samples[i * 3	];
				Float32	 r   = samples[i * 3 + 1];
				Float32	 lfe = samples[i * 3 + 2];

				samples[i * 2	 ] = Math::Min(1.0, Math::Max(-1.0, l + lfe * 0.708));
				samples[i * 2 + 1] = Math::Min(1.0, Math::Max(-1.0, r + lfe * 0.708));
			}

			data.Resize(data.Size() / 3 * 2);

			source = 2;
		}

		/* Stereo source.
		 */
		if (source == 2 && target == 1)
		{
			/* Convert Stereo to Mono.
			 */
			for (Int i = 0; i < numSamples; i++)
			{
				Float32	 l = samples[i * 2    ];
				Float32	 r = samples[i * 2 + 1];

				samples[i] = Math::Min(1.0, Math::Max(-1.0, (l + r) * 0.5));
			}

			data.Resize(data.Size() / 2);
		}

		/* Mono source.
		 */
		if (source == 1 && target == 2)
		{
			/* Convert Mono to Stereo.
			 */
			data.Resize(data.Size() * 2);

			Float32	*samples = (Float32 *) (UnsignedByte *) data;

			for (Int i = numSamples - 1; i >= 0; i--)
			{
				Float32	 m = samples[i];

				samples[i * 2	 ] = m;
				samples[i * 2 + 1] = m;
			}
		}
	}

	/* Swap Stereo channels if requested.
	 */
	if (swapChannels && source > 1 && target == 2)
	{
		const Channel::Layout	 Mirrored_2_0 = { Channel::FrontRight, Channel::FrontLeft };

		Utilities::ChangeChannelOrder(data, this->format, Mirrored_2_0, Channel::Default_2_0);
	}

	return data.Size();
}

ConfigLayer *BoCA::DSPChannels::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureChannels();

	return configLayer;
}
