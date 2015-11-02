 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#undef callbacks
#include <FLAC/metadata.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*flacdll;

Bool			 LoadFLACDLL();
Void			 FreeFLACDLL();

typedef int				 *FLAC_API_SUPPORTS_OGG_FLAC_TYPE;

typedef FLAC__Metadata_Chain *		(*FLAC__METADATA_CHAIN_NEW)					();
typedef void				(*FLAC__METADATA_CHAIN_DELETE)					(FLAC__Metadata_Chain *);
typedef FLAC__bool			(*FLAC__METADATA_CHAIN_READ_WITH_CALLBACKS)			(FLAC__Metadata_Chain *, FLAC__IOHandle, FLAC__IOCallbacks);
typedef FLAC__bool			(*FLAC__METADATA_CHAIN_READ_OGG_WITH_CALLBACKS)			(FLAC__Metadata_Chain *, FLAC__IOHandle, FLAC__IOCallbacks);
typedef FLAC__bool			(*FLAC__METADATA_CHAIN_WRITE_WITH_CALLBACKS)			(FLAC__Metadata_Chain *, FLAC__bool, FLAC__IOHandle, FLAC__IOCallbacks);
typedef FLAC__bool			(*FLAC__METADATA_CHAIN_WRITE_WITH_CALLBACKS_AND_TEMPFILE)	(FLAC__Metadata_Chain *, FLAC__bool, FLAC__IOHandle, FLAC__IOCallbacks, FLAC__IOHandle, FLAC__IOCallbacks);
typedef void				(*FLAC__METADATA_CHAIN_SORT_PADDING)				(FLAC__Metadata_Chain *);
typedef FLAC__bool			(*FLAC__METADATA_CHAIN_CHECK_IF_TEMPFILE_NEEDED)		(FLAC__Metadata_Chain *);
typedef FLAC__Metadata_ChainStatus	(*FLAC__METADATA_CHAIN_STATUS)					(FLAC__Metadata_Chain *);

typedef FLAC__Metadata_Iterator *	(*FLAC__METADATA_ITERATOR_NEW)					();
typedef void				(*FLAC__METADATA_ITERATOR_DELETE)				(FLAC__Metadata_Iterator *);
typedef void				(*FLAC__METADATA_ITERATOR_INIT)					(FLAC__Metadata_Iterator *, FLAC__Metadata_Chain *);
typedef FLAC__bool			(*FLAC__METADATA_ITERATOR_NEXT)					(FLAC__Metadata_Iterator *);
typedef FLAC__MetadataType		(*FLAC__METADATA_ITERATOR_GET_BLOCK_TYPE)			(const FLAC__Metadata_Iterator *);
typedef FLAC__StreamMetadata *		(*FLAC__METADATA_ITERATOR_GET_BLOCK)				(const FLAC__Metadata_Iterator *);
typedef FLAC__bool			(*FLAC__METADATA_ITERATOR_DELETE_BLOCK)				(FLAC__Metadata_Iterator *, FLAC__bool);
typedef FLAC__bool			(*FLAC__METADATA_ITERATOR_INSERT_BLOCK_AFTER)			(FLAC__Metadata_Iterator *, FLAC__StreamMetadata *);

typedef FLAC__StreamMetadata *	 	(*FLAC__METADATA_OBJECT_NEW)					(FLAC__MetadataType);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE)			(FLAC__StreamMetadata *, char *, FLAC__bool);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION)		(FLAC__StreamMetadata *, FLAC__byte *, FLAC__bool);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_PICTURE_SET_DATA)			(FLAC__StreamMetadata *, FLAC__byte *, FLAC__uint32, FLAC__bool);
typedef FLAC__bool 			(*FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS)		(FLAC__StreamMetadata *, unsigned);

typedef char *				 *FLAC__VENDOR_STRING_TYPE;

extern FLAC_API_SUPPORTS_OGG_FLAC_TYPE				 ex_FLAC_API_SUPPORTS_OGG_FLAC;

extern FLAC__METADATA_CHAIN_NEW					 ex_FLAC__metadata_chain_new;
extern FLAC__METADATA_CHAIN_DELETE				 ex_FLAC__metadata_chain_delete;
extern FLAC__METADATA_CHAIN_READ_WITH_CALLBACKS			 ex_FLAC__metadata_chain_read_with_callbacks;
extern FLAC__METADATA_CHAIN_READ_OGG_WITH_CALLBACKS		 ex_FLAC__metadata_chain_read_ogg_with_callbacks;
extern FLAC__METADATA_CHAIN_WRITE_WITH_CALLBACKS		 ex_FLAC__metadata_chain_write_with_callbacks;
extern FLAC__METADATA_CHAIN_WRITE_WITH_CALLBACKS_AND_TEMPFILE	 ex_FLAC__metadata_chain_write_with_callbacks_and_tempfile;
extern FLAC__METADATA_CHAIN_SORT_PADDING			 ex_FLAC__metadata_chain_sort_padding;
extern FLAC__METADATA_CHAIN_CHECK_IF_TEMPFILE_NEEDED		 ex_FLAC__metadata_chain_check_if_tempfile_needed;
extern FLAC__METADATA_CHAIN_STATUS				 ex_FLAC__metadata_chain_status;

extern FLAC__METADATA_ITERATOR_NEW				 ex_FLAC__metadata_iterator_new;
extern FLAC__METADATA_ITERATOR_DELETE				 ex_FLAC__metadata_iterator_delete;
extern FLAC__METADATA_ITERATOR_INIT				 ex_FLAC__metadata_iterator_init;
extern FLAC__METADATA_ITERATOR_NEXT				 ex_FLAC__metadata_iterator_next;
extern FLAC__METADATA_ITERATOR_GET_BLOCK_TYPE			 ex_FLAC__metadata_iterator_get_block_type;
extern FLAC__METADATA_ITERATOR_GET_BLOCK			 ex_FLAC__metadata_iterator_get_block;
extern FLAC__METADATA_ITERATOR_DELETE_BLOCK			 ex_FLAC__metadata_iterator_delete_block;
extern FLAC__METADATA_ITERATOR_INSERT_BLOCK_AFTER		 ex_FLAC__metadata_iterator_insert_block_after;

extern FLAC__METADATA_OBJECT_NEW				 ex_FLAC__metadata_object_new;
extern FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE		 ex_FLAC__metadata_object_picture_set_mime_type;
extern FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION		 ex_FLAC__metadata_object_picture_set_description;
extern FLAC__METADATA_OBJECT_PICTURE_SET_DATA			 ex_FLAC__metadata_object_picture_set_data;
extern FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS	 ex_FLAC__metadata_object_vorbiscomment_resize_comments;

extern FLAC__VENDOR_STRING_TYPE					 ex_FLAC__VENDOR_STRING;
