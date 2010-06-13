 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_DSPCOMPONENT
#define H_BOCA_AS_DSPCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT DSPComponent : public Component
		{
			public:
						 DSPComponent(ComponentSpecs *);
				virtual		~DSPComponent();

				virtual Bool	 SetAudioTrackInfo(const Track &);
				virtual Void	 GetFormatInfo(Format &);

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Int	 TransformData(Buffer<UnsignedByte> &buffer, Int size);

				virtual Int	 Flush(Buffer<UnsignedByte> &buffer);

				virtual Int	 GetPackageSize();
		};
	};
};

#endif
