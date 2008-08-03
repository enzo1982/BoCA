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

#include "flac/stream_encoder.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*flacdll;

Bool			 LoadFLACDLL();
Void			 FreeFLACDLL();

typedef FLAC__StreamDecoder *		(*FLAC__STREAM_DECODER_NEW)						();
typedef void				(*FLAC__STREAM_DECODER_DELETE)						(FLAC__StreamDecoder *);
typedef FLAC__bool	 		(*FLAC__STREAM_DECODER_SET_METADATA_RESPOND)				(FLAC__StreamDecoder *, FLAC__MetadataType);
typedef FLAC__StreamDecoderInitStatus	(*FLAC__STREAM_DECODER_INIT_STREAM)					(FLAC__StreamDecoder *, FLAC__StreamDecoderReadCallback, FLAC__StreamDecoderSeekCallback, FLAC__StreamDecoderTellCallback, FLAC__StreamDecoderLengthCallback, FLAC__StreamDecoderEofCallback, FLAC__StreamDecoderWriteCallback, FLAC__StreamDecoderMetadataCallback, FLAC__StreamDecoderErrorCallback, void *);
typedef void				(*FLAC__STREAM_DECODER_FINISH)						(FLAC__StreamDecoder *);
typedef unsigned			(*FLAC__STREAM_DECODER_GET_CHANNELS)					(const FLAC__StreamDecoder *);
typedef unsigned			(*FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE)				(const FLAC__StreamDecoder *);
typedef unsigned			(*FLAC__STREAM_DECODER_GET_SAMPLE_RATE)					(const FLAC__StreamDecoder *);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA)			(FLAC__StreamDecoder *);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM)			(FLAC__StreamDecoder *);
typedef char *				 *FLAC__VERSION_STRING_TYPE;

extern FLAC__STREAM_DECODER_NEW						 ex_FLAC__stream_decoder_new;
extern FLAC__STREAM_DECODER_DELETE					 ex_FLAC__stream_decoder_delete;
extern FLAC__STREAM_DECODER_SET_METADATA_RESPOND			 ex_FLAC__stream_decoder_set_metadata_respond;
extern FLAC__STREAM_DECODER_INIT_STREAM					 ex_FLAC__stream_decoder_init_stream;
extern FLAC__STREAM_DECODER_FINISH					 ex_FLAC__stream_decoder_finish;
extern FLAC__STREAM_DECODER_GET_CHANNELS				 ex_FLAC__stream_decoder_get_channels;
extern FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE				 ex_FLAC__stream_decoder_get_bits_per_sample;
extern FLAC__STREAM_DECODER_GET_SAMPLE_RATE				 ex_FLAC__stream_decoder_get_sample_rate;
extern FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA		 ex_FLAC__stream_decoder_process_until_end_of_metadata;
extern FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM			 ex_FLAC__stream_decoder_process_until_end_of_stream;
extern FLAC__VERSION_STRING_TYPE					 ex_FLAC__VERSION_STRING;
