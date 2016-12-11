 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2016 Robert Kausch <robert.kausch@freac.org>
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

extern "C" {
#	include <cdda_interface.h>
#	include <cdda_paranoia.h>
}

BoCA_BEGIN_COMPONENT(DecoderCDParanoia)

namespace BoCA
{
	class DecoderCDParanoia : public CS::DecoderComponent
	{
		friend void	 paranoiaCallback(long, int);

		constants:
			static const UnsignedInt	 bytesPerSector	  = 2352;
			static const UnsignedInt	 samplesPerSector =  588;
		private:
			static Threads::Mutex		 readMutex;
			static DecoderCDParanoia	*readDecoder;

			static Array<UnsignedInt64>	 lastRead;

			ConfigLayer			*configLayer;

			cdrom_drive			*drive;
			cdrom_paranoia			*paranoia;

			Int				 nextSector;
			Int				 sectorsLeft;

			Int				 readOffset;

			Int				 skipSamples;
			Int				 prependSamples;
			Int				 appendSamples;

			Int				 numCacheErrors;

			Bool				 GetTrackSectors(Int &, Int &, Bool &);
		public:
			static const String		&GetComponentSpecs();

							 DecoderCDParanoia();
							~DecoderCDParanoia();

			Bool				 CanOpenStream(const String &);
			Error				 GetStreamInfo(const String &, Track &);

			Bool				 Activate();
			Bool				 Deactivate();

			Bool				 Seek(Int64);

			Int				 ReadData(Buffer<UnsignedByte> &);

			ConfigLayer			*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderCDParanoia)

BoCA_END_COMPONENT(DecoderCDParanoia)
