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
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(LAMEOut)

namespace BoCA
{
	class LAMEOut : public CS::EncoderComponent
	{
		private:
			lame_global_flags	*lameFlags;

			Buffer<unsigned char>	 outBuffer;
			Buffer<signed short>	 samplesBuffer;
		public:
			static const String	&GetComponentSpecs();

						 LAMEOut();
						~LAMEOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(LAMEOut)

BoCA_END_COMPONENT(LAMEOut)
