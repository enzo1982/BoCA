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

BoCA_BEGIN_COMPONENT(MuxerMP4)

namespace BoCA
{
	class MuxerMP4 : public CS::MuxerComponent
	{
		private:
			MuxerMode		 mode;

			MP4FileHandle		 mp4File;
			MP4TrackId		 mp4Track;

			MP4SampleId		 sampleId;
			MP4SampleId		 numberOfSamples;

			Int64			 position;

			Bool			 ReadGaplessInfo();
			Bool			 WriteGaplessInfo();
		public:
			static const String	&GetComponentSpecs();

						 MuxerMP4();
						~MuxerMP4();

			Bool			 Open(MuxerMode);
			Bool			 Close();

			Int64			 GetNumberOfPackets();
			Int64			 GetDuration();

			Int			 ReadPacket(Buffer<UnsignedByte> &, Int64 &, Int &);
			Int			 WritePacket(const UnsignedByte *, Int, Int64, Int);

			Int64			 SeekPacket(Int64);

			Bool			 OptimizeFile(const String &);
	};
};

BoCA_DEFINE_MUXER_COMPONENT(MuxerMP4)

BoCA_END_COMPONENT(MuxerMP4)
