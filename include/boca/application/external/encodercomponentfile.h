 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_ENCODERCOMPONENTEXTERNALFILE
#define H_BOCA_ENCODERCOMPONENTEXTERNALFILE

#include "encodercomponent.h"

namespace BoCA
{
	namespace AS
	{
		class EncoderComponentExternalFile : public EncoderComponentExternal
		{
			private:
				IO::OutStream	*out;

				String		 wavFileName;
				String		 encFileName;

				Int		 nOfSamples;
			public:
						 EncoderComponentExternalFile(ComponentSpecs *);
				virtual		~EncoderComponentExternalFile();

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Int	 WriteData(Buffer<UnsignedByte> &, Int);
		};
	};
};

#endif
