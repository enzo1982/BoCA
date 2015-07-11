 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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

			OpusEncoder		*encoder;

			Int			 frameSize;

			Int			 numPackets;
			Int			 totalSamples;

			Buffer<unsigned char>	 dataBuffer;
			Buffer<signed short>	 samplesBuffer;

			Int			 EncodeFrames(Buffer<signed short> &, Buffer<unsigned char> &, Bool);
			Bool			 FixChapterMarks();

			Int			 WriteOggPackets(Bool);
		public:
			static const String	&GetComponentSpecs();

						 EncoderOpus();
						~EncoderOpus();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderOpus)

BoCA_END_COMPONENT(EncoderOpus)
