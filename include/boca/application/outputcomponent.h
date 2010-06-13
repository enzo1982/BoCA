 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_OUTPUTCOMPONENT
#define H_BOCA_AS_OUTPUTCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT OutputComponent : public Component, public IO::Filter
		{
			public:
						 OutputComponent(ComponentSpecs *);
				virtual		~OutputComponent();

				virtual Bool	 SetAudioTrackInfo(const Track &);

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Int	 WriteData(Buffer<UnsignedByte> &buffer, Int size);

				virtual Int	 GetPackageSize();

				virtual Int	 CanWrite();

				virtual Int	 SetPause(Bool);
				virtual Bool	 IsPlaying();
		};
	};
};

#endif
