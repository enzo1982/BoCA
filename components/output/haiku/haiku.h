 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2016 Robert Kausch <robert.kausch@freac.org>
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

using namespace smooth::Threads;

BoCA_BEGIN_COMPONENT(OutputHaiku)

namespace BoCA
{
	class OutputHaiku : public CS::OutputComponent
	{
		friend void	 PlayBuffer(void *, void *, size_t, const media_raw_audio_format &);

		private:
			BSoundPlayer		*player;
			Bool			 paused;

			Buffer<UnsignedByte>	 samplesBuffer;

			Mutex			*samplesBufferMutex;
		public:
			static const String	&GetComponentSpecs();

						 OutputHaiku();
						~OutputHaiku();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			Int			 CanWrite();

			Int			 SetPause(Bool);
			Bool			 IsPlaying();
	};
};

BoCA_DEFINE_OUTPUT_COMPONENT(OutputHaiku)

BoCA_END_COMPONENT(OutputHaiku)
