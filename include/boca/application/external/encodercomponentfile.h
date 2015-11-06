 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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

				virtual Int	 WriteData(Buffer<UnsignedByte> &);
		};
	};
};

#endif
