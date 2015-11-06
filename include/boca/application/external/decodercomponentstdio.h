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

#ifndef H_BOCA_DECODERCOMPONENTEXTERNALSTDIO
#define H_BOCA_DECODERCOMPONENTEXTERNALSTDIO

#include "decodercomponent.h"

#ifdef __WIN32__
#	include <windows.h>
#endif

namespace BoCA
{
	namespace AS
	{
		class DecoderComponentExternalStdIO : public DecoderComponentExternal
		{
			private:
#ifdef __WIN32__
				HANDLE		 rPipe;
				HANDLE		 wPipe;

				HANDLE		 hProcess;
#else
				FILE		*rPipe;
#endif

				String		 encFileName;

				Int64		 samplesRead;

				String		 GetMD5(const String &);
			public:
						 DecoderComponentExternalStdIO(ComponentSpecs *);
				virtual		~DecoderComponentExternalStdIO();

				virtual Error	 GetStreamInfo(const String &, Track &);

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Bool	 Seek(Int64);

				virtual Int	 ReadData(Buffer<UnsignedByte> &);
		};
	};
};

#endif
