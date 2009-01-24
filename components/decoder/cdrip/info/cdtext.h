 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

using namespace smooth;

namespace BoCA
{
	class CDText
	{
		private:
			Array<String>		 cdText;
		public:
						 CDText();
						~CDText();

			Int			 ReadCDText();
			Int			 ClearCDText();

			const Array<String>	&GetCDText();
	};
};
