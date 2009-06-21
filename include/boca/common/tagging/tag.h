 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_TAG
#define H_BOCA_TAG

#include <smooth.h>
#include "../track/track.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT Tag
	{
		public:
					 Tag();
			virtual		~Tag();

			virtual Int	 Render(const Track &, Buffer<UnsignedByte> &);
			virtual Int	 Render(const Track &, const String &);

			virtual Int	 Parse(const Buffer<UnsignedByte> &, Track *);
			virtual Int	 Parse(const String &, Track *);

			virtual Int	 Update(Buffer<UnsignedByte> &, const Track &);
			virtual Int	 Update(const String &, const Track &);
	};
};

#endif
