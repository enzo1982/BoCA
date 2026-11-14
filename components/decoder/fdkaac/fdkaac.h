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

BoCA_BEGIN_COMPONENT(DecoderFDKAAC)

namespace BoCA
{
	class DecoderFDKAAC : public CS::DecoderComponent
	{
		private:
			AS::MuxerComponent		*muxer;

			HANDLE_AACDECODER		 handle;

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

			AS::MuxerComponent		*OpenMuxer(IO::Driver *) const;
			Void				 CloseMuxer(AS::MuxerComponent *) const;

			Int				 GetAudioObjectType(const Buffer<UnsignedByte> &) const;

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
