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
#include <FLAC/stream_encoder.h>

#undef callbacks
#include <FLAC/metadata.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*oggdll;
extern DynamicLoader	*flacdll;

Bool			 LoadOggDLL();
Void			 FreeOggDLL();

Bool			 LoadFLACDLL();
Void			 FreeFLACDLL();

typedef int	(*OGGSTREAMINIT)	(ogg_stream_state *, int);
typedef int	(*OGGSTREAMPACKETOUT)	(ogg_stream_state *, ogg_packet *);
typedef int	(*OGGSTREAMPAGEIN)	(ogg_stream_state *, ogg_page *);
typedef int	(*OGGPAGEEOS)		(ogg_page *);
typedef int	(*OGGPAGESERIALNO)	(ogg_page *);
typedef int	(*OGGSTREAMCLEAR)	(ogg_stream_state *);
typedef int	(*OGGSYNCINIT)		(ogg_sync_state *);
typedef char *	(*OGGSYNCBUFFER)	(ogg_sync_state *, int);
typedef int	(*OGGSYNCWROTE)		(ogg_sync_state *, int);
typedef int	(*OGGSYNCPAGEOUT)	(ogg_sync_state *, ogg_page *);
typedef int	(*OGGSYNCCLEAR)		(ogg_sync_state *);

extern OGGSTREAMINIT		 ex_ogg_stream_init;
extern OGGSTREAMPACKETOUT	 ex_ogg_stream_packetout;
extern OGGSTREAMPAGEIN		 ex_ogg_stream_pagein;
extern OGGPAGEEOS		 ex_ogg_page_eos;
extern OGGPAGESERIALNO		 ex_ogg_page_serialno;
extern OGGSTREAMCLEAR		 ex_ogg_stream_clear;
extern OGGSYNCINIT		 ex_ogg_sync_init;
extern OGGSYNCBUFFER		 ex_ogg_sync_buffer;
extern OGGSYNCWROTE		 ex_ogg_sync_wrote;
extern OGGSYNCPAGEOUT		 ex_ogg_sync_pageout;
extern OGGSYNCCLEAR		 ex_ogg_sync_clear;

typedef int				 *FLAC_API_SUPPORTS_OGG_FLAC_TYPE;

typedef FLAC__StreamDecoder *		(*FLAC__STREAM_DECODER_NEW)				();
typedef void				(*FLAC__STREAM_DECODER_DELETE)				(FLAC__StreamDecoder *);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_SET_METADATA_RESPOND)		(FLAC__StreamDecoder *, FLAC__MetadataType);
typedef FLAC__StreamDecoderInitStatus	(*FLAC__STREAM_DECODER_INIT_STREAM)			(FLAC__StreamDecoder *, FLAC__StreamDecoderReadCallback, FLAC__StreamDecoderSeekCallback, FLAC__StreamDecoderTellCallback, FLAC__StreamDecoderLengthCallback, FLAC__StreamDecoderEofCallback, FLAC__StreamDecoderWriteCallback, FLAC__StreamDecoderMetadataCallback, FLAC__StreamDecoderErrorCallback, void *);
typedef FLAC__StreamDecoderInitStatus	(*FLAC__STREAM_DECODER_INIT_OGG_STREAM)			(FLAC__StreamDecoder *, FLAC__StreamDecoderReadCallback, FLAC__StreamDecoderSeekCallback, FLAC__StreamDecoderTellCallback, FLAC__StreamDecoderLengthCallback, FLAC__StreamDecoderEofCallback, FLAC__StreamDecoderWriteCallback, FLAC__StreamDecoderMetadataCallback, FLAC__StreamDecoderErrorCallback, void *);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_FINISH)				(FLAC__StreamDecoder *);
typedef unsigned			(*FLAC__STREAM_DECODER_GET_CHANNELS)			(const FLAC__StreamDecoder *);
typedef unsigned			(*FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE)		(const FLAC__StreamDecoder *);
typedef unsigned			(*FLAC__STREAM_DECODER_GET_SAMPLE_RATE)			(const FLAC__StreamDecoder *);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_SEEK_ABSOLUTE)			(FLAC__StreamDecoder *, FLAC__uint64);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA)	(FLAC__StreamDecoder *);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM)	(FLAC__StreamDecoder *);

typedef char *				 *FLAC__VERSION_STRING_TYPE;

extern FLAC_API_SUPPORTS_OGG_FLAC_TYPE				 ex_FLAC_API_SUPPORTS_OGG_FLAC;

extern FLAC__STREAM_DECODER_NEW					 ex_FLAC__stream_decoder_new;
extern FLAC__STREAM_DECODER_DELETE				 ex_FLAC__stream_decoder_delete;
extern FLAC__STREAM_DECODER_SET_METADATA_RESPOND		 ex_FLAC__stream_decoder_set_metadata_respond;
extern FLAC__STREAM_DECODER_INIT_STREAM				 ex_FLAC__stream_decoder_init_stream;
extern FLAC__STREAM_DECODER_INIT_OGG_STREAM			 ex_FLAC__stream_decoder_init_ogg_stream;
extern FLAC__STREAM_DECODER_FINISH				 ex_FLAC__stream_decoder_finish;
extern FLAC__STREAM_DECODER_GET_CHANNELS			 ex_FLAC__stream_decoder_get_channels;
extern FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE			 ex_FLAC__stream_decoder_get_bits_per_sample;
extern FLAC__STREAM_DECODER_GET_SAMPLE_RATE			 ex_FLAC__stream_decoder_get_sample_rate;
extern FLAC__STREAM_DECODER_SEEK_ABSOLUTE			 ex_FLAC__stream_decoder_seek_absolute;
extern FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA	 ex_FLAC__stream_decoder_process_until_end_of_metadata;
extern FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM		 ex_FLAC__stream_decoder_process_until_end_of_stream;

extern FLAC__VERSION_STRING_TYPE				 ex_FLAC__VERSION_STRING;
