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
#include "wma_reader.h"

using namespace smooth::Threads;

BoCA_BEGIN_COMPONENT(WMAIn)

namespace BoCA
{
	class WMAIn : public CS::DecoderComponent
	{
		friend class WMAReader;

		private:
			WMAReader		*readerCallback;

			Buffer<UnsignedByte>	 samplesBuffer;
			Mutex			 samplesBufferMutex;

			HANDLE			 m_hAsyncEvent;

			IWMReader		*m_pReader;
			IWMReaderAdvanced	*m_pReaderAdvanced;

			HRESULT			 GetHeaderAttribute(IWMHeaderInfo *, LPCWSTR, BYTE **);

			Void			 WaitForEvent(HANDLE, DWORD = INFINITE);
		public:
			static const String	&GetComponentSpecs();

						 WMAIn();
						~WMAIn();

			Bool			 CanOpenStream(const String &);

			Error			 GetStreamInfo(const String &, Track &);
			Error			 UpdateStreamInfo(const String &, const Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &, Int);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(WMAIn)

BoCA_END_COMPONENT(WMAIn)
