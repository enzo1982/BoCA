 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(FAACOut)

namespace BoCA
{
	class FAACOut : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			MP4FileHandle		 mp4File;
			faacEncHandle		 handle;
			faacEncConfigurationPtr	 fConfig;

			Int			 mp4Track;
			Int			 sampleId;

			Int			 frameSize;

			Int			 totalSamples;
			Int			 encodedSamples;
			Int			 delaySamples;

			Buffer<unsigned char>	 outBuffer;
			Buffer<int32_t>		 samplesBuffer;
		public:
			static const String	&GetComponentSpecs();

						 FAACOut();
						~FAACOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			String			 GetOutputFileExtension();

			ConfigLayer		*GetConfigurationLayer();
			Void			 FreeConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(FAACOut)

BoCA_END_COMPONENT(FAACOut)
