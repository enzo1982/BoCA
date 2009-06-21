 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_TAG_ID3V2
#define H_BOCA_TAG_ID3V2

#include <smooth.h>
#include "tag.h"

using namespace smooth;

namespace BoCA
{
	class BOCA_DLL_EXPORT TagID3v2 : public Tag
	{
		private:
			static const String	 genres[148];
			static String		 dummyString;

			String			 GetFrameString(Void *, String & = dummyString);
			Int			 SetFrameString(Void *, const String &, const String & = NIL);

			String			 GetFrameURL(Void *);
			Int			 SetFrameURL(Void *, const String &);

			const String		&GetID3CategoryName(Int);
		public:
						 TagID3v2();
						~TagID3v2();

			Int			 Render(const Track &, Buffer<UnsignedByte> &);

			Int			 Parse(const Buffer<UnsignedByte> &, Track *);
			Int			 Parse(const String &, Track *);

			Int			 Update(const String &, const Track &);

	};
};

#endif
