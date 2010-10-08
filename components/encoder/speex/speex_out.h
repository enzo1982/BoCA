 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(SpeexOut)

namespace BoCA
{
	class SpeexOut : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			ogg_stream_state	 os;
			ogg_page		 og;
			ogg_packet		 op;

			void			*encoder;
			SpeexBits		 bits;

			int			 numPackets;
			int			 frameSize;

			Buffer<char>		 dataBuffer;
			Buffer<spx_int16_t>	 samplesBuffer;

			Int			 WriteOggPackets(Bool);
		public:
			static const String	&GetComponentSpecs();

						 SpeexOut();
						~SpeexOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(SpeexOut)

BoCA_END_COMPONENT(SpeexOut)
