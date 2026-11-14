 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(MuxerOgg)

namespace BoCA
{
	class MuxerOgg : public CS::MuxerComponent
	{
		private:
			MuxerMode		 mode;

			Bool			 opened;
			Bool			 streamInitialized;

			ogg_sync_state		 oy;
			ogg_stream_state	 os;
			ogg_page		 og;
			ogg_packet		 op;

			Int64			 pagePosition;
			Int64			 pageStartPosition;

			Bool			 ReadPage();
			Int			 WritePages(Bool);
		public:
			static const String	&GetComponentSpecs();

						 MuxerOgg();
						~MuxerOgg();

			Bool			 Open(MuxerMode);
			Bool			 Close();

			Int64			 GetDuration();

			Int			 ReadPacket(Buffer<UnsignedByte> &, Int64 &, Int &);
			Int			 WritePacket(const UnsignedByte *, Int, Int64, Int);

			Int64			 SeekPacket(Int64);
	};
};

BoCA_DEFINE_MUXER_COMPONENT(MuxerOgg)

BoCA_END_COMPONENT(MuxerOgg)
