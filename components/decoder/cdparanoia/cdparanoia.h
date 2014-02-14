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

extern "C" {
#	include <cdda_interface.h>
#	include <cdda_paranoia.h>
}

BoCA_BEGIN_COMPONENT(DecoderCDParanoia)

namespace BoCA
{
	class DecoderCDParanoia : public CS::DecoderComponent
	{
		private:
			ConfigLayer			*configLayer;

			cdrom_drive			*drive;
			cdrom_paranoia			*paranoia;

			Int				 nextSector;
			Int				 sectorsLeft;

			Bool				 GetTrackSectors(Int &, Int &);
		public:
			static const String		&GetComponentSpecs();

							 DecoderCDParanoia();
							~DecoderCDParanoia();

			Bool				 CanOpenStream(const String &);
			Error				 GetStreamInfo(const String &, Track &);

			Bool				 Activate();
			Bool				 Deactivate();

			Bool				 Seek(Int64);

			Int				 ReadData(Buffer<UnsignedByte> &, Int);

			ConfigLayer			*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderCDParanoia)

BoCA_END_COMPONENT(DecoderCDParanoia)
