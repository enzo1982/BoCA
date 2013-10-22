 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(EncoderWMA)

namespace BoCA
{
	class EncoderWMA : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			IWMWriter		*m_pWriter;
			IWMWriterAdvanced	*m_pWriterAdvanced;

			IWMWriterFileSink	*m_pWriterFileSink;

			IWMProfileManager	*m_pProfileManager;
			IWMProfile		*m_pProfile;

			IWMStreamConfig		*m_pStreamConfig;

			Int64			 samplesWritten;

			Buffer<signed short>	 samplesBuffer;

			Int			 GetDefaultCodec(IWMCodecInfo3 *);
			IWMStreamConfig		*GetBestCodecFormat(IWMCodecInfo3 *, DWORD, const Format &);

			Bool			 SetInputFormat(IWMWriter *, const Format &);
		public:
			static const String	&GetComponentSpecs();

						 EncoderWMA();
						~EncoderWMA();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderWMA)

BoCA_END_COMPONENT(EncoderWMA)
