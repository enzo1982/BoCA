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

BoCA_BEGIN_COMPONENT(TwinVQOut)

namespace BoCA
{
	class TwinVQOut : public CS::EncoderComponent
	{
		private:
			headerInfo		 setupInfo;
			encSpecificInfo		 encInfo;
			INDEX			 index;

			Buffer<unsigned char>	 outBuffer;
			Buffer<signed short>	 samplesBuffer;
			Buffer<float>		 frame;
		public:
			static const String	&GetComponentSpecs();

						 TwinVQOut();
						~TwinVQOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(TwinVQOut)

BoCA_END_COMPONENT(TwinVQOut)
