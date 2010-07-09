 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

BoCA_BEGIN_COMPONENT(FAAD2In)

namespace BoCA
{
	class FAAD2In : public CS::DecoderComponent
	{
		private:
			MP4FileHandle			 mp4File;
			NeAACDecHandle			 handle;
			NeAACDecConfigurationPtr	 fConfig;

			Int				 mp4Track;
			Int				 sampleId;

			Buffer<unsigned char>		 dataBuffer;
			Buffer<unsigned char>		 samplesBuffer;
			Buffer<unsigned char>		 backBuffer;

			Int				 GetAudioTrack();

			Bool				 SkipID3v2Tag(IO::InStream *);
			Bool				 SyncOnAACHeader(IO::InStream *);
		public:
			static const String		&GetComponentSpecs();

							 FAAD2In();
							~FAAD2In();

			Bool				 CanOpenStream(const String &);
			Error				 GetStreamInfo(const String &, Track &);

			Bool				 Activate();
			Bool				 Deactivate();

			Int				 ReadData(Buffer<UnsignedByte> &, Int);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(FAAD2In)

BoCA_END_COMPONENT(FAAD2In)
