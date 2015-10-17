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

BoCA_BEGIN_COMPONENT(EncoderMultiEncoderHub)

namespace BoCA
{
	class EncoderMultiEncoderHub : public CS::EncoderComponent
	{
		private:
			ConfigLayer			*configLayer;

			Array<IO::OutStream *>		 streams;
			Array<AS::EncoderComponent *>	 encoders;
		public:
			static const String		&GetComponentSpecs();

							 EncoderMultiEncoderHub();
							~EncoderMultiEncoderHub();

			Bool				 IsThreadSafe() const;

			Bool				 IsLossless() const;

			Bool				 Activate();
			Bool				 Deactivate();

			Int				 WriteData(Buffer<UnsignedByte> &);

			String				 GetOutputFileExtension() const;

			ConfigLayer			*GetConfigurationLayer();
	};
};

BoCA_DEFINE_ENCODER_COMPONENT(EncoderMultiEncoderHub)

BoCA_END_COMPONENT(EncoderMultiEncoderHub)
