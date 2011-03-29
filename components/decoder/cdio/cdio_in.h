 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>

extern "C" {
#	define DO_NOT_WANT_PARANOIA_COMPATIBILITY
#	include <cdio/cdda.h>
#	include <cdio/paranoia.h>
}

BoCA_BEGIN_COMPONENT(CDIOIn)

namespace BoCA
{
	class CDIOIn : public CS::DecoderComponent
	{
		private:
			ConfigLayer			*configLayer;

			cdrom_drive_t			*drive;
			cdrom_paranoia_t		*paranoia;

			Int				 nextSector;
			Int				 sectorsLeft;
		public:
			static const String		&GetComponentSpecs();

			static const Array<String>	&FindDrives();

							 CDIOIn();
							~CDIOIn();

			Bool				 CanOpenStream(const String &);
			Error				 GetStreamInfo(const String &, Track &);

			Bool				 Activate();
			Bool				 Deactivate();

			Int				 ReadData(Buffer<UnsignedByte> &, Int);

			ConfigLayer			*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(CDIOIn)

BoCA_END_COMPONENT(CDIOIn)
