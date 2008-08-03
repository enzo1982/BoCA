 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
			Int		 version;

			Int		 ParseID3Tag(Void *, Track *);

			String		 GetID3v2FrameString(ID3Frame *);
			Int		 SetID3v2FrameString(ID3Frame *, const String &);

			const String	&GetID3CategoryName(Int);

			String		 GetTempFileName(const String &);

			String		 CreateTempFile(const String &);
			Bool		 RemoveTempFile(const String &);
		public:
					 TagID3();
					~TagID3();

			Int		 Render(const Track &, Buffer<UnsignedByte> &);

			Int		 Parse(Buffer<UnsignedByte> &, Track *);
			Int		 Parse(const String &, Track *);
		accessors:
			Bool		 SetID3Version(Int nVersion)	{ if (version < 1 || version > 2) return False; version = nVersion; return True; }
	};
};

#endif
