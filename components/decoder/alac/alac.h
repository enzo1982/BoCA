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
#include "dllinterface.h"

#include "alac/ALACDecoder.h"

BoCA_BEGIN_COMPONENT(DecoderALAC)

namespace BoCA
{
	class DecoderALAC : public CS::DecoderComponent
	{
		private:
			MP4FileHandle		 mp4File;
			ALACDecoder		 decoder;

			Int			 mp4Track;
			MP4SampleId		 sampleId;

			UnsignedInt		 skipSamples;
			UnsignedInt64		 samplesLeft;

			Buffer<unsigned char>	 dataBuffer;

			Int			 GetAudioTrack(MP4FileHandle) const;
		public:
			static const String	&GetComponentSpecs();

						 DecoderALAC();
						~DecoderALAC();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Bool			 Seek(Int64);

			Int			 ReadData(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderALAC)

BoCA_END_COMPONENT(DecoderALAC)
