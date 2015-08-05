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

BoCA_BEGIN_COMPONENT(DSPResample)

namespace BoCA
{
	class DSPResample : public CS::DSPComponent
	{
		private:
			ConfigLayer		*configLayer;

			Float			 ratio;
			SRC_STATE		*state;

			Buffer<float>		 inBuffer;
			Buffer<float>		 outBuffer;

			Buffer<SignedInt16>	 shortBuffer;
			Buffer<SignedInt32>	 intBuffer;
		public:
			static const String	&GetComponentSpecs();

						 DSPResample();
						~DSPResample();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 TransformData(Buffer<UnsignedByte> &);

			Int			 Flush(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DSP_COMPONENT(DSPResample)

BoCA_END_COMPONENT(DSPResample)
