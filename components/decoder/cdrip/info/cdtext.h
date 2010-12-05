 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_CDRIP_CDTEXT
#define H_CDRIP_CDTEXT

#include <smooth.h>

using namespace smooth;

#include "cdinfo.h"

namespace BoCA
{
	class CDText
	{
		private:
			CDInfo		 cdInfo;
		public:
					 CDText();
					~CDText();

			Int		 ReadCDText();

			Int		 ClearCDInfo();
			const CDInfo	&GetCDInfo();
	};
};

#endif
