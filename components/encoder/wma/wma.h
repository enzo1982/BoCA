 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(EncoderWMA)

namespace BoCA
{
	class EncoderWMA : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;
			Config			*config;

			IWMWriter		*writer;
			IWMWriterAdvanced	*writerAdvanced;

			IWMWriterFileSink	*writerFileSink;

			IWMProfileManager	*profileManager;
			IWMProfile		*profile;

			IWMStreamConfig		*streamConfig;

			Int64			 samplesWritten;

			Buffer<signed short>	 samplesBuffer;

			static Int		 GetDefaultCodec(IWMCodecInfo3 *);
			IWMStreamConfig		*GetBestCodecFormat(IWMCodecInfo3 *, DWORD, const Format &) const;

			Bool			 SetInputFormat(IWMWriter *, const Format &);

			static Bool		 ConvertArguments(Config *);
		public:
			static const String	&GetComponentSpecs();

			static Void		 Initialize();
			static Void		 Cleanup();

						 EncoderWMA();
						~EncoderWMA();

			Int			 GetNumberOfPasses() const;

			Bool			 IsLossless() const;

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);
			Bool			 NextPass();

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderWMA)

BoCA_END_COMPONENT(EncoderWMA)
