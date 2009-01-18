 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_DSPCOMPONENT
#define H_BOCA_DSPCOMPONENT

#include <smooth.h>

using namespace smooth;

#include "component.h"
#include "../common/track/track.h"

namespace BoCA
{
	namespace CS
	{
		abstract class BOCA_DLL_EXPORT DSPComponent : public Component
		{
			protected:
				Track		 track;
				Format		 format;
			public:
						 DSPComponent();
				virtual		~DSPComponent();

				virtual Bool	 SetAudioTrackInfo(const Track &);
				virtual Void	 GetFormatInfo(Format &);

				virtual Bool	 Activate() = 0;
				virtual Bool	 Deactivate() = 0;

				virtual Int	 TransformData(Buffer<UnsignedByte> &, Int) = 0;

				virtual Int	 Flush(Buffer<UnsignedByte> &);

				virtual Int	 GetPackageSize();
		};
	};
};

#endif
