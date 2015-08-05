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

BoCA_BEGIN_COMPONENT(EncoderBonk)

namespace BoCA
{
	class EncoderBonk : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			void			*encoder;

			Buffer<signed short>	 samplesBuffer;
			Buffer<unsigned char>	 dataBuffer;
		public:
			static const String	&GetComponentSpecs();

						 EncoderBonk();
						~EncoderBonk();

			Bool			 IsLossless() const;

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderBonk)

BoCA_END_COMPONENT(EncoderBonk)
