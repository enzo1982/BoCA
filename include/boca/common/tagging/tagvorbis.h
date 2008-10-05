 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_TAG_VORBIS
#define H_BOCA_TAG_VORBIS

#include <smooth.h>
#include "tag.h"

using namespace smooth;

namespace BoCA
{
	class TagVorbis : public Tag
	{
		private:
			Int		 RenderTagHeader(const String &, Int, Buffer<UnsignedByte> &);
			Int		 RenderTagItem(const String &, const String &, Buffer<UnsignedByte> &);
		public:
					 TagVorbis();
					~TagVorbis();

			Int		 Render(const Track &, Buffer<UnsignedByte> &, const String &);

			Int		 Parse(const String &, Track *);
	};
};

#endif
