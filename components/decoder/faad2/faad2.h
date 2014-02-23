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
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(DecoderFAAD2)

namespace BoCA
{
	class DecoderFAAD2 : public CS::DecoderComponent
	{
		private:
			MP4FileHandle			 mp4File;
			NeAACDecHandle			 handle;
			NeAACDecConfigurationPtr	 fConfig;

			Int				 mp4Track;
			Int				 sampleId;

			Int				 frameSize;
			Int				 delaySamples;

			Buffer<unsigned char>		 dataBuffer;
			Buffer<unsigned char>		 backBuffer;
			Buffer<short>			 samplesBuffer;

			Int				 GetAudioTrack(MP4FileHandle) const;

			Bool				 SkipID3v2Tag(IO::InStream *);
			Bool				 SyncOnAACHeader(IO::InStream *);
		public:
			static const String		&GetComponentSpecs();

							 DecoderFAAD2();
							~DecoderFAAD2();

			Bool				 CanOpenStream(const String &);
			Error				 GetStreamInfo(const String &, Track &);

			Bool				 Activate();
			Bool				 Deactivate();

			Bool				 Seek(Int64);

			Int				 ReadData(Buffer<UnsignedByte> &, Int);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderFAAD2)

BoCA_END_COMPONENT(DecoderFAAD2)
