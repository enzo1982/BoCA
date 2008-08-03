 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_TAG_APE
#define H_BOCA_TAG_APE

#include <smooth.h>
#include "tag.h"

using namespace smooth;

namespace BoCA
{
	class TagAPE : public Tag
	{
		private:
			Int		 RenderAPEHeader(Int, Int, Buffer<UnsignedByte> &);
			Int		 RenderAPEFooter(Int, Int, Buffer<UnsignedByte> &);
			Int		 RenderAPEItem(const String &, const String &, Buffer<UnsignedByte> &);

			Bool		 ParseAPEHeader(Buffer<UnsignedByte> &, Int *, Int *);
			Bool		 ParseAPEFooter(Buffer<UnsignedByte> &, Int *, Int *);
			Bool		 ParseAPEItem(Buffer<UnsignedByte> &, Int &, String *, String *);

		public:
					 TagAPE();
					~TagAPE();

			Int		 Render(const Track &, Buffer<UnsignedByte> &);

			Int		 Parse(Buffer<UnsignedByte> &, Track *);
			Int		 Parse(const String &, Track *);
	};
};

#endif
