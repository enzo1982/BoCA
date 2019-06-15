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

#include <boca.h>

namespace BoCA
{
	class LengthDisplay : public Widget
	{
		private:
			Bitmap	 bitmap;
			Bitmap	 bitmapScaled;
		public:
			Int	 Paint(Int);

				 LengthDisplay(const Bitmap &);
				~LengthDisplay();
		accessors:
			Int	 SetText(const String &);
	};
};
