 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
#include "../track.h"

using namespace smooth;

namespace BoCA
{
	class Tag
	{
		public:
					 Tag();
			virtual		~Tag();

			virtual Int	 Render(const Track &, Buffer<UnsignedByte> &);

			virtual Int	 ParseBuffer(Buffer<UnsignedByte> &, Track *);
			virtual Int	 ParseFile(const String &, Track *);
	};
};

#endif
