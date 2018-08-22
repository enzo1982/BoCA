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
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(EncoderLAME)

namespace BoCA
{
	class EncoderLAME : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;
			Config			*config;

			lame_t			 context;

			Int			 dataOffset;

			Buffer<unsigned char>	 outBuffer;

			static Bool		 ConvertArguments(Config *);
		public:
			static const String	&GetComponentSpecs();

						 EncoderLAME();
						~EncoderLAME();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderLAME)

BoCA_END_COMPONENT(EncoderLAME)
