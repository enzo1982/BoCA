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

#include "twinvq/bfile.h"

BoCA_BEGIN_COMPONENT(DecoderTwinVQ)

namespace BoCA
{
	class DecoderTwinVQ : public CS::DecoderComponent
	{
		private:
			headerInfo		 setupInfo;
			INDEX			 index;

			BFILE			*bfp;

			Buffer<float>		 frame;
			Int			 frameSize;
		public:
			static const String	&GetComponentSpecs();

						 DecoderTwinVQ();
						~DecoderTwinVQ();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderTwinVQ)

BoCA_END_COMPONENT(DecoderTwinVQ)
