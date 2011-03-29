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

using namespace smooth::Threads;

BoCA_BEGIN_COMPONENT(CoreAudioOut)

namespace BoCA
{
	class CoreAudioOut : public CS::OutputComponent
	{
		friend CA::OSStatus		 AudioCallback(void *, CA::AudioUnitRenderActionFlags *, const CA::AudioTimeStamp *, CA::UInt32, CA::UInt32, CA::AudioBufferList *);

		private:
			CA::AudioUnit		 audioUnit;
			Bool			 paused;

			Buffer<UnsignedByte>	 samplesBuffer;

			Mutex			*samplesBufferMutex;
		public:
			static const String	&GetComponentSpecs();

						 CoreAudioOut();
						~CoreAudioOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			Int			 CanWrite();

			Int			 SetPause(Bool);
			Bool			 IsPlaying();
	};
};

BoCA_DEFINE_OUTPUT_COMPONENT(CoreAudioOut)

BoCA_END_COMPONENT(CoreAudioOut)
