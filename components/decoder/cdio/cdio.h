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

#include <cdio/cdda.h>
#include <cdio/paranoia.h>

BoCA_BEGIN_COMPONENT(DecoderCDIO)

namespace BoCA
{
	class DecoderCDIO : public CS::DecoderComponent
	{
		private:
			ConfigLayer			*configLayer;

			CdIo_t				*cd;

			cdrom_drive_t			*drive;
			cdrom_paranoia_t		*paranoia;

			Int				 nextSector;
			Int				 sectorsLeft;

			Bool				 GetTrackSectors(Int &, Int &);
		public:
			static const String		&GetComponentSpecs();

			static const Array<String>	&FindDrives();

							 DecoderCDIO();
							~DecoderCDIO();

			Bool				 CanOpenStream(const String &);
			Error				 GetStreamInfo(const String &, Track &);

			Bool				 Activate();
			Bool				 Deactivate();

			Bool				 Seek(Int64);

			Int				 ReadData(Buffer<UnsignedByte> &, Int);

			ConfigLayer			*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderCDIO)

BoCA_END_COMPONENT(DecoderCDIO)