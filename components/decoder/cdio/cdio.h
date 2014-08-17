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

#include <cdio/cdio.h>

#if HAVE_OLD_PARANOIA_INCLUDE
#	include <cdio/paranoia.h>
#else
#	include <cdio/paranoia/paranoia.h>
#endif

BoCA_BEGIN_COMPONENT(DecoderCDIO)

namespace BoCA
{
	class DecoderCDIO : public CS::DecoderComponent
	{
		private:
			static UnsignedInt64	 lastRead;

			ConfigLayer		*configLayer;

			CdIo_t			*cd;

			cdrom_drive_t		*drive;
			cdrom_paranoia_t	*paranoia;

			Int			 nextSector;
			Int			 sectorsLeft;

			Int			 readOffset;

			Int			 skipSamples;
			Int			 prependSamples;

			Bool			 GetTrackSectors(Int &, Int &);
		public:
			static const String	&GetComponentSpecs();

						 DecoderCDIO();
						~DecoderCDIO();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Bool			 Seek(Int64);

			Int			 ReadData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderCDIO)

BoCA_END_COMPONENT(DecoderCDIO)
