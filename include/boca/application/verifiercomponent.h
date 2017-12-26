 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_VERIFIERCOMPONENT
#define H_BOCA_AS_VERIFIERCOMPONENT

#include "streamcomponent.h"

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT VerifierComponent : public StreamComponent
		{
			public:
						 VerifierComponent(ComponentSpecs *);
				virtual		~VerifierComponent();

				virtual Bool	 CanVerifyTrack(const Track &);

				virtual Bool	 Activate();
				virtual Bool	 Deactivate();

				virtual Int	 ProcessData(Buffer<UnsignedByte> &);

				virtual Bool	 Verify();
		};
	};
};

#endif
