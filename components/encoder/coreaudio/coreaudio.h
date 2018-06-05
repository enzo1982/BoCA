 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

BoCA_BEGIN_COMPONENT(EncoderCoreAudio)

namespace BoCA
{
	class EncoderCoreAudio : public CS::EncoderComponent
	{
		friend CA::OSStatus	 AudioConverterComplexInputDataProc(CA::AudioConverterRef, CA::UInt32 *, CA::AudioBufferList *, CA::AudioStreamPacketDescription **, void *);

		private:
			ConfigLayer		*configLayer;

			CA::AudioFileID		 audioFile;
			CA::AudioConverterRef	 converter;

			CA::UInt32		 fileType;

			CA::AudioBufferList	*buffers;

			Buffer<unsigned char>	 buffer;
			CA::UInt32		 bufferSize;
			Int			 bytesConsumed;

			Buffer<unsigned char>	 suppliedData;

			Int			 packetsWritten;
			Int			 totalSamples;

			Int			 EncodeFrames(Bool);

			Int			 GetOutputSampleRate(Int) const;
		public:
			static const String	&GetComponentSpecs();

						 EncoderCoreAudio();
						~EncoderCoreAudio();

			Bool			 IsLossless() const;

			Bool			 Activate();
			Bool			 Deactivate();

			Int			 WriteData(Buffer<UnsignedByte> &);

			Bool			 SetOutputFormat(Int);
			String			 GetOutputFileExtension() const;

			ConfigLayer		*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderCoreAudio)

BoCA_END_COMPONENT(EncoderCoreAudio)
