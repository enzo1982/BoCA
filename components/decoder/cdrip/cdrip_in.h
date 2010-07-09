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

#include "info/cdtext.h"
#include "info/cdplayerini.h"

BoCA_BEGIN_COMPONENT(CDRipIn)

namespace BoCA
{
	class CDRipIn : public CS::DecoderComponent
	{
		private:
			static CDText		 cdText;
			static Int		 cdTextDiscID;

			static CDPlayerIni	 cdPlayer;
			static Int		 cdPlayerDiscID;

			ConfigLayer		*configLayer;

			Bool			 ripperOpen;

			Int			 dataBufferSize;

			Int			 ComputeDiscID();

			Bool			 OpenRipper(Int, Int);
			Bool			 CloseRipper();
		public:
			static const String	&GetComponentSpecs();

						 CDRipIn();
						~CDRipIn();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 ReadData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
			Void			 FreeConfigurationLayer();
	};
};

BoCA_DEFINE_DECODER_COMPONENT(CDRipIn)

BoCA_END_COMPONENT(CDRipIn)
