 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "lame/lame.h"

BoCA_BEGIN_COMPONENT(LAMEIn)

namespace BoCA
{
	class LAMEIn : public CS::DecoderComponent
	{
		private:
			hip_t			 context;

			Buffer<short>		 pcm_l;
			Buffer<short>		 pcm_r;

			Int			 delaySamples;
			Int			 padSamples;

			Int			 delaySamplesLeft;

			Bool			 SkipID3v2Tag(IO::InStream *);
			Bool			 ParseVBRHeaders(IO::InStream *);
		public:
			static const String	&GetComponentSpecs();

						 LAMEIn();
						~LAMEIn();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &, Int);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(LAMEIn)

BoCA_END_COMPONENT(LAMEIn)
