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

BoCA_BEGIN_COMPONENT(EncoderFLAC)

namespace BoCA
{
	class EncoderFLAC : public CS::EncoderComponent
	{
		friend FLAC__StreamEncoderReadStatus	 FLACStreamEncoderReadCallback(const FLAC__StreamEncoder *, FLAC__byte[], size_t *, void *);
		friend FLAC__StreamEncoderWriteStatus	 FLACStreamEncoderWriteCallback(const FLAC__StreamEncoder *, const FLAC__byte[], size_t, uint32_t, uint32_t, void *);
		friend FLAC__StreamEncoderSeekStatus	 FLACStreamEncoderSeekCallback(const FLAC__StreamEncoder *, FLAC__uint64, void *);
		friend FLAC__StreamEncoderTellStatus	 FLACStreamEncoderTellCallback(const FLAC__StreamEncoder *, FLAC__uint64 *, void *);

		private:
			ConfigLayer				*configLayer;
			Config					*config;

			FLAC__StreamEncoder			*encoder;

			Array<FLAC__StreamMetadata *, Void *>	 metadata;

			Buffer<FLAC__int32>			 buffer;
			Int					 bytesWritten;

			Bool					 FixChapterMarks();

			static Bool				 ConvertArguments(Config *);
		public:
			static const String			&GetComponentSpecs();

								 EncoderFLAC();
								~EncoderFLAC();

			Bool					 Activate();
			Bool					 Deactivate();

			Int					 WriteData(Buffer<UnsignedByte> &);

			Bool					 SetOutputFormat(Int);
			String					 GetOutputFileExtension() const;

			ConfigLayer				*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderFLAC)

BoCA_END_COMPONENT(EncoderFLAC)
