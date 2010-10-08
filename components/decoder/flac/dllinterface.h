 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "flac/stream_encoder.h"

#undef callbacks
#include "flac/metadata.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*flacdll;

Bool			 LoadFLACDLL();
Void			 FreeFLACDLL();

typedef FLAC__StreamDecoder *		(*FLAC__STREAM_DECODER_NEW)						();
typedef void				(*FLAC__STREAM_DECODER_DELETE)						(FLAC__StreamDecoder *);
typedef FLAC__bool	 		(*FLAC__STREAM_DECODER_SET_METADATA_RESPOND)				(FLAC__StreamDecoder *, FLAC__MetadataType);
typedef FLAC__StreamDecoderInitStatus	(*FLAC__STREAM_DECODER_INIT_STREAM)					(FLAC__StreamDecoder *, FLAC__StreamDecoderReadCallback, FLAC__StreamDecoderSeekCallback, FLAC__StreamDecoderTellCallback, FLAC__StreamDecoderLengthCallback, FLAC__StreamDecoderEofCallback, FLAC__StreamDecoderWriteCallback, FLAC__StreamDecoderMetadataCallback, FLAC__StreamDecoderErrorCallback, void *);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_FINISH)						(FLAC__StreamDecoder *);
typedef unsigned			(*FLAC__STREAM_DECODER_GET_CHANNELS)					(const FLAC__StreamDecoder *);
typedef unsigned			(*FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE)				(const FLAC__StreamDecoder *);
typedef unsigned			(*FLAC__STREAM_DECODER_GET_SAMPLE_RATE)					(const FLAC__StreamDecoder *);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_SEEK_ABSOLUTE)					(FLAC__StreamDecoder *, FLAC__uint64);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA)			(FLAC__StreamDecoder *);
typedef FLAC__bool			(*FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM)			(FLAC__StreamDecoder *);

typedef FLAC__Metadata_Chain *		(*FLAC__METADATA_CHAIN_NEW)						();
typedef void				(*FLAC__METADATA_CHAIN_DELETE)						(FLAC__Metadata_Chain *);
typedef FLAC__bool			(*FLAC__METADATA_CHAIN_READ)						(FLAC__Metadata_Chain *, const char *);
typedef FLAC__bool			(*FLAC__METADATA_CHAIN_WRITE)						(FLAC__Metadata_Chain *, FLAC__bool, FLAC__bool);
typedef void				(*FLAC__METADATA_CHAIN_SORT_PADDING)					(FLAC__Metadata_Chain *);

typedef FLAC__Metadata_Iterator *	(*FLAC__METADATA_ITERATOR_NEW)						();
typedef void				(*FLAC__METADATA_ITERATOR_DELETE)					(FLAC__Metadata_Iterator *);
typedef void				(*FLAC__METADATA_ITERATOR_INIT)						(FLAC__Metadata_Iterator *, FLAC__Metadata_Chain *);
typedef FLAC__bool			(*FLAC__METADATA_ITERATOR_NEXT)						(FLAC__Metadata_Iterator *);
typedef FLAC__MetadataType		(*FLAC__METADATA_ITERATOR_GET_BLOCK_TYPE)				(const FLAC__Metadata_Iterator *);
typedef FLAC__bool			(*FLAC__METADATA_ITERATOR_DELETE_BLOCK)					(FLAC__Metadata_Iterator *, FLAC__bool);
typedef FLAC__bool			(*FLAC__METADATA_ITERATOR_INSERT_BLOCK_AFTER)				(FLAC__Metadata_Iterator *, FLAC__StreamMetadata *);

typedef FLAC__StreamMetadata *	 	(*FLAC__METADATA_OBJECT_NEW)						(FLAC__MetadataType);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE)				(FLAC__StreamMetadata *, char *, FLAC__bool);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION)			(FLAC__StreamMetadata *, FLAC__byte *, FLAC__bool);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_PICTURE_SET_DATA)				(FLAC__StreamMetadata *, FLAC__byte *, FLAC__uint32, FLAC__bool);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS)			(FLAC__StreamMetadata *, unsigned);

typedef char *				 *FLAC__VENDOR_STRING_TYPE;
typedef char *				 *FLAC__VERSION_STRING_TYPE;

extern FLAC__STREAM_DECODER_NEW						 ex_FLAC__stream_decoder_new;
extern FLAC__STREAM_DECODER_DELETE					 ex_FLAC__stream_decoder_delete;
extern FLAC__STREAM_DECODER_SET_METADATA_RESPOND			 ex_FLAC__stream_decoder_set_metadata_respond;
extern FLAC__STREAM_DECODER_INIT_STREAM					 ex_FLAC__stream_decoder_init_stream;
extern FLAC__STREAM_DECODER_FINISH					 ex_FLAC__stream_decoder_finish;
extern FLAC__STREAM_DECODER_GET_CHANNELS				 ex_FLAC__stream_decoder_get_channels;
extern FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE				 ex_FLAC__stream_decoder_get_bits_per_sample;
extern FLAC__STREAM_DECODER_GET_SAMPLE_RATE				 ex_FLAC__stream_decoder_get_sample_rate;
extern FLAC__STREAM_DECODER_SEEK_ABSOLUTE				 ex_FLAC__stream_decoder_seek_absolute;
extern FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA		 ex_FLAC__stream_decoder_process_until_end_of_metadata;
extern FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM			 ex_FLAC__stream_decoder_process_until_end_of_stream;

extern FLAC__METADATA_CHAIN_NEW						 ex_FLAC__metadata_chain_new;
extern FLAC__METADATA_CHAIN_DELETE					 ex_FLAC__metadata_chain_delete;
extern FLAC__METADATA_CHAIN_READ					 ex_FLAC__metadata_chain_read;
extern FLAC__METADATA_CHAIN_WRITE					 ex_FLAC__metadata_chain_write;
extern FLAC__METADATA_CHAIN_SORT_PADDING				 ex_FLAC__metadata_chain_sort_padding;

extern FLAC__METADATA_ITERATOR_NEW					 ex_FLAC__metadata_iterator_new;
extern FLAC__METADATA_ITERATOR_DELETE					 ex_FLAC__metadata_iterator_delete;
extern FLAC__METADATA_ITERATOR_INIT					 ex_FLAC__metadata_iterator_init;
extern FLAC__METADATA_ITERATOR_NEXT					 ex_FLAC__metadata_iterator_next;
extern FLAC__METADATA_ITERATOR_GET_BLOCK_TYPE				 ex_FLAC__metadata_iterator_get_block_type;
extern FLAC__METADATA_ITERATOR_DELETE_BLOCK				 ex_FLAC__metadata_iterator_delete_block;
extern FLAC__METADATA_ITERATOR_INSERT_BLOCK_AFTER			 ex_FLAC__metadata_iterator_insert_block_after;

extern FLAC__METADATA_OBJECT_NEW					 ex_FLAC__metadata_object_new;
extern FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE			 ex_FLAC__metadata_object_picture_set_mime_type;
extern FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION			 ex_FLAC__metadata_object_picture_set_description;
extern FLAC__METADATA_OBJECT_PICTURE_SET_DATA				 ex_FLAC__metadata_object_picture_set_data;
extern FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS		 ex_FLAC__metadata_object_vorbiscomment_resize_comments;

extern FLAC__VENDOR_STRING_TYPE						 ex_FLAC__VENDOR_STRING;
extern FLAC__VERSION_STRING_TYPE					 ex_FLAC__VERSION_STRING;
