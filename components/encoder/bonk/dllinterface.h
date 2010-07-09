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

#include "bonk/bonk.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*bonkdll;

Bool			 LoadBonkDLL();
Void			 FreeBonkDLL();

typedef void *				(BONKCONV *BONKENCODERCREATE)			();
typedef bool				(BONKCONV *BONKENCODERINIT)			(void *, unsigned int, unsigned int, int, bool, bool, int, int, int, double);
typedef int				(BONKCONV *BONKENCODERENCODEPACKET)		(void *, signed short *, int, unsigned char *, int);
typedef int				(BONKCONV *BONKENCODERFINISH)			(void *, unsigned char *, int);
typedef bool				(BONKCONV *BONKENCODERCLOSE)			(void *);
typedef int				(BONKCONV *BONKENCODERGETSAMPLECOUNT)		(void *);
typedef int				(BONKCONV *BONKENCODERGETSAMPLECOUNTOFFSET)	(void *);
typedef bool				(BONKCONV *BONKENCODERSETID3DATA)		(void *, unsigned char *, int);
typedef const char *			(BONKCONV *BONKGETVERSIONSTRING)		();

extern BONKENCODERCREATE		 ex_bonk_encoder_create;
extern BONKENCODERINIT			 ex_bonk_encoder_init;
extern BONKENCODERENCODEPACKET		 ex_bonk_encoder_encode_packet;
extern BONKENCODERFINISH		 ex_bonk_encoder_finish;
extern BONKENCODERCLOSE			 ex_bonk_encoder_close;
extern BONKENCODERGETSAMPLECOUNT	 ex_bonk_encoder_get_sample_count;
extern BONKENCODERGETSAMPLECOUNTOFFSET	 ex_bonk_encoder_get_sample_count_offset;
extern BONKENCODERSETID3DATA		 ex_bonk_encoder_set_id3_data;
extern BONKGETVERSIONSTRING		 ex_bonk_get_version_string;
