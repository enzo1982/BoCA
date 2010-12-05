 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
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

BoCA_BEGIN_COMPONENT(CDParanoiaIn)

namespace BoCA
{
	class CDParanoiaIn : public CS::DecoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			cdrom_drive		*drive;
			cdrom_paranoia		*paranoia;

			Int			 nextSector;
			Int			 sectorsLeft;
		public:
			static const String	&GetComponentSpecs();

						 CDParanoiaIn();
						~CDParanoiaIn();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(CDParanoiaIn)

BoCA_END_COMPONENT(CDParanoiaIn)
