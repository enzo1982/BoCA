 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(EncoderFDKAAC)

namespace BoCA
{
	class EncoderFDKAAC : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			MP4FileHandle		 mp4File;
			HANDLE_AACENCODER	 handle;

			Int			 mp4Track;
			Int			 sampleId;

			Int			 frameSize;

			Int			 totalSamples;
			Int			 delaySamples;

			Buffer<unsigned char>	 outBuffer;
			Buffer<int16_t>		 samplesBuffer;

			Int			 EncodeFrames(Buffer<int16_t> &, Buffer<unsigned char> &, Bool);

			Int			 GetSampleRateIndex(Int);
		public:
			static const String	&GetComponentSpecs();

						 EncoderFDKAAC();
						~EncoderFDKAAC();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			String			 GetOutputFileExtension();

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderFDKAAC)

BoCA_END_COMPONENT(EncoderFDKAAC)
