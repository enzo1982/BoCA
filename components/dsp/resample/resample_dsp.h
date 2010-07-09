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

#include "samplerate/samplerate.h"

BoCA_BEGIN_COMPONENT(ResampleDSP)

namespace BoCA
{
	class ResampleDSP : public CS::DSPComponent
	{
		private:
			ConfigLayer		*configLayer;

			SRC_STATE		*state;
		public:
			static const String	&GetComponentSpecs();

						 ResampleDSP();
						~ResampleDSP();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 TransformData(Buffer<UnsignedByte> &, Int);

			Int			 Flush(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
			Void			 FreeConfigurationLayer();
	};
};

BoCA_DEFINE_DSP_COMPONENT(ResampleDSP)

BoCA_END_COMPONENT(ResampleDSP)
