 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(EncoderVOAAC)

namespace BoCA
{
	class EncoderVOAAC : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			MP4FileHandle		 mp4File;
			VO_HANDLE		 handle;

			VO_AUDIO_CODECAPI	 api;

			VO_MEM_OPERATOR		 memOperator;
			VO_CODEC_INIT_USERDATA	 userData;

			Int			 mp4Track;
			Int			 sampleId;

			Int			 frameSize;

			Int			 totalSamples;
			Int			 delaySamples;

			Buffer<unsigned char>	 outBuffer;
			Buffer<int16_t>		 samplesBuffer;

			Int			 EncodeFrames(Buffer<int16_t> &, Buffer<unsigned char> &, Bool);

			Int			 GetSampleRateIndex(Int) const;
		public:
			static const String	&GetComponentSpecs();

						 EncoderVOAAC();
						~EncoderVOAAC();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderVOAAC)

BoCA_END_COMPONENT(EncoderVOAAC)
