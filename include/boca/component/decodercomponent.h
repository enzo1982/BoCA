 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_DECODERCOMPONENT
#define H_BOCA_DECODERCOMPONENT

#include <smooth.h>

using namespace smooth;

#include "component.h"
#include "../common/track/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT DecoderComponent : public Component, public IO::Filter
		{
			protected:
				Track		 track;

				Int64		 inBytes;
			public:
						 DecoderComponent();
				virtual		~DecoderComponent();

				virtual Bool	 CanOpenStream(const String &) = 0;

				virtual Error	 GetStreamInfo(const String &, Track &) = 0;
				virtual Error	 UpdateStreamInfo(const String &, const Track &);

				virtual Bool	 SetAudioTrackInfo(const Track &);

				Int64		 GetInBytes() { return inBytes; }

				virtual Bool	 Activate() = 0;
				virtual Bool	 Deactivate() = 0;

				virtual Int	 ReadData(Buffer<UnsignedByte> &, Int) = 0;
		};
	};
};

#endif
