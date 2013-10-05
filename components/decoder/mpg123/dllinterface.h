 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "mpg123/mpg123.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*mpg123dll;

Bool			 LoadMPG123DLL();
Void			 FreeMPG123DLL();

typedef int		(*MPG123_INIT)			();
typedef void		(*MPG123_EXIT)			();

typedef mpg123_handle *	(*MPG123_NEW)			(const char *, int *);
typedef void		(*MPG123_DELETE)		(mpg123_handle *);

typedef int		(*MPG123_OPEN_FEED)		(mpg123_handle *);
typedef int		(*MPG123_DECODE)		(mpg123_handle *, const unsigned char *, size_t, unsigned char *, size_t, size_t *);
typedef int		(*MPG123_FEED)			(mpg123_handle *, const unsigned char *, size_t);
typedef off_t		(*MPG123_FEEDSEEK)		(mpg123_handle *, off_t, int, off_t *);

typedef int		(*MPG123_GETFORMAT)		(mpg123_handle *, long *, int *, int *);
typedef int		(*MPG123_INFO)			(mpg123_handle *, mpg123_frameinfo *);
typedef int		(*MPG123_SPF)			(mpg123_handle *);

typedef const char **	(*MPG123_SUPPORTED_DECODERS)	();
typedef int		(*MPG123_DECODER)		(mpg123_handle *, const char *);

extern MPG123_INIT			 ex_mpg123_init;
extern MPG123_EXIT			 ex_mpg123_exit;

extern MPG123_NEW			 ex_mpg123_new;
extern MPG123_DELETE			 ex_mpg123_delete;

extern MPG123_OPEN_FEED			 ex_mpg123_open_feed;
extern MPG123_DECODE			 ex_mpg123_decode;
extern MPG123_FEED			 ex_mpg123_feed;
extern MPG123_FEEDSEEK			 ex_mpg123_feedseek;

extern MPG123_GETFORMAT			 ex_mpg123_getformat;
extern MPG123_INFO			 ex_mpg123_info;
extern MPG123_SPF			 ex_mpg123_spf;

extern MPG123_SUPPORTED_DECODERS	 ex_mpg123_supported_decoders;
extern MPG123_DECODER			 ex_mpg123_decoder;
