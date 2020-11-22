 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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

#include "libhdcd/hdcd_simple.h"

BoCA_BEGIN_COMPONENT(DSPHDCD)

namespace BoCA
{
	class DSPHDCD : public CS::DSPComponent
	{
		private:
			hdcd_simple		*context;

			Buffer<int>		 samplesBuffer;

			Bool			 IsHDCDContent();
		public:
			static const String	&GetComponentSpecs();

						 DSPHDCD();
						~DSPHDCD();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 TransformData(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DSP_COMPONENT(DSPHDCD)

BoCA_END_COMPONENT(DSPHDCD)
