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

#include <bonk/bonk.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*bonkdll;

Bool			 LoadBonkDLL();
Void			 FreeBonkDLL();

typedef void *				(BONKCONV *BONKDECODERCREATE)			();
typedef bool				(BONKCONV *BONKDECODERINIT)			(void *, unsigned char *, int, unsigned int *, unsigned int *, int *);
typedef int				(BONKCONV *BONKDECODERDECODEPACKET)		(void *, unsigned char *, int, signed short *, int);
typedef int				(BONKCONV *BONKDECODERFINISH)			(void *);
typedef bool				(BONKCONV *BONKDECODERCLOSE)			(void *);
typedef bool				(BONKCONV *BONKDECODERGETSTREAMINFO)		(void *, bool *, bool *);
typedef bool				(BONKCONV *BONKDECODERGETID3DATA)		(void *, unsigned char **, int *);
typedef bool				(BONKCONV *BONKDECODERINITSEEKTABLE)		(void *, unsigned char *, int);
typedef bool				(BONKCONV *BONKDECODERSEEKTO)			(void *, int);
typedef const char *			(BONKCONV *BONKGETVERSIONSTRING)		();

extern BONKDECODERCREATE		 ex_bonk_decoder_create;
extern BONKDECODERINIT			 ex_bonk_decoder_init;
extern BONKDECODERDECODEPACKET		 ex_bonk_decoder_decode_packet;
extern BONKDECODERFINISH		 ex_bonk_decoder_finish;
extern BONKDECODERCLOSE			 ex_bonk_decoder_close;
extern BONKDECODERGETSTREAMINFO		 ex_bonk_decoder_get_stream_info;
extern BONKDECODERGETID3DATA		 ex_bonk_decoder_get_id3_data;
extern BONKDECODERINITSEEKTABLE		 ex_bonk_decoder_init_seektable;
extern BONKDECODERSEEKTO		 ex_bonk_decoder_seek_to;
extern BONKGETVERSIONSTRING		 ex_bonk_get_version_string;
