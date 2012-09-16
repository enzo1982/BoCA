 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

#include "samplerate/samplerate.h"

BoCA_BEGIN_COMPONENT(ResampleDSP)

namespace BoCA
{
	class ResampleDSP : public CS::DSPComponent
	{
		private:
			ConfigLayer		*configLayer;

			Float			 ratio;
			SRC_STATE		*state;

			Buffer<float>		 inBuffer;
			Buffer<float>		 outBuffer;
		public:
			static const String	&GetComponentSpecs();

						 ResampleDSP();
						~ResampleDSP();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 TransformData(Buffer<UnsignedByte> &, Int);

			Int			 Flush(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DSP_COMPONENT(ResampleDSP)

BoCA_END_COMPONENT(ResampleDSP)
