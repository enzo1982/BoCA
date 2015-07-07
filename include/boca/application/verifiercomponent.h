 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_VERIFIERCOMPONENT
#define H_BOCA_AS_VERIFIERCOMPONENT

#include "component.h"
#include "../common/metadata/track.h"

using namespace smooth::GUI;

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT VerifierComponent : public Component
		{
			public:
						 VerifierComponent(ComponentSpecs *);
				virtual		~VerifierComponent();

				virtual Bool	 CanVerifyTrack(const Track &);

				virtual Bool	 SetAudioTrackInfo(const Track &);

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Int	 ProcessData(Buffer<UnsignedByte> &);

				virtual Bool	 Verify();
		};
	};
};

#endif
