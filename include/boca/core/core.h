 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_CORE
#define H_BOCA_CORE

#include <smooth.h>
#include "../core/definitions.h"

using namespace smooth;

namespace BoCA
{
	/* Should be called first, just after the BoCA library is loaded.
	 */
	BOCA_DLL_EXPORT Void	 Init();

	/* Should be called last, just before the BoCA library is unloaded.
	 */
	BOCA_DLL_EXPORT Void	 Free();
};

#endif
