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

BoCA_BEGIN_COMPONENT(DecoderOpus)

namespace BoCA
{
	class DecoderOpus : public CS::DecoderComponent
	{
		private:
			ogg_sync_state		 oy;
			ogg_stream_state	 os;
			ogg_page		 og;
			ogg_packet		 op;

			OpusDecoder		*decoder;

			Int			 sampleRate;

			Int			 preSkip;
			Int			 preSkipLeft;

			Int			 skipSamples;
		public:
			static const String	&GetComponentSpecs();

						 DecoderOpus();
						~DecoderOpus();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Bool			 Seek(Int64);

			Int			 ReadData(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderOpus)

BoCA_END_COMPONENT(DecoderOpus)
