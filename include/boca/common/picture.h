 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_PICTURE
#define H_BOCA_PICTURE

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;
using namespace smooth::GUI;

namespace BoCA
{
	class BOCA_DLL_EXPORT Picture
	{
		public:

			Int			 type;
			String			 mime;
			String			 description;
			Buffer<unsigned char>	 data;

						 Picture(int = 0);
						 Picture(const Picture &);
						~Picture();

			Picture &operator	 =(const Picture &);

			Int			 LoadFromFile(const String &);
			Int			 SaveToFile(const String &) const;

			const Bitmap		&GetBitmap() const;
	};
};

#endif
