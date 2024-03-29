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

#ifndef H_BOCA_PICTURE
#define H_BOCA_PICTURE

#include "picturedata.h"

using namespace smooth;
using namespace smooth::GUI;

namespace BoCA
{
	class BOCA_DLL_EXPORT Picture
	{
		public:
			S::Byte			 type;

			String			 mime;
			String			 description;

			PictureData		 data;

						 Picture();
						 Picture(const Picture &);
						~Picture();

			Picture &operator	 =(const Picture &);

			Bool operator		 ==(const Picture &) const;
			Bool operator		 !=(const Picture &) const;

			Int			 LoadFromFile(const String &);
			Int			 SaveToFile(const String &) const;

			Bitmap			 GetBitmap() const;
	};
};

#endif
