 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(DSPRNNoise)

namespace BoCA
{
	class DSPRNNoise : public CS::DSPComponent
	{
		private:
			Array<DenoiseState *>	 states;

			Buffer<Short>		 samples;
		public:
			static const String	&GetComponentSpecs();

						 DSPRNNoise();
						~DSPRNNoise();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 TransformData(Buffer<UnsignedByte> &);

			Int			 Flush(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DSP_COMPONENT(DSPRNNoise)

BoCA_END_COMPONENT(DSPRNNoise)
