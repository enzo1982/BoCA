 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_FORMAT
#define H_BOCA_FORMAT

#include <smooth.h>
#include "../../core/definitions.h"

using namespace smooth;

const Byte	 BYTE_INTEL	= 0;
const Byte	 BYTE_RAW	= 1;

namespace BoCA
{
	class BOCA_DLL_EXPORT Format
	{
		public:
			/* Audio format information:
			 */
			Int	 rate;
			Byte	 channels;
			Byte	 bits;
			Byte	 order;

			/* Class constructor / destructor:
			 */
				 Format();
				~Format();
	};
};

#endif
