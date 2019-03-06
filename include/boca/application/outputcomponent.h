 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_AS_OUTPUTCOMPONENT
#define H_BOCA_AS_OUTPUTCOMPONENT

#include "streamcomponent.h"

namespace BoCA
{
	namespace AS
	{
		class BOCA_DLL_EXPORT OutputComponent : public StreamComponent
		{
			public:
						 OutputComponent(ComponentSpecs *);
				virtual		~OutputComponent();

				virtual Int	 WriteData(const Buffer<UnsignedByte> &);

				virtual Bool	 Finish();

				virtual Int	 CanWrite();

				virtual Int	 SetPause(Bool);
				virtual Bool	 IsPlaying();
		};
	};
};

#endif
