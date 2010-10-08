 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "sndfile/sndfile.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*sndfiledll;

Bool			 LoadSndFileDLL();
Void			 FreeSndFileDLL();

typedef SNDFILE *	(*SF_OPEN_FD)		(int, int, SF_INFO *, int);
typedef int		(*SF_CLOSE)		(SNDFILE *);
typedef sf_count_t	(*SF_SEEK)		(SNDFILE *, sf_count_t, int);
typedef sf_count_t	(*SF_READ_SHORT)	(SNDFILE *, short *, sf_count_t);
typedef sf_count_t	(*SF_READ_INT)		(SNDFILE *, int *, sf_count_t);
typedef const char *	(*SF_GET_STRING)	(SNDFILE *, int);

extern SF_OPEN_FD	 ex_sf_open_fd;
extern SF_CLOSE		 ex_sf_close;
extern SF_SEEK		 ex_sf_seek;
extern SF_READ_SHORT	 ex_sf_read_short;
extern SF_READ_INT	 ex_sf_read_int;
extern SF_GET_STRING	 ex_sf_get_string;
