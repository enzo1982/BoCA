 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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

namespace BoCA
{
	class SuperWorker : public Threads::Thread
	{
		private:
			Threads::Semaphore		 processSignal;
			Threads::Semaphore		 readySignal;

			void				*encoder;
			SpeexBits			 bits;

			Format				 format;

			spx_int32_t			 frameSize;

			Buffer<spx_int16_t>		 samplesBuffer;

			Buffer<unsigned char>		 packetBuffer;
			Array<Int>			 packetSizes;

			Bool				 quit;

			Int				 Run();
		public:
							 SuperWorker(const Config *, const Format &);
							~SuperWorker();

			Void				 Encode(const Buffer<spx_int16_t> &, Int, Int);
			Void				 WaitUntilReady();

			Int				 Quit();

			const Buffer<unsigned char>	&GetPackets() const	{ return packetBuffer; };
			const Array<Int>		&GetPacketSizes() const	{ return packetSizes; };
	};
};
