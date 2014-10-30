 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include <windows.h>

#include "connector/communication.h"

BoCA_BEGIN_COMPONENT(EncoderCoreAudioConnect)

namespace BoCA
{
	class EncoderCoreAudioConnect : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			HANDLE			 connector;
			HANDLE			 mapping;

			CoreAudioCommBuffer	*comm;
			Bool			 ready;

			Bool			 ProcessConnectorCommand();
		public:
			static const String	&GetComponentSpecs();

						 EncoderCoreAudioConnect();
						~EncoderCoreAudioConnect();

			Bool			 IsReady();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderCoreAudioConnect)

BoCA_END_COMPONENT(EncoderCoreAudioConnect)
