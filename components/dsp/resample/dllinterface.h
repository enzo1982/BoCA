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

#include "samplerate/samplerate.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*srcdll;

Bool			 LoadSRCDLL();
Void			 FreeSRCDLL();

typedef SRC_STATE *	(*SRC_NEW)		(int, int, int *);
typedef SRC_STATE *	(*SRC_DELETE)		(SRC_STATE *);
typedef int		(*SRC_PROCESS)		(SRC_STATE *, SRC_DATA *);
typedef const char *	(*SRC_GET_NAME)		(int);
typedef const char *	(*SRC_GET_DESCRIPTION)	(int);
typedef const char *	(*SRC_STRERROR)		(int);

extern SRC_NEW			 ex_src_new;
extern SRC_DELETE		 ex_src_delete;
extern SRC_PROCESS		 ex_src_process;
extern SRC_GET_NAME		 ex_src_get_name;
extern SRC_GET_DESCRIPTION	 ex_src_get_description;
extern SRC_STRERROR		 ex_src_strerror;
