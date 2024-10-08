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

#include <ogg/ogg.h>
#include <speex/speex.h>
#include <speex/speex_header.h>
#include <speex/speex_stereo.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*oggdll;
extern DynamicLoader	*speexdll;

Bool			 LoadOggDLL();
Void			 FreeOggDLL();

Bool			 LoadSpeexDLL();
Void			 FreeSpeexDLL();

typedef int			(*OGGSTREAMINIT)		(ogg_stream_state *, int);
typedef int			(*OGGSTREAMPACKETOUT)		(ogg_stream_state *, ogg_packet *);
typedef int			(*OGGSTREAMPAGEIN)		(ogg_stream_state *, ogg_page *);
typedef int			(*OGGPAGEEOS)			(ogg_page *);
typedef int			(*OGGPAGESERIALNO)		(ogg_page *);
typedef ogg_int64_t		(*OGGPAGEGRANULEPOS)		(ogg_page *);
typedef int			(*OGGPAGEPACKETS)		(ogg_page *);
typedef int			(*OGGSTREAMCLEAR)		(ogg_stream_state *);
typedef int			(*OGGSYNCINIT)			(ogg_sync_state *);
typedef char *			(*OGGSYNCBUFFER)		(ogg_sync_state *, int);
typedef int			(*OGGSYNCWROTE)			(ogg_sync_state *, int);
typedef int			(*OGGSYNCPAGEOUT)		(ogg_sync_state *, ogg_page *);
typedef int			(*OGGSYNCPAGESEEK)		(ogg_sync_state *, ogg_page *);
typedef int			(*OGGSYNCRESET)			(ogg_sync_state *);
typedef int			(*OGGSYNCCLEAR)			(ogg_sync_state *);

extern OGGSTREAMINIT		 ex_ogg_stream_init;
extern OGGSTREAMPACKETOUT	 ex_ogg_stream_packetout;
extern OGGSTREAMPAGEIN		 ex_ogg_stream_pagein;
extern OGGPAGEEOS		 ex_ogg_page_eos;
extern OGGPAGESERIALNO		 ex_ogg_page_serialno;
extern OGGPAGEGRANULEPOS	 ex_ogg_page_granulepos;
extern OGGPAGEPACKETS		 ex_ogg_page_packets;
extern OGGSTREAMCLEAR		 ex_ogg_stream_clear;
extern OGGSYNCINIT		 ex_ogg_sync_init;
extern OGGSYNCBUFFER		 ex_ogg_sync_buffer;
extern OGGSYNCWROTE		 ex_ogg_sync_wrote;
extern OGGSYNCPAGEOUT		 ex_ogg_sync_pageout;
extern OGGSYNCPAGESEEK		 ex_ogg_sync_pageseek;
extern OGGSYNCRESET		 ex_ogg_sync_reset;
extern OGGSYNCCLEAR		 ex_ogg_sync_clear;

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
