 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

BoCA_BEGIN_COMPONENT(WaveOut)

namespace BoCA
{
	class WaveOut : public CS::EncoderComponent
	{
		private:
			Int			 nOfSamples;
		public:
			static const String	&GetComponentSpecs();

						 WaveOut();
						~WaveOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(WaveOut)

BoCA_END_COMPONENT(WaveOut)
