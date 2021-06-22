 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*oggdll;

Bool			 LoadOggDLL();
Void			 FreeOggDLL();

/* Ogg API functions.
 */
typedef int			(*OGGSTREAMINIT)	(ogg_stream_state *, int);
typedef int			(*OGGSTREAMPACKETIN)	(ogg_stream_state *, ogg_packet *);
typedef int			(*OGGSTREAMPACKETOUT)	(ogg_stream_state *, ogg_packet *);
typedef int			(*OGGSTREAMFLUSH)	(ogg_stream_state *, ogg_page *);
typedef int			(*OGGSTREAMPAGEIN)	(ogg_stream_state *, ogg_page *);
typedef int			(*OGGPAGESERIALNO)	(ogg_page *);
typedef int			(*OGGSTREAMCLEAR)	(ogg_stream_state *);
typedef int			(*OGGSYNCINIT)		(ogg_sync_state *);
typedef char *			(*OGGSYNCBUFFER)	(ogg_sync_state *, int);
typedef int			(*OGGSYNCWROTE)		(ogg_sync_state *, int);
typedef int			(*OGGSYNCPAGEOUT)	(ogg_sync_state *, ogg_page *);
typedef int			(*OGGSYNCCLEAR)		(ogg_sync_state *);

extern OGGSTREAMINIT		 ex_ogg_stream_init;
extern OGGSTREAMPACKETIN	 ex_ogg_stream_packetin;
extern OGGSTREAMPACKETOUT	 ex_ogg_stream_packetout;
extern OGGSTREAMFLUSH		 ex_ogg_stream_flush;
extern OGGSTREAMPAGEIN		 ex_ogg_stream_pagein;
extern OGGPAGESERIALNO		 ex_ogg_page_serialno;
extern OGGSTREAMCLEAR		 ex_ogg_stream_clear;
extern OGGSYNCINIT		 ex_ogg_sync_init;
extern OGGSYNCBUFFER		 ex_ogg_sync_buffer;
extern OGGSYNCWROTE		 ex_ogg_sync_wrote;
extern OGGSYNCPAGEOUT		 ex_ogg_sync_pageout;
extern OGGSYNCCLEAR		 ex_ogg_sync_clear;
