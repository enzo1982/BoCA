 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(OpusOut)

namespace BoCA
{
	class OpusOut : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			AS::DSPComponent	*resampler;

			ogg_stream_state	 os;
			ogg_page		 og;
			ogg_packet		 op;

			OpusEncoder		*encoder;

			Int			 frameSize;

			Int			 numPackets;
			Int			 totalSamples;

			Buffer<unsigned char>	 dataBuffer;
			Buffer<signed short>	 samplesBuffer;

			Int			 EncodeFrames(const Buffer<signed short> &, Buffer<unsigned char> &, Bool);

			Int			 WriteOggPackets(Bool);
		public:
			static const String	&GetComponentSpecs();

						 OpusOut();
						~OpusOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			String			 GetOutputFileExtension();

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(OpusOut)

BoCA_END_COMPONENT(OpusOut)
