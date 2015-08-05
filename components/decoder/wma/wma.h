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
#include "wmareader.h"

using namespace smooth::Threads;

BoCA_BEGIN_COMPONENT(DecoderWMA)

namespace BoCA
{
	class DecoderWMA : public CS::DecoderComponent
	{
		friend class WMAReader;

		private:
			WMAReader		*readerCallback;

			Buffer<UnsignedByte>	 samplesBuffer;
			Mutex			 samplesBufferMutex;

			Bool			 userProvidedClock;

			HANDLE			 m_hAsyncEvent;

			IWMReader		*m_pReader;
			IWMReaderAdvanced2	*m_pReaderAdvanced;

			HRESULT			 GetHeaderAttribute(IWMHeaderInfo *, LPCWSTR, BYTE **);

			Void			 WaitForEvent(HANDLE, DWORD = INFINITE);
		public:
			static const String	&GetComponentSpecs();

						 DecoderWMA();
						~DecoderWMA();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Bool			 Seek(Int64);

			Int			 ReadData(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderWMA)

BoCA_END_COMPONENT(DecoderWMA)
