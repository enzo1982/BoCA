 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

namespace BoCA
{
	class Resampler : public Threads::Thread
	{
		private:
			Threads::Mutex			 resamplerMutex;

			AS::DSPComponent		*resampler;
			Config				*resamplerConfig;

			Buffer<UnsignedByte>		 samplesBuffer;

			Bool				 process;
			Bool				 flush;
			Bool				 quit;

			Bool				 errorState;
			String				 errorString;

			Int				 Run();
		public:
							 Resampler(const Config *, const Track &, Int);
							~Resampler();

			Void				 Resample(const Buffer<UnsignedByte> &);
			Void				 Flush();

			Int				 Quit();

			Bool				 Lock()			{ return resamplerMutex.Lock(); }
			Bool				 Release()		{ return resamplerMutex.Release(); }

			Bool				 IsReady() const	{ return !process; }

			const Buffer<UnsignedByte>	&GetSamples() const	{ return samplesBuffer; };

			Bool				 GetErrorState() const	{ return errorState; }
			const String			&GetErrorString() const	{ return errorString; }
	};
};
