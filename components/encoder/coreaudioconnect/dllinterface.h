 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#ifdef callbacks
#	undef callbacks
#endif

#include "mp4v2/mp4v2.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*mp4v2dll;

Bool			 LoadMP4v2DLL();
Void			 FreeMP4v2DLL();

typedef bool	(*MP4OPTIMIZE)(const char *, const char *);

extern MP4OPTIMIZE	 ex_MP4Optimize;
