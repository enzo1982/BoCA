 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(DecoderMPG123)

namespace BoCA
{
	class DecoderMPG123 : public CS::DecoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			mpg123_handle		*context;

			Buffer<short>		 pcm_l;
			Buffer<short>		 pcm_r;

			Int			 numFrames;

			Int			 delaySamples;
			Int			 padSamples;

			Int			 delaySamplesLeft;

			Int			 dataOffset;

			Bool			 SkipID3v2Tag(IO::InStream *);
			Bool			 ParseVBRHeaders(IO::InStream *);
		public:
			static const String	&GetComponentSpecs();

						 DecoderMPG123();
						~DecoderMPG123();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Bool			 Seek(Int64);

			Int			 ReadData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderMPG123)

BoCA_END_COMPONENT(DecoderMPG123)
