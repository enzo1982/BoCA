 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

namespace CA
{
#	include "CoreAudio/AudioFile.h"
#	include "CoreAudio/AudioFormat.h"
};

BoCA_BEGIN_COMPONENT(EncoderCoreAudioConnect)

namespace BoCA
{
	class EncoderCoreAudioConnect : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;
			Config			*config;

			String			 mappingName;

#ifdef __WIN32__
			HANDLE			 connector;
			HANDLE			 semaphore;
			HANDLE			 mapping;
#else
			pid_t			 connector;
			int			 semaphore;
			int			 mapping;
#endif

			CoreAudioCommBuffer	*comm;
			Bool			 connected;
			Bool			 ready;

			CA::UInt32		 fileType;

			Bool			 Connect();
			Bool			 Disconnect();

			Bool			 ProcessConnectorCommand();

			Bool			 FixupDurationAtoms(Int64, IO::InStream &, IO::OutStream &, Int64, Bool = False);

			static Bool		 ConvertArguments(Config *);
		public:
			static const String	&GetComponentSpecs();

						 EncoderCoreAudioConnect();
						~EncoderCoreAudioConnect();

			Bool			 IsReady() const;
			Bool			 IsLossless() const;

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			Bool			 SetOutputFormat(Int);
			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderCoreAudioConnect)

BoCA_END_COMPONENT(EncoderCoreAudioConnect)
