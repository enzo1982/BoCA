 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(EncoderSpeex)

namespace BoCA
{
	class EncoderSpeex : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			ogg_stream_state	 os;
			ogg_page		 og;
			ogg_packet		 op;

			void			*encoder;
			SpeexBits		 bits;

			spx_int32_t		 frameSize;
			spx_int32_t		 lookAhead;

			Int			 numPackets;
			Int			 totalSamples;

			Buffer<unsigned char>	 dataBuffer;
			Buffer<spx_int16_t>	 samplesBuffer;
			Buffer<signed short>	 backBuffer;

			Int			 EncodeFrames(const Buffer<signed short> &, Buffer<unsigned char> &, Bool);

			Int			 WriteOggPackets(Bool);
		public:
			static const String	&GetComponentSpecs();

						 EncoderSpeex();
						~EncoderSpeex();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderSpeex)

BoCA_END_COMPONENT(EncoderSpeex)
