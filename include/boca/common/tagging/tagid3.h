 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_TAG_ID3
#define H_BOCA_TAG_ID3

#include <smooth.h>
#include "tag.h"
#include "../../core/dllinterfaces.h"

using namespace smooth;

namespace BoCA
{
	class TagID3 : public Tag
	{
		private:
			static String	 dummyString;

			Int		 version;

			Int		 ParseID3Tag(Void *, Track *);

			String		 GetID3v2FrameString(ID3Frame *, String & = dummyString);
			Int		 SetID3v2FrameString(ID3Frame *, const String &, const String & = NIL);

			const String	&GetID3CategoryName(Int);
		public:
					 TagID3();
					~TagID3();

			Int		 Render(const Track &, Buffer<UnsignedByte> &);

			Int		 Parse(const Buffer<UnsignedByte> &, Track *);
			Int		 Parse(const String &, Track *);
		accessors:
			Bool		 SetID3Version(Int nVersion)	{ if (version < 1 || version > 2) return False; version = nVersion; return True; }
	};
};

#endif
