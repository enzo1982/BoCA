 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
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

typedef FLAC__StreamEncoder *		(*FLAC__STREAM_ENCODER_NEW)						();
typedef void				(*FLAC__STREAM_ENCODER_DELETE)						(FLAC__StreamEncoder *);
typedef FLAC__StreamEncoderInitStatus	(*FLAC__STREAM_ENCODER_INIT_STREAM)					(FLAC__StreamEncoder *, FLAC__StreamEncoderWriteCallback, FLAC__StreamEncoderSeekCallback, FLAC__StreamEncoderTellCallback, FLAC__StreamEncoderMetadataCallback, void *);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_FINISH)						(FLAC__StreamEncoder *);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_CHANNELS)					(FLAC__StreamEncoder *, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_BITS_PER_SAMPLE)				(FLAC__StreamEncoder *, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_SAMPLE_RATE)					(FLAC__StreamEncoder *, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_STREAMABLE_SUBSET)				(FLAC__StreamEncoder *, FLAC__bool);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_DO_MID_SIDE_STEREO)				(FLAC__StreamEncoder *, FLAC__bool);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_LOOSE_MID_SIDE_STEREO)			(FLAC__StreamEncoder *, FLAC__bool);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_BLOCKSIZE)					(FLAC__StreamEncoder *, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_MAX_LPC_ORDER)				(FLAC__StreamEncoder *, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_QLP_COEFF_PRECISION)				(FLAC__StreamEncoder *, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_DO_QLP_COEFF_PREC_SEARCH)			(FLAC__StreamEncoder *, FLAC__bool);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_DO_EXHAUSTIVE_MODEL_SEARCH)			(FLAC__StreamEncoder *, FLAC__bool);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_MIN_RESIDUAL_PARTITION_ORDER)		(FLAC__StreamEncoder *, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_MAX_RESIDUAL_PARTITION_ORDER)		(FLAC__StreamEncoder *, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_METADATA)					(FLAC__StreamEncoder *, FLAC__StreamMetadata **, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_COMPRESSION_LEVEL)				(FLAC__StreamEncoder *, unsigned);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_SET_APODIZATION)					(FLAC__StreamEncoder *, const char *);
typedef FLAC__bool			(*FLAC__STREAM_ENCODER_PROCESS_INTERLEAVED)				(FLAC__StreamEncoder *, const FLAC__int32[], unsigned);
typedef FLAC__StreamMetadata *	 	(*FLAC__METADATA_OBJECT_NEW)						(FLAC__MetadataType);
typedef void				(*FLAC__METADATA_OBJECT_DELETE)						(FLAC__StreamMetadata *);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE)				(FLAC__StreamMetadata *, char *, FLAC__bool);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION)			(FLAC__StreamMetadata *, FLAC__byte *, FLAC__bool);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_PICTURE_SET_DATA)				(FLAC__StreamMetadata *, FLAC__byte *, FLAC__uint32, FLAC__bool);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_VORBISCOMMENT_APPEND_COMMENT)			(FLAC__StreamMetadata *, FLAC__StreamMetadata_VorbisComment_Entry, FLAC__bool);
typedef char *				 *FLAC__VENDOR_STRING_TYPE;
typedef char *				 *FLAC__VERSION_STRING_TYPE;

extern FLAC__STREAM_ENCODER_NEW						 ex_FLAC__stream_encoder_new;
extern FLAC__STREAM_ENCODER_DELETE					 ex_FLAC__stream_encoder_delete;
extern FLAC__STREAM_ENCODER_INIT_STREAM					 ex_FLAC__stream_encoder_init_stream;
extern FLAC__STREAM_ENCODER_FINISH					 ex_FLAC__stream_encoder_finish;
extern FLAC__STREAM_ENCODER_SET_CHANNELS				 ex_FLAC__stream_encoder_set_channels;
extern FLAC__STREAM_ENCODER_SET_BITS_PER_SAMPLE				 ex_FLAC__stream_encoder_set_bits_per_sample;
extern FLAC__STREAM_ENCODER_SET_SAMPLE_RATE				 ex_FLAC__stream_encoder_set_sample_rate;
extern FLAC__STREAM_ENCODER_SET_STREAMABLE_SUBSET			 ex_FLAC__stream_encoder_set_streamable_subset;
extern FLAC__STREAM_ENCODER_SET_DO_MID_SIDE_STEREO			 ex_FLAC__stream_encoder_set_do_mid_side_stereo;
extern FLAC__STREAM_ENCODER_SET_LOOSE_MID_SIDE_STEREO			 ex_FLAC__stream_encoder_set_loose_mid_side_stereo;
extern FLAC__STREAM_ENCODER_SET_BLOCKSIZE				 ex_FLAC__stream_encoder_set_blocksize;
extern FLAC__STREAM_ENCODER_SET_MAX_LPC_ORDER				 ex_FLAC__stream_encoder_set_max_lpc_order;
extern FLAC__STREAM_ENCODER_SET_QLP_COEFF_PRECISION			 ex_FLAC__stream_encoder_set_qlp_coeff_precision;
extern FLAC__STREAM_ENCODER_SET_DO_QLP_COEFF_PREC_SEARCH		 ex_FLAC__stream_encoder_set_do_qlp_coeff_prec_search;
extern FLAC__STREAM_ENCODER_SET_DO_EXHAUSTIVE_MODEL_SEARCH		 ex_FLAC__stream_encoder_set_do_exhaustive_model_search;
extern FLAC__STREAM_ENCODER_SET_MIN_RESIDUAL_PARTITION_ORDER		 ex_FLAC__stream_encoder_set_min_residual_partition_order;
extern FLAC__STREAM_ENCODER_SET_MAX_RESIDUAL_PARTITION_ORDER		 ex_FLAC__stream_encoder_set_max_residual_partition_order;
extern FLAC__STREAM_ENCODER_SET_METADATA				 ex_FLAC__stream_encoder_set_metadata;
extern FLAC__STREAM_ENCODER_SET_COMPRESSION_LEVEL			 ex_FLAC__stream_encoder_set_compression_level;
extern FLAC__STREAM_ENCODER_SET_APODIZATION				 ex_FLAC__stream_encoder_set_apodization;
extern FLAC__STREAM_ENCODER_PROCESS_INTERLEAVED				 ex_FLAC__stream_encoder_process_interleaved;
extern FLAC__METADATA_OBJECT_NEW					 ex_FLAC__metadata_object_new;
extern FLAC__METADATA_OBJECT_DELETE					 ex_FLAC__metadata_object_delete;
extern FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE			 ex_FLAC__metadata_object_picture_set_mime_type;
extern FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION			 ex_FLAC__metadata_object_picture_set_description;
extern FLAC__METADATA_OBJECT_PICTURE_SET_DATA				 ex_FLAC__metadata_object_picture_set_data;
extern FLAC__METADATA_OBJECT_VORBISCOMMENT_APPEND_COMMENT		 ex_FLAC__metadata_object_vorbiscomment_append_comment;
extern FLAC__VENDOR_STRING_TYPE						 ex_FLAC__VENDOR_STRING;
extern FLAC__VERSION_STRING_TYPE					 ex_FLAC__VERSION_STRING;
