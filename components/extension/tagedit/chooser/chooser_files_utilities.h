 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_TAGEDIT_CHOOSER_FILES_UTILITIES_
#define _H_TAGEDIT_CHOOSER_FILES_UTILITIES_

#include <smooth.h>

/* Undefine this in order to use BoCA application classes.
 */
#undef __BOCA_COMPONENT_BUILD__

#include <boca.h>

using namespace smooth;
using namespace smooth::GUI;

using namespace BoCA;
using namespace BoCA::AS;

namespace BoCA
{
	class ChooserFilesUtilities
	{
		public:
			static DecoderComponent	*CreateDecoderComponent(const String &);
	};
};

#endif
