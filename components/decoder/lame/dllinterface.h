 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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

typedef int				(*LAME_DECODE_INIT)				();
typedef int				(*LAME_DECODE_EXIT)				();
typedef int				(*LAME_DECODE)					(unsigned char *, int, short [], short []);
typedef int				(*LAME_DECODE_HEADERS)				(unsigned char *, int, short [], short [], mp3data_struct *);
typedef char *				(*GET_LAME_SHORT_VERSION)			();

extern LAME_DECODE_INIT			 ex_lame_decode_init;
extern LAME_DECODE_EXIT			 ex_lame_decode_exit;
extern LAME_DECODE			 ex_lame_decode;
extern LAME_DECODE_HEADERS		 ex_lame_decode_headers;
extern GET_LAME_SHORT_VERSION		 ex_get_lame_short_version;
