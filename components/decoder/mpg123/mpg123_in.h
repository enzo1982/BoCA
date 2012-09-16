 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "mpg123/mpg123.h"

BoCA_BEGIN_COMPONENT(MPG123In)

namespace BoCA
{
	class MPG123In : public CS::DecoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			mpg123_handle		*context;

			Buffer<short>		 pcm_l;
			Buffer<short>		 pcm_r;

			Int			 delaySamples;
			Int			 padSamples;

			Int			 delaySamplesLeft;

			Bool			 SkipID3v2Tag(IO::InStream *);
			Bool			 ParseVBRHeaders(IO::InStream *);
		public:
			static const String	&GetComponentSpecs();

						 MPG123In();
						~MPG123In();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(MPG123In)

BoCA_END_COMPONENT(MPG123In)
