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

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(EncoderOpus)

namespace BoCA
{
	class EncoderOpus : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			AS::DSPComponent	*resampler;
			Config			*resamplerConfig;

			ogg_stream_state	 os;
			ogg_page		 og;
			ogg_packet		 op;

			OpusMSEncoder		*encoder;

			Int			 frameSize;
			Int			 sampleRate;

			Int			 numPackets;
			Int			 totalSamples;

			Buffer<unsigned char>	 dataBuffer;
			Buffer<signed short>	 samplesBuffer;

			Int			 EncodeFrames(Bool);
			Bool			 FixChapterMarks();

			Int			 WriteOggPackets(Bool);

		public:
			static const String	&GetComponentSpecs();

						 EncoderOpus();
						~EncoderOpus();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderOpus)

BoCA_END_COMPONENT(EncoderOpus)
