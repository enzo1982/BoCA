 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(DSPChannels)

namespace BoCA
{
	class DSPChannels : public CS::DSPComponent
	{
		private:
			ConfigLayer		*configLayer;

			Bool			 swapChannels;
		public:
			static const String	&GetComponentSpecs();

						 DSPChannels();
						~DSPChannels();

			Bool			 Activate();

			Int			 TransformData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DSP_COMPONENT(DSPChannels)

BoCA_END_COMPONENT(DSPChannels)
