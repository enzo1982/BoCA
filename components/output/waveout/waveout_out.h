 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

BoCA_BEGIN_COMPONENT(WaveOutOut)

using namespace smooth::Threads;

namespace BoCA
{
	class WaveOutOut : public CS::OutputComponent
	{
		private:
			HWAVEOUT		 hWaveOut;
			Array<WAVEHDR *>	 headers;

			Thread			*thread;
			Bool			 stop;

			CRITICAL_SECTION	 sync;

			HANDLE			 hEvent;
			char			*buffer;
			UINT			 buf_size, buf_size_used;
			UINT			 data_written, write_ptr;
			UINT			 minblock, maxblock, avgblock;
			DWORD			 last_time;
			DWORD			 p_time;

			UINT			 n_playing;
			bool			 paused;
			bool			 needplay;
			bool			 newpause;

			Void			 WorkerThread();
		public:
			static const String	&GetComponentSpecs();

						 WaveOutOut();
						~WaveOutOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			Int			 CanWrite();

			Int			 SetPause(Bool);
			Bool			 IsPlaying();
	};
};

BoCA_DEFINE_OUTPUT_COMPONENT(WaveOutOut)

BoCA_END_COMPONENT(WaveOutOut)
