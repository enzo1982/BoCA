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
#include <dsound.h>

BoCA_BEGIN_COMPONENT(OutputDirectSound)

using namespace smooth::Threads;

namespace BoCA
{
	class OutputDirectSound : public CS::OutputComponent
	{
		private:
			IDirectSound8		*directSound;

			IDirectSoundBuffer	*primaryBuffer;
			IDirectSoundBuffer	*workBuffer;

			UnsignedInt		 bufferSize;
			UnsignedInt		 bufferPos;

			Bool			 firstBlock;
		public:
			static const String	&GetComponentSpecs();

						 OutputDirectSound();
						~OutputDirectSound();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);
			Bool			 Finish();

			Int			 CanWrite();

			Int			 SetPause(Bool);
			Bool			 IsPlaying();
	};
};

BoCA_DEFINE_OUTPUT_COMPONENT(OutputDirectSound)

BoCA_END_COMPONENT(OutputDirectSound)
