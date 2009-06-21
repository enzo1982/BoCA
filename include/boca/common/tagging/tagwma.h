 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_TAG_WMA
#define H_BOCA_TAG_WMA

#include <smooth.h>
#include "tag.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT TagWMA : public Tag
	{
		public:
					 TagWMA();
					~TagWMA();

			Int		 Render(const Track &, const String &);

			Int		 Parse(const String &, Track *);

			Int		 Update(const String &, const Track &);
	};
};

#endif
