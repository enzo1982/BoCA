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
#include "dllinterface.h"

using namespace smooth::Threads;

BoCA_BEGIN_COMPONENT(DecoderWinamp)

namespace BoCA
{
	class DecoderWinamp : public CS::DecoderComponent
	{
		friend int	 Out_Open(int, int, int, int, int);
		friend int	 Out_CanWrite();
		friend int	 Out_Write(char *, int);

		private:
			ConfigLayer		*configLayer;

			Buffer<Byte>		 samplesBuffer;
			Mutex			*samplesBufferMutex;
			Int64			 samplesDone;

			Track			*infoTrack;

			In_Module		*plugin;

			In_Module		*GetPluginForFile(const String &) const;
		public:
			static const String	&GetComponentSpecs();

						 DecoderWinamp();
						~DecoderWinamp();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderWinamp)

BoCA_END_COMPONENT(DecoderWinamp)
