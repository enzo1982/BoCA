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
	class CDPlayerIni
	{
		private:
			Array<String>		 cdInfo;

			String			 DiscIDToString(Int);
		public:
						 CDPlayerIni();
						~CDPlayerIni();

			Int			 ReadCDInfo();
			Int			 ClearCDInfo();

			const Array<String>	&GetCDInfo();
	};
};
