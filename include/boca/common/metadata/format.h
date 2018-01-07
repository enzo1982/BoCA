 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_FORMAT
#define H_BOCA_FORMAT

#include <smooth.h>
#include "../../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	enum ByteOrder
	{
		BYTE_NATIVE = 0,
		BYTE_RAW,
		BYTE_INTEL,

		NUM_BYTE_ORDERS
	};

	class BOCA_DLL_EXPORT Format
	{
		public:
			/* Audio format information:
			 */
			Int		 rate;
			Short		 channels;

			Short		 bits;

			Bool		 fp;
			Bool		 sign;

			ByteOrder	 order;

			/* Class constructor / destructor:
			 */
					 Format(int = 0);
					~Format();

			Bool operator	 ==(const Format &) const;
			Bool operator	 !=(const Format &) const;
	};
};

#endif
