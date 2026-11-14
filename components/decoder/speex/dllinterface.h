 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
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

#include <speex/speex.h>
#include <speex/speex_header.h>
#include <speex/speex_stereo.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*speexdll;

Bool			 LoadSpeexDLL();
Void			 FreeSpeexDLL();

typedef void			(*SPEEXBITSINIT)		(SpeexBits *);
typedef void			(*SPEEXBITSDESTROY)		(SpeexBits *);
typedef void			(*SPEEXBITSREADFROM)		(SpeexBits *, char *, int);
typedef void *			(*SPEEXDECODERINIT)		(const SpeexMode *);
typedef void			(*SPEEXDECODERDESTROY)		(void *);
typedef int			(*SPEEXDECODERCTL)		(void *, int, void *);
typedef int			(*SPEEXDECODEINT)		(void *, SpeexBits *, spx_int16_t *);
typedef void			(*SPEEXDECODESTEREOINT)		(spx_int16_t *, int, SpeexStereoState *);
typedef SpeexHeader *		(*SPEEXPACKETTOHEADER)		(char *, int);
typedef int			(*SPEEXLIBCTL)			(int, void *);
typedef const SpeexMode *	(*SPEEXLIBGETMODE)		(int);

extern SPEEXBITSINIT		 ex_speex_bits_init;
extern SPEEXBITSDESTROY		 ex_speex_bits_destroy;
extern SPEEXBITSREADFROM	 ex_speex_bits_read_from;
extern SPEEXDECODERINIT		 ex_speex_decoder_init;
extern SPEEXDECODERDESTROY	 ex_speex_decoder_destroy;
extern SPEEXDECODERCTL		 ex_speex_decoder_ctl;
extern SPEEXDECODEINT		 ex_speex_decode_int;
extern SPEEXDECODESTEREOINT	 ex_speex_decode_stereo_int;
extern SPEEXPACKETTOHEADER	 ex_speex_packet_to_header;
extern SPEEXLIBCTL		 ex_speex_lib_ctl;
extern SPEEXLIBGETMODE		 ex_speex_lib_get_mode;
