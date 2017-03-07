 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

#include "lame/lame.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*lamedll;

Bool			 LoadLAMEDLL();
Void			 FreeLAMEDLL();

typedef hip_t			(CDECL *HIP_DECODE_INIT)	();
typedef int			(CDECL *HIP_DECODE_EXIT)	(hip_t);
typedef int			(CDECL *HIP_DECODE)		(hip_t, unsigned char *, size_t, short [], short []);
typedef int			(CDECL *HIP_DECODE_HEADERS)	(hip_t, unsigned char *, size_t, short [], short [], mp3data_struct *);
typedef char *			(CDECL *GET_LAME_SHORT_VERSION)	();

extern HIP_DECODE_INIT		 ex_hip_decode_init;
extern HIP_DECODE_EXIT		 ex_hip_decode_exit;
extern HIP_DECODE		 ex_hip_decode;
extern HIP_DECODE_HEADERS	 ex_hip_decode_headers;
extern GET_LAME_SHORT_VERSION	 ex_get_lame_short_version;
