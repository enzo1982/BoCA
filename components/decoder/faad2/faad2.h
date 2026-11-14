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

BoCA_BEGIN_COMPONENT(DecoderFAAD2)

namespace BoCA
{
	class DecoderFAAD2 : public CS::DecoderComponent
	{
		private:
			AS::MuxerComponent		*muxer;

			NeAACDecHandle			 handle;
			NeAACDecConfigurationPtr	 fConfig;

			Int				 frameSize;
			Int				 sbrRatio;

			Int				 delaySamples;
			Int				 delaySamplesLeft;

			Buffer<unsigned char>		 dataBuffer;
			Buffer<unsigned char>		 backBuffer;
			Buffer<short>			 samplesBuffer;

			AS::MuxerComponent		*OpenMuxer(IO::Driver *) const;
			Void				 CloseMuxer(AS::MuxerComponent *) const;

			Int				 GetAudioObjectType(const Buffer<UnsignedByte> &) const;

			Bool				 SkipID3v2Tag(IO::InStream &);
			Bool				 SyncOnAACHeader(IO::InStream &);
		public:
			static const String		&GetComponentSpecs();

							 DecoderFAAD2();
							~DecoderFAAD2();

			Bool				 CanOpenStream(const String &);
			Error				 GetStreamInfo(const String &, Track &);

			Bool				 Activate();
			Bool				 Deactivate();

			Bool				 Seek(Int64);

			Int				 ReadData(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderFAAD2)

BoCA_END_COMPONENT(DecoderFAAD2)
