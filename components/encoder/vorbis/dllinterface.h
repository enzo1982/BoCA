 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include <vorbis/vorbisenc.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*oggdll;
extern DynamicLoader	*vorbisdll;
extern DynamicLoader	*vorbisencdll;

Bool			 LoadOggDLL();
Void			 FreeOggDLL();

Bool			 LoadVorbisDLL();
Void			 FreeVorbisDLL();

typedef int			(*OGGSTREAMINIT)		(ogg_stream_state *, int);
typedef int			(*OGGSTREAMPACKETIN)		(ogg_stream_state *, ogg_packet *);
typedef int			(*OGGSTREAMFLUSH)		(ogg_stream_state *, ogg_page *);
typedef int			(*OGGSTREAMPAGEOUT)		(ogg_stream_state *, ogg_page *);
typedef int			(*OGGPAGEEOS)			(ogg_page *);
typedef int			(*OGGSTREAMCLEAR)		(ogg_stream_state *);

extern OGGSTREAMINIT		 ex_ogg_stream_init;
extern OGGSTREAMPACKETIN	 ex_ogg_stream_packetin;
extern OGGSTREAMFLUSH		 ex_ogg_stream_flush;
extern OGGSTREAMPAGEOUT		 ex_ogg_stream_pageout;
extern OGGPAGEEOS		 ex_ogg_page_eos;
extern OGGSTREAMCLEAR		 ex_ogg_stream_clear;

typedef void			(*VORBISINFOINIT)		(vorbis_info *);
typedef void			(*VORBISCOMMENTINIT)		(vorbis_comment *);
typedef void			(*VORBISCOMMENTADDTAG)		(vorbis_comment *, char *, char *);
typedef int			(*VORBISANALYSISINIT)		(vorbis_dsp_state *, vorbis_info *);
typedef int			(*VORBISBLOCKINIT)		(vorbis_dsp_state *, vorbis_block *);
typedef int			(*VORBISANALYSISHEADEROUT)	(vorbis_dsp_state *, vorbis_comment *, ogg_packet *, ogg_packet *, ogg_packet *);
typedef float **		(*VORBISANALYSISBUFFER)		(vorbis_dsp_state *, int);
typedef int			(*VORBISANALYSISWROTE)		(vorbis_dsp_state *, int);
typedef int			(*VORBISANALYSISBLOCKOUT)	(vorbis_dsp_state *, vorbis_block *);
typedef int			(*VORBISANALYSIS)		(vorbis_block *, ogg_packet *);
typedef int			(*VORBISBITRATEADDBLOCK)	(vorbis_block *);
typedef int			(*VORBISBITRATEFLUSHPACKET)	(vorbis_dsp_state *, ogg_packet *);
typedef int			(*VORBISBLOCKCLEAR)		(vorbis_block *);
typedef void			(*VORBISDSPCLEAR)		(vorbis_dsp_state *);
typedef void			(*VORBISCOMMENTCLEAR)		(vorbis_comment *);
typedef void			(*VORBISINFOCLEAR)		(vorbis_info *);

typedef int			(*VORBISENCODEINIT)		(vorbis_info *, long, long, long, long, long);
typedef int			(*VORBISENCODEINITVBR)		(vorbis_info *, long, long, float);

extern VORBISINFOINIT		 ex_vorbis_info_init;
extern VORBISCOMMENTINIT	 ex_vorbis_comment_init;
extern VORBISCOMMENTADDTAG	 ex_vorbis_comment_add_tag;
extern VORBISANALYSISINIT	 ex_vorbis_analysis_init;
extern VORBISBLOCKINIT		 ex_vorbis_block_init;
extern VORBISANALYSISHEADEROUT	 ex_vorbis_analysis_headerout;
extern VORBISANALYSISBUFFER	 ex_vorbis_analysis_buffer;
extern VORBISANALYSISWROTE	 ex_vorbis_analysis_wrote;
extern VORBISANALYSISBLOCKOUT	 ex_vorbis_analysis_blockout;
extern VORBISANALYSIS		 ex_vorbis_analysis;
extern VORBISBITRATEADDBLOCK	 ex_vorbis_bitrate_addblock;
extern VORBISBITRATEFLUSHPACKET	 ex_vorbis_bitrate_flushpacket;
extern VORBISBLOCKCLEAR		 ex_vorbis_block_clear;
extern VORBISDSPCLEAR		 ex_vorbis_dsp_clear;
extern VORBISCOMMENTCLEAR	 ex_vorbis_comment_clear;
extern VORBISINFOCLEAR		 ex_vorbis_info_clear;

extern VORBISENCODEINIT		 ex_vorbis_encode_init;
extern VORBISENCODEINITVBR	 ex_vorbis_encode_init_vbr;
