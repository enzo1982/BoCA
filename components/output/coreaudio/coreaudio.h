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

BoCA_BEGIN_COMPONENT(OutputCoreAudio)

namespace BoCA
{
	class OutputCoreAudio : public CS::OutputComponent
	{
		friend CA::OSStatus		 AudioCallback(void *, CA::AudioUnitRenderActionFlags *, const CA::AudioTimeStamp *, CA::UInt32, CA::UInt32, CA::AudioBufferList *);

		private:
			CA::AudioUnit		 audioUnit;
			Bool			 paused;

			Buffer<UnsignedByte>	 samplesBuffer;

			Threads::Mutex		*samplesBufferMutex;
		public:
			static const String	&GetComponentSpecs();

						 OutputCoreAudio();
						~OutputCoreAudio();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			Int			 CanWrite();

			Int			 SetPause(Bool);
			Bool			 IsPlaying();
	};
};

BoCA_DEFINE_OUTPUT_COMPONENT(OutputCoreAudio)

BoCA_END_COMPONENT(OutputCoreAudio)
