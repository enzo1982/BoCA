 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

FLAC__STREAM_DECODER_NEW				 ex_FLAC__stream_decoder_new				= NIL;
FLAC__STREAM_DECODER_DELETE				 ex_FLAC__stream_decoder_delete				= NIL;
FLAC__STREAM_DECODER_SET_METADATA_RESPOND		 ex_FLAC__stream_decoder_set_metadata_respond		= NIL;
FLAC__STREAM_DECODER_INIT_STREAM			 ex_FLAC__stream_decoder_init_stream			= NIL;
FLAC__STREAM_DECODER_FINISH				 ex_FLAC__stream_decoder_finish				= NIL;
FLAC__STREAM_DECODER_GET_CHANNELS			 ex_FLAC__stream_decoder_get_channels			= NIL;
FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE		 ex_FLAC__stream_decoder_get_bits_per_sample		= NIL;
FLAC__STREAM_DECODER_GET_SAMPLE_RATE			 ex_FLAC__stream_decoder_get_sample_rate		= NIL;
FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA	 ex_FLAC__stream_decoder_process_until_end_of_metadata	= NIL;
FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM	 ex_FLAC__stream_decoder_process_until_end_of_stream	= NIL;
FLAC__VERSION_STRING_TYPE				 ex_FLAC__VERSION_STRING				= NIL;

DynamicLoader *flacdll	= NIL;

Bool LoadFLACDLL()
{
	flacdll = new DynamicLoader("codecs/FLAC");

	ex_FLAC__stream_decoder_new				= (FLAC__STREAM_DECODER_NEW) flacdll->GetFunctionAddress("FLAC__stream_decoder_new");
	ex_FLAC__stream_decoder_delete				= (FLAC__STREAM_DECODER_DELETE) flacdll->GetFunctionAddress("FLAC__stream_decoder_delete");
	ex_FLAC__stream_decoder_set_metadata_respond		= (FLAC__STREAM_DECODER_SET_METADATA_RESPOND) flacdll->GetFunctionAddress("FLAC__stream_decoder_set_metadata_respond");
	ex_FLAC__stream_decoder_init_stream			= (FLAC__STREAM_DECODER_INIT_STREAM) flacdll->GetFunctionAddress("FLAC__stream_decoder_init_stream");
	ex_FLAC__stream_decoder_finish				= (FLAC__STREAM_DECODER_FINISH) flacdll->GetFunctionAddress("FLAC__stream_decoder_finish");
	ex_FLAC__stream_decoder_get_channels			= (FLAC__STREAM_DECODER_GET_CHANNELS) flacdll->GetFunctionAddress("FLAC__stream_decoder_get_channels");
	ex_FLAC__stream_decoder_get_bits_per_sample		= (FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE) flacdll->GetFunctionAddress("FLAC__stream_decoder_get_bits_per_sample");
	ex_FLAC__stream_decoder_get_sample_rate			= (FLAC__STREAM_DECODER_GET_SAMPLE_RATE) flacdll->GetFunctionAddress("FLAC__stream_decoder_get_sample_rate");
	ex_FLAC__stream_decoder_process_until_end_of_metadata	= (FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA) flacdll->GetFunctionAddress("FLAC__stream_decoder_process_until_end_of_metadata");
	ex_FLAC__stream_decoder_process_until_end_of_stream	= (FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM) flacdll->GetFunctionAddress("FLAC__stream_decoder_process_until_end_of_stream");
	ex_FLAC__VERSION_STRING					= (FLAC__VERSION_STRING_TYPE) flacdll->GetFunctionAddress("FLAC__VERSION_STRING");

	if (ex_FLAC__stream_decoder_new					== NIL ||
	    ex_FLAC__stream_decoder_delete				== NIL ||
	    ex_FLAC__stream_decoder_set_metadata_respond		== NIL ||
	    ex_FLAC__stream_decoder_init_stream				== NIL ||
	    ex_FLAC__stream_decoder_finish				== NIL ||
	    ex_FLAC__stream_decoder_get_channels			== NIL ||
	    ex_FLAC__stream_decoder_get_bits_per_sample			== NIL ||
	    ex_FLAC__stream_decoder_get_sample_rate			== NIL ||
	    ex_FLAC__stream_decoder_process_until_end_of_metadata	== NIL ||
	    ex_FLAC__stream_decoder_process_until_end_of_stream		== NIL ||
	    ex_FLAC__VERSION_STRING					== NIL) { FreeFLACDLL(); return False; }

	return True;
}

Void FreeFLACDLL()
{
	Object::DeleteObject(flacdll);

	flacdll = NIL;
}
