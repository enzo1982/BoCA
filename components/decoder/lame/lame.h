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

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(DecoderLAME)

namespace BoCA
{
	class DecoderLAME : public CS::DecoderComponent
	{
		private:
			hip_t			 context;

			Buffer<short>		 pcm_l;
			Buffer<short>		 pcm_r;

			Int			 delaySamples;
			Int			 padSamples;

			Int			 delaySamplesLeft;

			Bool			 SkipID3v2Tag(IO::InStream &);
			Bool			 ParseVBRHeaders(IO::InStream &);
		public:
			static const String	&GetComponentSpecs();

						 DecoderLAME();
						~DecoderLAME();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderLAME)

BoCA_END_COMPONENT(DecoderLAME)
