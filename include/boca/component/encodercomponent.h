 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_ENCODERCOMPONENT
#define H_BOCA_ENCODERCOMPONENT

#include <smooth.h>

using namespace smooth;

#include "component.h"
#include "../common/track/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT EncoderComponent : public Component, public IO::Filter
		{
			protected:
				Track		 track;
			public:
						 EncoderComponent();
				virtual		~EncoderComponent();

				virtual Bool	 SetAudioTrackInfo(const Track &);

				virtual String	 GetOutputFileExtension();

				virtual Bool	 Activate() = 0;
				virtual Bool	 Deactivate() = 0;

				virtual Int	 WriteData(Buffer<UnsignedByte> &, Int) = 0;
		};
	};
};

#endif
