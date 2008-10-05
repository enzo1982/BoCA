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

#include "ogg/ogg.h"
#include "speex/speex.h"
#include "speex/speex_header.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*oggdll;
extern DynamicLoader	*speexdll;

Bool			 LoadOggDLL();
Void			 FreeOggDLL();

Bool			 LoadSpeexDLL();
Void			 FreeSpeexDLL();

typedef int			(*OGGSTREAMINIT)		 (ogg_stream_state *, int);
typedef int			(*OGGSTREAMPACKETIN)		 (ogg_stream_state *, ogg_packet *);
typedef int			(*OGGSTREAMFLUSH)		 (ogg_stream_state *, ogg_page *);
typedef int			(*OGGSTREAMPAGEOUT)		 (ogg_stream_state *, ogg_page *);
typedef int			(*OGGPAGEEOS)			 (ogg_page *);
typedef int			(*OGGSTREAMCLEAR)		 (ogg_stream_state *);

extern OGGSTREAMINIT		 ex_ogg_stream_init;
extern OGGSTREAMPACKETIN	 ex_ogg_stream_packetin;
extern OGGSTREAMFLUSH		 ex_ogg_stream_flush;
extern OGGSTREAMPAGEOUT		 ex_ogg_stream_pageout;
extern OGGPAGEEOS		 ex_ogg_page_eos;
extern OGGSTREAMCLEAR		 ex_ogg_stream_clear;

typedef void			(*SPEEXBITSINIT)		(SpeexBits *);
typedef void			(*SPEEXBITSDESTROY)		(SpeexBits *);
typedef void			(*SPEEXBITSRESET)		(SpeexBits *);
typedef int			(*SPEEXBITSNBYTES)		(SpeexBits *);
typedef void			(*SPEEXBITSINSERTTERMINATOR)	(SpeexBits *);
typedef int			(*SPEEXBITSWRITE)		(SpeexBits *, char *, int);
typedef void *			(*SPEEXENCODERINIT)		(const SpeexMode *);
typedef void			(*SPEEXENCODERDESTROY)		(void *);
typedef int			(*SPEEXENCODERCTL)		(void *, int, void *);
typedef int			(*SPEEXENCODEINT)		(void *, spx_int16_t *, SpeexBits *);
typedef void			(*SPEEXENCODESTEREOINT)		(spx_int16_t *, int, SpeexBits *); 
typedef void			(*SPEEXINITHEADER)		(SpeexHeader *, int, int, const struct SpeexMode *);
typedef char *			(*SPEEXHEADERTOPACKET)		(SpeexHeader *, int *);
typedef int			(*SPEEXLIBCTL)			(int, void *);
typedef const SpeexMode		(*SPEEXNBMODE)			;
typedef const SpeexMode		(*SPEEXWBMODE)			;
typedef const SpeexMode		(*SPEEXUWBMODE)			;

extern SPEEXBITSINIT		 ex_speex_bits_init;
extern SPEEXBITSDESTROY		 ex_speex_bits_destroy;
extern SPEEXBITSRESET		 ex_speex_bits_reset;
extern SPEEXBITSNBYTES		 ex_speex_bits_nbytes;
extern SPEEXBITSINSERTTERMINATOR ex_speex_bits_insert_terminator;
extern SPEEXBITSWRITE		 ex_speex_bits_write;
extern SPEEXENCODERINIT		 ex_speex_encoder_init;
extern SPEEXENCODERDESTROY	 ex_speex_encoder_destroy;
extern SPEEXENCODERCTL		 ex_speex_encoder_ctl;
extern SPEEXENCODEINT		 ex_speex_encode_int;
extern SPEEXENCODESTEREOINT	 ex_speex_encode_stereo_int;
extern SPEEXINITHEADER		 ex_speex_init_header;
extern SPEEXHEADERTOPACKET	 ex_speex_header_to_packet;
extern SPEEXLIBCTL		 ex_speex_lib_ctl;
extern SPEEXNBMODE		 ex_speex_nb_mode;
extern SPEEXWBMODE		 ex_speex_wb_mode;
extern SPEEXUWBMODE		 ex_speex_uwb_mode;
