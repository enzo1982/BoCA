 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_DECODERCOMPONENTEXTERNALSTDIO
#define H_BOCA_DECODERCOMPONENTEXTERNALSTDIO

#include "decodercomponent.h"

namespace BoCA
{
	namespace AS
	{
		class DecoderComponentExternalStdIO : public DecoderComponentExternal
		{
			private:
				HANDLE		 rPipe;
				HANDLE		 wPipe;

				HANDLE		 hProcess;

				String		 encFileName;
			public:
						 DecoderComponentExternalStdIO(ComponentSpecs *);
				virtual		~DecoderComponentExternalStdIO();

				virtual Error	 GetStreamInfo(const String &, Track &);

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Bool	 Seek(Int64);

				virtual Int	 ReadData(Buffer<UnsignedByte> &, Int);
		};
	};
};

#endif
