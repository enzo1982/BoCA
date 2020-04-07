 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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

#include <cdio/cdio.h>

#if HAVE_OLD_PARANOIA_INCLUDE
#	include <cdio/paranoia.h>
#else
#	include <cdio/paranoia/paranoia.h>
#endif

#include "info/cdtext.h"

BoCA_BEGIN_COMPONENT(DecoderCDIO)

namespace BoCA
{
	class DecoderCDIO : public CS::DecoderComponent
	{
		friend void	 paranoiaCallback(long int, paranoia_cb_mode_t);

		constants:
			static const UnsignedInt	 bytesPerSector	  = 2352;
			static const UnsignedInt	 samplesPerSector =  588;
		private:
			static Threads::Mutex		 readMutex;
			static DecoderCDIO		*readDecoder;

			static CDText			 cdText;
			static UnsignedInt32		 cdTextDiscID;

			static Array<UnsignedInt64>	 lastRead;

			ConfigLayer			*configLayer;

			CdIo_t				*cd;

			cdrom_drive_t			*drive;
			cdrom_paranoia_t		*paranoia;

			Int				 nextSector;
			Int				 sectorsLeft;

			Int				 readOffset;

			Int				 skipSamples;
			Int				 prependSamples;
			Int				 appendSamples;

			Int				 numCacheErrors;

			UnsignedInt32			 ComputeDiscID(const MCDI &);

			Bool				 GetTrackSectors(Int &, Int &, Bool &);
		public:
			static const String		&GetComponentSpecs();

							 DecoderCDIO();
							~DecoderCDIO();

			Bool				 CanOpenStream(const String &);
			Error				 GetStreamInfo(const String &, Track &);

			Bool				 Activate();
			Bool				 Deactivate();

			Bool				 Seek(Int64);

			Int				 ReadData(Buffer<UnsignedByte> &);

			ConfigLayer			*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderCDIO)

BoCA_END_COMPONENT(DecoderCDIO)
