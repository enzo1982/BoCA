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
#include "mad/mad.h"

using namespace smooth::Threads;

BoCA_BEGIN_COMPONENT(MADIn)

namespace BoCA
{
	class MADIn : public CS::DecoderComponent
	{
		friend mad_flow	 MADInputCallback(void *, mad_stream *);
		friend mad_flow	 MADOutputCallback(void *, const mad_header *, mad_pcm *);
		friend mad_flow	 MADHeaderCallback(void *, const mad_header *, mad_pcm *);
		friend mad_flow	 MADErrorCallback(void *, mad_stream *, mad_frame *);

		private:
			mad_decoder		 decoder;
			Bool			 finished;

			Buffer<signed int>	 samplesBuffer;

			Track			*infoTrack;

			Int			 numFrames;

			Mutex			*readDataMutex;
			Mutex			*samplesBufferMutex;
			Thread			*decoderThread;

			Bool			 SkipID3v2Tag(IO::InStream *);
			Bool			 ReadXingTag(IO::InStream *);

			Int			 ReadMAD(Bool);
		public:
			static const String	&GetComponentSpecs();

						 MADIn();
						~MADIn();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &, Int);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(MADIn)

BoCA_END_COMPONENT(MADIn)
