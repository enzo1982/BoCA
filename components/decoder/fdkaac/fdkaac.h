 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(DecoderFDKAAC)

namespace BoCA
{
	class DecoderFDKAAC : public CS::DecoderComponent
	{
		private:
			MP4FileHandle			 mp4File;
			HANDLE_AACDECODER		 handle;

			MP4TrackId			 mp4Track;
			MP4SampleId			 sampleId;

			Bool				 finished;

			Bool				 adifFound;
			Bool				 adtsFound;
			Bool				 loasFound;

			Int				 frameSize;
			Int				 sbrRatio;

			Int				 delaySamples;
			Int				 delaySamplesLeft;

			Buffer<unsigned char>		 dataBuffer;
			Buffer<short>			 samplesBuffer;

			Bool				 ReadGaplessInfo(MP4FileHandle, Int &, Int &, Int64 &) const;

			Bool				 SkipID3v2Tag(IO::InStream &);
			Bool				 SyncOnAACHeader(IO::InStream &);

			static UnsignedInt32		 GetDecoderVersion();
		public:
			static const String		&GetComponentSpecs();

							 DecoderFDKAAC();
							~DecoderFDKAAC();

			Bool				 CanOpenStream(const String &);
			Error				 GetStreamInfo(const String &, Track &);

			Bool				 Activate();
			Bool				 Deactivate();

			Bool				 Seek(Int64);

			Int				 ReadData(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderFDKAAC)

BoCA_END_COMPONENT(DecoderFDKAAC)
