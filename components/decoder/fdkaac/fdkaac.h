 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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

			Int				 mp4Track;
			Int				 sampleId;

			Bool				 adifFound;
			Bool				 adtsFound;
			Bool				 loasFound;

			Int				 frameSize;

			Int				 delaySamples;
			Int				 delaySamplesLeft;

			Buffer<unsigned char>		 dataBuffer;
			Buffer<short>			 samplesBuffer;

			Int				 GetAudioTrack(MP4FileHandle) const;

			Bool				 SkipID3v2Tag(IO::InStream *);
			Bool				 SyncOnAACHeader(IO::InStream *);
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
