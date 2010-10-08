 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

using namespace BoCA;

FLAC__STREAM_DECODER_NEW				 ex_FLAC__stream_decoder_new				= NIL;
FLAC__STREAM_DECODER_DELETE				 ex_FLAC__stream_decoder_delete				= NIL;
FLAC__STREAM_DECODER_SET_METADATA_RESPOND		 ex_FLAC__stream_decoder_set_metadata_respond		= NIL;
FLAC__STREAM_DECODER_INIT_STREAM			 ex_FLAC__stream_decoder_init_stream			= NIL;
FLAC__STREAM_DECODER_FINISH				 ex_FLAC__stream_decoder_finish				= NIL;
FLAC__STREAM_DECODER_GET_CHANNELS			 ex_FLAC__stream_decoder_get_channels			= NIL;
FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE		 ex_FLAC__stream_decoder_get_bits_per_sample		= NIL;
FLAC__STREAM_DECODER_GET_SAMPLE_RATE			 ex_FLAC__stream_decoder_get_sample_rate		= NIL;
FLAC__STREAM_DECODER_SEEK_ABSOLUTE			 ex_FLAC__stream_decoder_seek_absolute			= NIL;
FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA	 ex_FLAC__stream_decoder_process_until_end_of_metadata	= NIL;
FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM	 ex_FLAC__stream_decoder_process_until_end_of_stream	= NIL;

FLAC__METADATA_CHAIN_NEW				 ex_FLAC__metadata_chain_new				= NIL;
FLAC__METADATA_CHAIN_DELETE				 ex_FLAC__metadata_chain_delete				= NIL;
FLAC__METADATA_CHAIN_READ				 ex_FLAC__metadata_chain_read				= NIL;
FLAC__METADATA_CHAIN_WRITE				 ex_FLAC__metadata_chain_write				= NIL;
FLAC__METADATA_CHAIN_SORT_PADDING			 ex_FLAC__metadata_chain_sort_padding			= NIL;

FLAC__METADATA_ITERATOR_NEW				 ex_FLAC__metadata_iterator_new				= NIL;
FLAC__METADATA_ITERATOR_DELETE				 ex_FLAC__metadata_iterator_delete			= NIL;
FLAC__METADATA_ITERATOR_INIT				 ex_FLAC__metadata_iterator_init			= NIL;
FLAC__METADATA_ITERATOR_NEXT				 ex_FLAC__metadata_iterator_next			= NIL;
FLAC__METADATA_ITERATOR_GET_BLOCK_TYPE			 ex_FLAC__metadata_iterator_get_block_type		= NIL;
FLAC__METADATA_ITERATOR_DELETE_BLOCK			 ex_FLAC__metadata_iterator_delete_block		= NIL;
FLAC__METADATA_ITERATOR_INSERT_BLOCK_AFTER		 ex_FLAC__metadata_iterator_insert_block_after		= NIL;

FLAC__METADATA_OBJECT_NEW				 ex_FLAC__metadata_object_new				= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE		 ex_FLAC__metadata_object_picture_set_mime_type		= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION		 ex_FLAC__metadata_object_picture_set_description	= NIL;
FLAC__METADATA_OBJECT_PICTURE_SET_DATA			 ex_FLAC__metadata_object_picture_set_data		= NIL;
FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS	 ex_FLAC__metadata_object_vorbiscomment_resize_comments	= NIL;

FLAC__VENDOR_STRING_TYPE				 ex_FLAC__VENDOR_STRING					= NIL;
FLAC__VERSION_STRING_TYPE				 ex_FLAC__VERSION_STRING				= NIL;

DynamicLoader *flacdll	= NIL;

Bool LoadFLACDLL()
{
	if (Config::Get()->GetIntValue("OpenMP", "EnableOpenMP", True) && CPU().GetNumCores() >= 2 && CPU().HasSSE3())
	{
#ifdef __WIN32__
		if (!File(String(GUI::Application::GetApplicationDirectory()).Append("codecs\\FLAC-OpenMP.dll")).Exists()) return False;
#endif

		flacdll = new DynamicLoader("codecs/FLAC-OpenMP");

		if (flacdll->GetSystemModuleHandle() == NIL) FreeFLACDLL();
	}

	if (flacdll == NIL)
	{
#ifdef __WIN32__
		if (!File(String(GUI::Application::GetApplicationDirectory()).Append("codecs\\FLAC.dll")).Exists()) return False;
#endif

		flacdll = new DynamicLoader("codecs/FLAC");
	}

	ex_FLAC__stream_decoder_new				= (FLAC__STREAM_DECODER_NEW) flacdll->GetFunctionAddress("FLAC__stream_decoder_new");
	ex_FLAC__stream_decoder_delete				= (FLAC__STREAM_DECODER_DELETE) flacdll->GetFunctionAddress("FLAC__stream_decoder_delete");
	ex_FLAC__stream_decoder_set_metadata_respond		= (FLAC__STREAM_DECODER_SET_METADATA_RESPOND) flacdll->GetFunctionAddress("FLAC__stream_decoder_set_metadata_respond");
	ex_FLAC__stream_decoder_init_stream			= (FLAC__STREAM_DECODER_INIT_STREAM) flacdll->GetFunctionAddress("FLAC__stream_decoder_init_stream");
	ex_FLAC__stream_decoder_finish				= (FLAC__STREAM_DECODER_FINISH) flacdll->GetFunctionAddress("FLAC__stream_decoder_finish");
	ex_FLAC__stream_decoder_get_channels			= (FLAC__STREAM_DECODER_GET_CHANNELS) flacdll->GetFunctionAddress("FLAC__stream_decoder_get_channels");
	ex_FLAC__stream_decoder_get_bits_per_sample		= (FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE) flacdll->GetFunctionAddress("FLAC__stream_decoder_get_bits_per_sample");
	ex_FLAC__stream_decoder_get_sample_rate			= (FLAC__STREAM_DECODER_GET_SAMPLE_RATE) flacdll->GetFunctionAddress("FLAC__stream_decoder_get_sample_rate");
	ex_FLAC__stream_decoder_seek_absolute			= (FLAC__STREAM_DECODER_SEEK_ABSOLUTE) flacdll->GetFunctionAddress("FLAC__stream_decoder_seek_absolute");
	ex_FLAC__stream_decoder_process_until_end_of_metadata	= (FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA) flacdll->GetFunctionAddress("FLAC__stream_decoder_process_until_end_of_metadata");
	ex_FLAC__stream_decoder_process_until_end_of_stream	= (FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM) flacdll->GetFunctionAddress("FLAC__stream_decoder_process_until_end_of_stream");

	ex_FLAC__metadata_chain_new				= (FLAC__METADATA_CHAIN_NEW) flacdll->GetFunctionAddress("FLAC__metadata_chain_new");
	ex_FLAC__metadata_chain_delete				= (FLAC__METADATA_CHAIN_DELETE) flacdll->GetFunctionAddress("FLAC__metadata_chain_delete");
	ex_FLAC__metadata_chain_read				= (FLAC__METADATA_CHAIN_READ) flacdll->GetFunctionAddress("FLAC__metadata_chain_read");
	ex_FLAC__metadata_chain_write				= (FLAC__METADATA_CHAIN_WRITE) flacdll->GetFunctionAddress("FLAC__metadata_chain_write");
	ex_FLAC__metadata_chain_sort_padding			= (FLAC__METADATA_CHAIN_SORT_PADDING) flacdll->GetFunctionAddress("FLAC__metadata_chain_sort_padding");

	ex_FLAC__metadata_iterator_new				= (FLAC__METADATA_ITERATOR_NEW) flacdll->GetFunctionAddress("FLAC__metadata_iterator_new");
	ex_FLAC__metadata_iterator_delete			= (FLAC__METADATA_ITERATOR_DELETE) flacdll->GetFunctionAddress("FLAC__metadata_iterator_delete");
	ex_FLAC__metadata_iterator_init				= (FLAC__METADATA_ITERATOR_INIT) flacdll->GetFunctionAddress("FLAC__metadata_iterator_init");
	ex_FLAC__metadata_iterator_next				= (FLAC__METADATA_ITERATOR_NEXT) flacdll->GetFunctionAddress("FLAC__metadata_iterator_next");
	ex_FLAC__metadata_iterator_get_block_type		= (FLAC__METADATA_ITERATOR_GET_BLOCK_TYPE) flacdll->GetFunctionAddress("FLAC__metadata_iterator_get_block_type");
	ex_FLAC__metadata_iterator_delete_block			= (FLAC__METADATA_ITERATOR_DELETE_BLOCK) flacdll->GetFunctionAddress("FLAC__metadata_iterator_delete_block");
	ex_FLAC__metadata_iterator_insert_block_after		= (FLAC__METADATA_ITERATOR_INSERT_BLOCK_AFTER) flacdll->GetFunctionAddress("FLAC__metadata_iterator_insert_block_after");

	ex_FLAC__metadata_object_new				= (FLAC__METADATA_OBJECT_NEW) flacdll->GetFunctionAddress("FLAC__metadata_object_new");
	ex_FLAC__metadata_object_picture_set_mime_type		= (FLAC__METADATA_OBJECT_PICTURE_SET_MIME_TYPE) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_mime_type");
	ex_FLAC__metadata_object_picture_set_description	= (FLAC__METADATA_OBJECT_PICTURE_SET_DESCRIPTION) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_description");
	ex_FLAC__metadata_object_picture_set_data		= (FLAC__METADATA_OBJECT_PICTURE_SET_DATA) flacdll->GetFunctionAddress("FLAC__metadata_object_picture_set_data");
	ex_FLAC__metadata_object_vorbiscomment_resize_comments	= (FLAC__METADATA_OBJECT_VORBISCOMMENT_RESIZE_COMMENTS) flacdll->GetFunctionAddress("FLAC__metadata_object_vorbiscomment_resize_comments");

	ex_FLAC__VENDOR_STRING					= (FLAC__VENDOR_STRING_TYPE) flacdll->GetFunctionAddress("FLAC__VENDOR_STRING");
	ex_FLAC__VERSION_STRING					= (FLAC__VERSION_STRING_TYPE) flacdll->GetFunctionAddress("FLAC__VERSION_STRING");

	if (ex_FLAC__stream_decoder_new					== NIL ||
	    ex_FLAC__stream_decoder_delete				== NIL ||
	    ex_FLAC__stream_decoder_set_metadata_respond		== NIL ||
	    ex_FLAC__stream_decoder_init_stream				== NIL ||
	    ex_FLAC__stream_decoder_finish				== NIL ||
	    ex_FLAC__stream_decoder_get_channels			== NIL ||
	    ex_FLAC__stream_decoder_get_bits_per_sample			== NIL ||
	    ex_FLAC__stream_decoder_get_sample_rate			== NIL ||
	    ex_FLAC__stream_decoder_seek_absolute			== NIL ||
	    ex_FLAC__stream_decoder_process_until_end_of_metadata	== NIL ||
	    ex_FLAC__stream_decoder_process_until_end_of_stream		== NIL ||

	    ex_FLAC__metadata_chain_new					== NIL ||
	    ex_FLAC__metadata_chain_delete				== NIL ||
	    ex_FLAC__metadata_chain_read				== NIL ||
	    ex_FLAC__metadata_chain_write				== NIL ||
	    ex_FLAC__metadata_chain_sort_padding			== NIL ||

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

	    ex_FLAC__VENDOR_STRING					== NIL ||
	    ex_FLAC__VERSION_STRING					== NIL) { FreeFLACDLL(); return False; }

	return True;
}

Void FreeFLACDLL()
{
	Object::DeleteObject(flacdll);

	flacdll = NIL;
}
