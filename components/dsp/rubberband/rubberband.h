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

BoCA_BEGIN_COMPONENT(DSPRubberBand)

namespace BoCA
{
	class DSPRubberBand : public CS::DSPComponent
	{
		private:
			ConfigLayer		*configLayer;

			RubberBandState		 state;

			Buffer<float>		 samplesBuffer;
		public:
			static const String	&GetComponentSpecs();

						 DSPRubberBand();
						~DSPRubberBand();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 TransformData(Buffer<UnsignedByte> &);

			Int			 Flush(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DSP_COMPONENT(DSPRubberBand)

BoCA_END_COMPONENT(DSPRubberBand)
