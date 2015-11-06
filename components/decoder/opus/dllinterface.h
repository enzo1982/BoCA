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

#include <ogg/ogg.h>
#include <opus/opus.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*oggdll;
extern DynamicLoader	*opusdll;

Bool			 LoadOggDLL();
Void			 FreeOggDLL();

Bool			 LoadOpusDLL();
Void			 FreeOpusDLL();

typedef int			(*OGGSTREAMINIT)	(ogg_stream_state *, int);
typedef int			(*OGGSTREAMPACKETOUT)	(ogg_stream_state *, ogg_packet *);
typedef int			(*OGGSTREAMPAGEIN)	(ogg_stream_state *, ogg_page *);
typedef int			(*OGGPAGEEOS)		(ogg_page *);
typedef int			(*OGGPAGESERIALNO)	(ogg_page *);
typedef ogg_int64_t		(*OGGPAGEGRANULEPOS)	(ogg_page *);
typedef int			(*OGGSTREAMCLEAR)	(ogg_stream_state *);
typedef int			(*OGGSYNCINIT)		(ogg_sync_state *);
typedef char *			(*OGGSYNCBUFFER)	(ogg_sync_state *, int);
typedef int			(*OGGSYNCWROTE)		(ogg_sync_state *, int);
typedef int			(*OGGSYNCPAGEOUT)	(ogg_sync_state *, ogg_page *);
typedef int			(*OGGSYNCPAGESEEK)	(ogg_sync_state *, ogg_page *);
typedef int			(*OGGSYNCRESET)		(ogg_sync_state *);
typedef int			(*OGGSYNCCLEAR)		(ogg_sync_state *);

extern OGGSTREAMINIT		 ex_ogg_stream_init;
extern OGGSTREAMPACKETOUT	 ex_ogg_stream_packetout;
extern OGGSTREAMPAGEIN		 ex_ogg_stream_pagein;
extern OGGPAGEEOS		 ex_ogg_page_eos;
extern OGGPAGESERIALNO		 ex_ogg_page_serialno;
extern OGGPAGEGRANULEPOS	 ex_ogg_page_granulepos;
extern OGGSTREAMCLEAR		 ex_ogg_stream_clear;
extern OGGSYNCINIT		 ex_ogg_sync_init;
extern OGGSYNCBUFFER		 ex_ogg_sync_buffer;
extern OGGSYNCWROTE		 ex_ogg_sync_wrote;
extern OGGSYNCPAGEOUT		 ex_ogg_sync_pageout;
extern OGGSYNCPAGESEEK		 ex_ogg_sync_pageseek;
extern OGGSYNCRESET		 ex_ogg_sync_reset;
extern OGGSYNCCLEAR		 ex_ogg_sync_clear;

typedef OpusDecoder *		(*OPUSDECODERCREATE)	 (opus_int32, int, int *);
typedef int			(*OPUSDECODE)		 (OpusDecoder *, const unsigned char *, opus_int32, opus_int16 *, int, int);
typedef int			(*OPUSDECODERCTL)	 (OpusDecoder *, int, ...);
typedef void			(*OPUSDECODERDESTROY)	 (OpusDecoder *);

extern OPUSDECODERCREATE	 ex_opus_decoder_create;
extern OPUSDECODE		 ex_opus_decode;
extern OPUSDECODERCTL		 ex_opus_decoder_ctl;
extern OPUSDECODERDESTROY	 ex_opus_decoder_destroy;
