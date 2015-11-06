 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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

#include "sndfile/sndfile.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*sndfiledll;

Bool			 LoadSndFileDLL();
Void			 FreeSndFileDLL();

typedef int		(*SF_FORMAT_CHECK)	(const SF_INFO *);
typedef SNDFILE *	(*SF_OPEN_VIRTUAL)	(SF_VIRTUAL_IO *, int, SF_INFO *, void *);
typedef int		(*SF_CLOSE)		(SNDFILE *);
typedef sf_count_t	(*SF_WRITE_SHORT)	(SNDFILE *, short *, sf_count_t);
typedef sf_count_t	(*SF_WRITE_INT)		(SNDFILE *, int *, sf_count_t);
typedef int		(*SF_SET_STRING)	(SNDFILE *, int, const char *);
typedef int		(*SF_COMMAND)		(SNDFILE *, int, void *, int);
typedef const char *	(*SF_STRERROR)		(SNDFILE *);

extern SF_FORMAT_CHECK	 ex_sf_format_check;
extern SF_OPEN_VIRTUAL	 ex_sf_open_virtual;
extern SF_CLOSE		 ex_sf_close;
extern SF_WRITE_SHORT	 ex_sf_write_short;
extern SF_WRITE_INT	 ex_sf_write_int;
extern SF_SET_STRING	 ex_sf_set_string;
extern SF_COMMAND	 ex_sf_command;
extern SF_STRERROR	 ex_sf_strerror;
