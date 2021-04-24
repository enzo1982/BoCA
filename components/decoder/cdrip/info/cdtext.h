 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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

			static String	 ReplaceFullWidthChars(const String &);
		public:
					 CDText();
					~CDText();

			Int		 ReadCDText(Int);

			const CDInfo	&GetCDInfo() const;
	};
};

#endif
