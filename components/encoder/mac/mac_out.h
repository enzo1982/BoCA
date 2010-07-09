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
#include "mac/MACDll.h"

BoCA_BEGIN_COMPONENT(MACOut)

namespace BoCA
{
	class MACOut : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			APE_COMPRESS_HANDLE	 hAPECompress;
		public:
			static const String	&GetComponentSpecs();

						 MACOut();
						~MACOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
			Void			 FreeConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(MACOut)

BoCA_END_COMPONENT(MACOut)
