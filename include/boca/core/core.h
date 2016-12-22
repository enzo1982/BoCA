 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2016 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
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
	BOCA_DLL_EXPORT Void		 Init(const String &);

	/* Should be called last, just before the BoCA library is unloaded.
	 */
	BOCA_DLL_EXPORT Void		 Free();

	/* Get the application prefix as passed to BoCA::Init().
	 */
	BOCA_DLL_EXPORT const String	&GetApplicationPrefix();
};

#endif
