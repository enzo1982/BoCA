 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(EncoderVorbis)

namespace BoCA
{
	class EncoderVorbis : public CS::EncoderComponent
	{
		private:
			ConfigLayer		*configLayer;
			Config			*config;

			ogg_stream_state	 os;
			ogg_page		 og;
			ogg_packet		 op;

			vorbis_info		 vi;
			vorbis_comment		 vc;
			vorbis_dsp_state	 vd;
			vorbis_block		 vb;

			Int			 WriteOggPackets(Bool);

			static Bool		 ConvertArguments(Config *);
		public:
			static const String	&GetComponentSpecs();

						 EncoderVorbis();
						~EncoderVorbis();

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderVorbis)

BoCA_END_COMPONENT(EncoderVorbis)
