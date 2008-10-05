 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(VorbisOut)

namespace BoCA
{
	class VorbisOut : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;

			ogg_stream_state	 os;
			ogg_page		 og;
			ogg_packet		 op;

			vorbis_info		 vi;
			vorbis_comment		 vc;
			vorbis_dsp_state	 vd;
			vorbis_block		 vb;

			Buffer<unsigned short>	 samplesBuffer;

			Int			 WriteOggPackets(Bool);
		public:
			static const String	&GetComponentSpecs();

						 VorbisOut();
						~VorbisOut();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &, Int);

			ConfigLayer		*GetConfigurationLayer();
			Void			 FreeConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(VorbisOut)

BoCA_END_COMPONENT(VorbisOut)
