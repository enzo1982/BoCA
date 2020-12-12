 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(DecoderFLAC)

namespace BoCA
{
	class DecoderFLAC : public CS::DecoderComponent
	{
		friend FLAC__StreamDecoderReadStatus	 FLACStreamDecoderReadCallback(const FLAC__StreamDecoder *, FLAC__byte [], size_t *, void *);
		friend FLAC__StreamDecoderWriteStatus	 FLACStreamDecoderWriteCallback(const FLAC__StreamDecoder *, const FLAC__Frame *, const FLAC__int32 * const [], void *);
		friend FLAC__StreamDecoderSeekStatus	 FLACStreamDecoderSeekCallback(const FLAC__StreamDecoder *, FLAC__uint64, void *);
		friend FLAC__StreamDecoderTellStatus	 FLACStreamDecoderTellCallback(const FLAC__StreamDecoder *, FLAC__uint64 *, void *);
		friend FLAC__StreamDecoderLengthStatus	 FLACStreamDecoderLengthCallback(const FLAC__StreamDecoder *, FLAC__uint64 *, void *);
		friend FLAC__bool			 FLACStreamDecoderEofCallback(const FLAC__StreamDecoder *, void *);
		friend void				 FLACStreamDecoderMetadataCallback(const FLAC__StreamDecoder *, const FLAC__StreamMetadata *, void *);
		friend void				 FLACStreamDecoderErrorCallback(const FLAC__StreamDecoder *, FLAC__StreamDecoderErrorStatus, void *);

		private:
			Bool			 stop;
			Bool			 finished;

			Int64			 seekPosition;

			Buffer<signed int>	 samplesBuffer;

			Track			*infoTrack;

			Threads::Thread		*decoderThread;

			Threads::Semaphore	 samplesRequestedSignal;
			Threads::Semaphore	 samplesAvailableSignal;

			Int			 ReadFLAC(Bool);

			Bool			 SkipID3v2Tag(IO::InStream &);
		public:
			static const String	&GetComponentSpecs();

						 DecoderFLAC();
						~DecoderFLAC();

			Bool			 CanOpenStream(const String &);
			Error			 GetStreamInfo(const String &, Track &);

			Bool			 Activate();
			Bool			 Deactivate();

			Bool			 Seek(Int64);

			Int			 ReadData(Buffer<UnsignedByte> &);
	};
};

BoCA_DEFINE_DECODER_COMPONENT(DecoderFLAC)

BoCA_END_COMPONENT(DecoderFLAC)
