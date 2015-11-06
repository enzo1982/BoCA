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

#include "connector/communication.h"

BoCA_BEGIN_COMPONENT(EncoderCoreAudioConnect)

namespace BoCA
{
	class EncoderCoreAudioConnect : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			String			 fileName;
			String			 mappingName;

#ifdef __WIN32__
			HANDLE			 connector;
			HANDLE			 mapping;
#else
			pid_t			 connector;
			int			 mapping;
#endif

			CoreAudioCommBuffer	*comm;
			Bool			 connected;
			Bool			 ready;

			Bool			 Connect();
			Bool			 Disconnect();

			Bool			 ProcessConnectorCommand();
		public:
			static const String	&GetComponentSpecs();

						 EncoderCoreAudioConnect();
						~EncoderCoreAudioConnect();

			Bool			 IsReady() const;
			Bool			 IsLossless() const;

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderCoreAudioConnect)

BoCA_END_COMPONENT(EncoderCoreAudioConnect)
