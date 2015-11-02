 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

FLAC_API_SUPPORTS_OGG_FLAC_TYPE				 ex_FLAC_API_SUPPORTS_OGG_FLAC					= NIL;

FLAC__METADATA_CHAIN_NEW				 ex_FLAC__metadata_chain_new					= NIL;
FLAC__METADATA_CHAIN_DELETE				 ex_FLAC__metadata_chain_delete					= NIL;
FLAC__METADATA_CHAIN_READ_WITH_CALLBACKS		 ex_FLAC__metadata_chain_read_with_callbacks			= NIL;
FLAC__METADATA_CHAIN_READ_OGG_WITH_CALLBACKS		 ex_FLAC__metadata_chain_read_ogg_with_callbacks		= NIL;
FLAC__METADATA_CHAIN_WRITE_WITH_CALLBACKS		 ex_FLAC__metadata_chain_write_with_callbacks			= NIL;
FLAC__METADATA_CHAIN_WRITE_WITH_CALLBACKS_AND_TEMPFILE	 ex_FLAC__metadata_chain_write_with_callbacks_and_tempfile	= NIL;
FLAC__METADATA_CHAIN_SORT_PADDING			 ex_FLAC__metadata_chain_sort_padding				= NIL;
FLAC__METADATA_CHAIN_CHECK_IF_TEMPFILE_NEEDED		 ex_FLAC__metadata_chain_check_if_tempfile_needed		= NIL;
FLAC__METADATA_CHAIN_STATUS				 ex_FLAC__metadata_chain_status					= NIL;

FLAC__METADATA_ITERATOR_NEW				 ex_FLAC__metadata_iterator_new					= NIL;
FLAC__METADATA_ITERATOR_DELETE				 ex_FLAC__metadata_iterator_delete				= NIL;
FLAC__METADATA_ITERATOR_INIT				 ex_FLAC__metadata_iterator_init				= NIL;
FLAC__METADATA_ITERATOR_NEXT				 ex_FLAC__metadata_iterator_next				= NIL;
FLAC__METADATA_ITERATOR_GET_BLOCK_TYPE			 ex_FLAC__metadata_iterator_get_block_type			= NIL;
FLAC__METADATA_ITERATOR_GET_BLOCK			 ex_FLAC__metadata_iterator_get_block				= NIL;
FLAC__METADATA_ITERATOR_DELETE_BLOCK			 ex_FLAC__metadata_iterator_delete_block			= NIL;
FLAC__METADATA_ITERATOR_INSERT_BLOCK_AFTER		 ex_FLAC__metadata_iterator_insert_block_after			= NIL;

FLAC__METADATA_OBJECT_NEW				 ex_FLAC__metadata_object_new					= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE		 ex_FLAC__metadata_object_picture_set_mime_type			= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION		 ex_FLAC__metadata_object_picture_set_description		= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_DATA			 ex_FLAC__metadata_object_picture_set_data			= NIL;
FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS	 ex_FLAC__metadata_object_vorbiscomment_resize_comments		= NIL;

FLAC__VENDOR_STRING_TYPE				 ex_FLAC__VENDOR_STRING						= NIL;

DynamicLoader *flacdll	= NIL;

Bool LoadFLACDLL()
{
	flacdll = BoCA::Utilities::LoadCodecDLL("FLAC");

	if (flacdll == NIL) return False;

	ex_FLAC_API_SUPPORTS_OGG_FLAC					= (FLAC_API_SUPPORTS_OGG_FLAC_TYPE) flacdll->GetFunctionAddress("FLAC_API_SUPPORTS_OGG_FLAC");

	ex_FLAC__metadata_chain_new					= (FLAC__METADATA_CHAIN_NEW) flacdll->GetFunctionAddress("FLAC__metadata_chain_new");
	ex_FLAC__metadata_chain_delete					= (FLAC__METADATA_CHAIN_DELETE) flacdll->GetFunctionAddress("FLAC__metadata_chain_delete");
	ex_FLAC__metadata_chain_read_with_callbacks			= (FLAC__METADATA_CHAIN_READ_WITH_CALLBACKS) flacdll->GetFunctionAddress("FLAC__metadata_chain_read_with_callbacks");
	ex_FLAC__metadata_chain_read_ogg_with_callbacks			= (FLAC__METADATA_CHAIN_READ_OGG_WITH_CALLBACKS) flacdll->GetFunctionAddress("FLAC__metadata_chain_read_ogg_with_callbacks");
	ex_FLAC__metadata_chain_write_with_callbacks			= (FLAC__METADATA_CHAIN_WRITE_WITH_CALLBACKS) flacdll->GetFunctionAddress("FLAC__metadata_chain_write_with_callbacks");
	ex_FLAC__metadata_chain_write_with_callbacks_and_tempfile	= (FLAC__METADATA_CHAIN_WRITE_WITH_CALLBACKS_AND_TEMPFILE) flacdll->GetFunctionAddress("FLAC__metadata_chain_write_with_callbacks_and_tempfile");
	ex_FLAC__metadata_chain_sort_padding				= (FLAC__METADATA_CHAIN_SORT_PADDING) flacdll->GetFunctionAddress("FLAC__metadata_chain_sort_padding");
	ex_FLAC__metadata_chain_check_if_tempfile_needed		= (FLAC__METADATA_CHAIN_CHECK_IF_TEMPFILE_NEEDED) flacdll->GetFunctionAddress("FLAC__metadata_chain_check_if_tempfile_needed");
	ex_FLAC__metadata_chain_status					= (FLAC__METADATA_CHAIN_STATUS) flacdll->GetFunctionAddress("FLAC__metadata_chain_status");

	ex_FLAC__metadata_iterator_new					= (FLAC__METADATA_ITERATOR_NEW) flacdll->GetFunctionAddress("FLAC__metadata_iterator_new");
	ex_FLAC__metadata_iterator_delete				= (FLAC__METADATA_ITERATOR_DELETE) flacdll->GetFunctionAddress("FLAC__metadata_iterator_delete");
	ex_FLAC__metadata_iterator_init					= (FLAC__METADATA_ITERATOR_INIT) flacdll->GetFunctionAddress("FLAC__metadata_iterator_init");
	ex_FLAC__metadata_iterator_next					= (FLAC__METADATA_ITERATOR_NEXT) flacdll->GetFunctionAddress("FLAC__metadata_iterator_next");
	ex_FLAC__metadata_iterator_get_block_type			= (FLAC__METADATA_ITERATOR_GET_BLOCK_TYPE) flacdll->GetFunctionAddress("FLAC__metadata_iterator_get_block_type");
	ex_FLAC__metadata_iterator_get_block				= (FLAC__METADATA_ITERATOR_GET_BLOCK) flacdll->GetFunctionAddress("FLAC__metadata_iterator_get_block");
	ex_FLAC__metadata_iterator_delete_block				= (FLAC__METADATA_ITERATOR_DELETE_BLOCK) flacdll->GetFunctionAddress("FLAC__metadata_iterator_delete_block");
	ex_FLAC__metadata_iterator_insert_block_after			= (FLAC__METADATA_ITERATOR_INSERT_BLOCK_AFTER) flacdll->GetFunctionAddress("FLAC__metadata_iterator_insert_block_after");

	ex_FLAC__metadata_object_new					= (FLAC__METADATA_OBJECT_NEW) flacdll->GetFunctionAddress("FLAC__metadata_object_new");
	ex_FLAC__metadata_object_picture_set_mime_type			= (FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_mime_type");
	ex_FLAC__metadata_object_picture_set_description		= (FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_description");
	ex_FLAC__metadata_object_picture_set_data			= (FLAC__METADATA_OBJECT_PICTURE_SET_DATA) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_data");
	ex_FLAC__metadata_object_vorbiscomment_resize_comments		= (FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS) flacdll->GetFunctionAddress("FLAC__metadata_object_vorbiscomment_resize_comments");

	ex_FLAC__VENDOR_STRING						= (FLAC__VENDOR_STRING_TYPE) flacdll->GetFunctionAddress("FLAC__VENDOR_STRING");

	if (ex_FLAC_API_SUPPORTS_OGG_FLAC				== NIL ||

	    ex_FLAC__metadata_chain_new					== NIL ||
	    ex_FLAC__metadata_chain_delete				== NIL ||
	    ex_FLAC__metadata_chain_read_with_callbacks			== NIL ||
	    ex_FLAC__metadata_chain_read_ogg_with_callbacks		== NIL ||
	    ex_FLAC__metadata_chain_write_with_callbacks		== NIL ||
	    ex_FLAC__metadata_chain_write_with_callbacks_and_tempfile	== NIL ||
	    ex_FLAC__metadata_chain_sort_padding			== NIL ||
	    ex_FLAC__metadata_chain_check_if_tempfile_needed		== NIL ||
	    ex_FLAC__metadata_chain_status				== NIL ||

	    ex_FLAC__metadata_iterator_new				== NIL ||
	    ex_FLAC__metadata_iterator_delete				== NIL ||
	    ex_FLAC__metadata_iterator_init				== NIL ||
	    ex_FLAC__metadata_iterator_next				== NIL ||
	    ex_FLAC__metadata_iterator_get_block_type			== NIL ||
	    ex_FLAC__metadata_iterator_delete_block			== NIL ||
	    ex_FLAC__metadata_iterator_insert_block_after		== NIL ||

	    ex_FLAC__metadata_object_new				== NIL ||
	    ex_FLAC__metadata_object_picture_set_mime_type		== NIL ||
	    ex_FLAC__metadata_object_picture_set_description		== NIL ||
	    ex_FLAC__metadata_object_picture_set_data			== NIL ||
	    ex_FLAC__metadata_object_vorbiscomment_resize_comments	== NIL ||

	    ex_FLAC__VENDOR_STRING					== NIL) { FreeFLACDLL(); return False; }

	return True;
}

Void FreeFLACDLL()
{
	BoCA::Utilities::FreeCodecDLL(flacdll);

	flacdll = NIL;
}
