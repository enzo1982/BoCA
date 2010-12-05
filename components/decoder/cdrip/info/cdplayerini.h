 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_CDRIP_CDPLAYER
#define H_CDRIP_CDPLAYER

#include <smooth.h>

using namespace smooth;

#include "cdinfo.h"

namespace BoCA
{
	class CDPlayerIni
	{
		private:
			CDInfo		 cdInfo;

			String		 DiscIDToString(Int);
		public:
					 CDPlayerIni();
					~CDPlayerIni();

			Int		 ReadCDInfo();

			Int		 ClearCDInfo();
			const CDInfo	&GetCDInfo();
	};
};

#endif
