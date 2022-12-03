 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(DSPVolume)

namespace BoCA
{
	class DSPVolume : public CS::DSPComponent
	{
		private:
			ConfigLayer		*configLayer;

			Float			 scaleFactor;
		public:
			static const String	&GetComponentSpecs();

						 DSPVolume();
						~DSPVolume();

			Bool			 Activate();

			Int			 TransformData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DSP_COMPONENT(DSPVolume)

BoCA_END_COMPONENT(DSPVolume)
