 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_CONFIGLAYER
#define H_BOCA_CONFIGLAYER

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;
using namespace smooth::GUI;

namespace BoCA
{
	abstract class BOCA_DLL_EXPORT ConfigLayer : public Layer
	{
		public:
					 ConfigLayer();
			virtual		~ConfigLayer();

			virtual Int	 SaveSettings() = 0;
	};
};

#endif
