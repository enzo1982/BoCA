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

#include "twinvq/bfile_e.h"

BoCA_BEGIN_COMPONENT(EncoderTwinVQ)

namespace BoCA
{
	class EncoderTwinVQ : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			headerInfo		 setupInfo;
			encSpecificInfo		 encInfo;
			INDEX			 index;

			BFILE			*bfp;

			Buffer<signed short>	 samplesBuffer;
			Buffer<float>		 frame;
		public:
			static const String	&GetComponentSpecs();

						 EncoderTwinVQ();
						~EncoderTwinVQ();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderTwinVQ)

BoCA_END_COMPONENT(EncoderTwinVQ)
