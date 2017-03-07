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

#include <boca.h>
#include "dllinterface.h"

using namespace BoCA;

OGGSTREAMINIT		 ex_ogg_stream_init		= NIL;
OGGSTREAMPACKETOUT	 ex_ogg_stream_packetout	= NIL;
OGGSTREAMPAGEIN		 ex_ogg_stream_pagein		= NIL;
OGGPAGEEOS		 ex_ogg_page_eos		= NIL;
OGGPAGESERIALNO		 ex_ogg_page_serialno		= NIL;
OGGSTREAMCLEAR		 ex_ogg_stream_clear		= NIL;
OGGSYNCINIT		 ex_ogg_sync_init		= NIL;
OGGSYNCBUFFER		 ex_ogg_sync_buffer		= NIL;
OGGSYNCWROTE		 ex_ogg_sync_wrote		= NIL;
OGGSYNCPAGEOUT		 ex_ogg_sync_pageout		= NIL;
OGGSYNCCLEAR		 ex_ogg_sync_clear		= NIL;

FLAC_API_SUPPORTS_OGG_FLAC_TYPE				 ex_FLAC_API_SUPPORTS_OGG_FLAC				= NIL;

FLAC__STREAM_DECODER_NEW				 ex_FLAC__stream_decoder_new				= NIL;
FLAC__STREAM_DECODER_DELETE				 ex_FLAC__stream_decoder_delete				= NIL;
FLAC__STREAM_DECODER_SET_METADATA_RESPOND		 ex_FLAC__stream_decoder_set_metadata_respond		= NIL;
FLAC__STREAM_DECODER_INIT_STREAM			 ex_FLAC__stream_decoder_init_stream			= NIL;
FLAC__STREAM_DECODER_INIT_OGG_STREAM			 ex_FLAC__stream_decoder_init_ogg_stream		= NIL;
FLAC__STREAM_DECODER_FINISH				 ex_FLAC__stream_decoder_finish				= NIL;
FLAC__STREAM_DECODER_GET_CHANNELS			 ex_FLAC__stream_decoder_get_channels			= NIL;
FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE		 ex_FLAC__stream_decoder_get_bits_per_sample		= NIL;
FLAC__STREAM_DECODER_GET_SAMPLE_RATE			 ex_FLAC__stream_decoder_get_sample_rate		= NIL;
FLAC__STREAM_DECODER_SEEK_ABSOLUTE			 ex_FLAC__stream_decoder_seek_absolute			= NIL;
FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA	 ex_FLAC__stream_decoder_process_until_end_of_metadata	= NIL;
FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM	 ex_FLAC__stream_decoder_process_until_end_of_stream	= NIL;

FLAC__VERSION_STRING_TYPE				 ex_FLAC__VERSION_STRING				= NIL;

DynamicLoader *oggdll	= NIL;
DynamicLoader *flacdll	= NIL;

Bool LoadOggDLL()
{
	oggdll = BoCA::Utilities::LoadCodecDLL("ogg");

	if (oggdll == NIL) return False;

	ex_ogg_stream_init	= (OGGSTREAMINIT) oggdll->GetFunctionAddress("ogg_stream_init");
	ex_ogg_stream_packetout	= (OGGSTREAMPACKETOUT) oggdll->GetFunctionAddress("ogg_stream_packetout");
	ex_ogg_stream_pagein	= (OGGSTREAMPAGEIN) oggdll->GetFunctionAddress("ogg_stream_pagein");
	ex_ogg_page_eos		= (OGGPAGEEOS) oggdll->GetFunctionAddress("ogg_page_eos");
	ex_ogg_page_serialno	= (OGGPAGESERIALNO) oggdll->GetFunctionAddress("ogg_page_serialno");
	ex_ogg_stream_clear	= (OGGSTREAMCLEAR) oggdll->GetFunctionAddress("ogg_stream_clear");
	ex_ogg_sync_init	= (OGGSYNCINIT) oggdll->GetFunctionAddress("ogg_sync_init");
	ex_ogg_sync_buffer	= (OGGSYNCBUFFER) oggdll->GetFunctionAddress("ogg_sync_buffer");
	ex_ogg_sync_wrote	= (OGGSYNCWROTE) oggdll->GetFunctionAddress("ogg_sync_wrote");
	ex_ogg_sync_pageout	= (OGGSYNCPAGEOUT) oggdll->GetFunctionAddress("ogg_sync_pageout");
	ex_ogg_sync_clear	= (OGGSYNCCLEAR) oggdll->GetFunctionAddress("ogg_sync_clear");

	if (ex_ogg_stream_init		== NIL ||
	    ex_ogg_stream_packetout	== NIL ||
	    ex_ogg_stream_pagein	== NIL ||
	    ex_ogg_page_eos		== NIL ||
	    ex_ogg_page_serialno	== NIL ||
	    ex_ogg_stream_clear		== NIL ||
	    ex_ogg_sync_init		== NIL ||
	    ex_ogg_sync_buffer		== NIL ||
	    ex_ogg_sync_wrote		== NIL ||
	    ex_ogg_sync_pageout		== NIL ||
	    ex_ogg_sync_clear		== NIL) { FreeOggDLL(); return False; }

	return True;
}

Void FreeOggDLL()
{
	BoCA::Utilities::FreeCodecDLL(oggdll);

	oggdll = NIL;
}

Bool LoadFLACDLL()
{
	flacdll = BoCA::Utilities::LoadCodecDLL("FLAC");

	if (flacdll == NIL) return False;

	ex_FLAC_API_SUPPORTS_OGG_FLAC					= (FLAC_API_SUPPORTS_OGG_FLAC_TYPE) flacdll->GetFunctionAddress("FLAC_API_SUPPORTS_OGG_FLAC");

	ex_FLAC__stream_decoder_new				= (FLAC__STREAM_DECODER_NEW) flacdll->GetFunctionAddress("FLAC__stream_decoder_new");
	ex_FLAC__stream_decoder_delete				= (FLAC__STREAM_DECODER_DELETE) flacdll->GetFunctionAddress("FLAC__stream_decoder_delete");
	ex_FLAC__stream_decoder_set_metadata_respond		= (FLAC__STREAM_DECODER_SET_METADATA_RESPOND) flacdll->GetFunctionAddress("FLAC__stream_decoder_set_metadata_respond");
	ex_FLAC__stream_decoder_init_stream			= (FLAC__STREAM_DECODER_INIT_STREAM) flacdll->GetFunctionAddress("FLAC__stream_decoder_init_stream");
	ex_FLAC__stream_decoder_init_ogg_stream			= (FLAC__STREAM_DECODER_INIT_OGG_STREAM) flacdll->GetFunctionAddress("FLAC__stream_decoder_init_ogg_stream");
	ex_FLAC__stream_decoder_finish				= (FLAC__STREAM_DECODER_FINISH) flacdll->GetFunctionAddress("FLAC__stream_decoder_finish");
	ex_FLAC__stream_decoder_get_channels			= (FLAC__STREAM_DECODER_GET_CHANNELS) flacdll->GetFunctionAddress("FLAC__stream_decoder_get_channels");
	ex_FLAC__stream_decoder_get_bits_per_sample		= (FLAC__STREAM_DECODER_GET_BITS_PER_SAMPLE) flacdll->GetFunctionAddress("FLAC__stream_decoder_get_bits_per_sample");
	ex_FLAC__stream_decoder_get_sample_rate			= (FLAC__STREAM_DECODER_GET_SAMPLE_RATE) flacdll->GetFunctionAddress("FLAC__stream_decoder_get_sample_rate");
	ex_FLAC__stream_decoder_seek_absolute			= (FLAC__STREAM_DECODER_SEEK_ABSOLUTE) flacdll->GetFunctionAddress("FLAC__stream_decoder_seek_absolute");
	ex_FLAC__stream_decoder_process_until_end_of_metadata	= (FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_METADATA) flacdll->GetFunctionAddress("FLAC__stream_decoder_process_until_end_of_metadata");
	ex_FLAC__stream_decoder_process_until_end_of_stream	= (FLAC__STREAM_DECODER_PROCESS_UNTIL_END_OF_STREAM) flacdll->GetFunctionAddress("FLAC__stream_decoder_process_until_end_of_stream");

	ex_FLAC__VERSION_STRING					= (FLAC__VERSION_STRING_TYPE) flacdll->GetFunctionAddress("FLAC__VERSION_STRING");

	if (ex_FLAC_API_SUPPORTS_OGG_FLAC				== NIL ||

	    ex_FLAC__stream_decoder_new					== NIL ||
	    ex_FLAC__stream_decoder_delete				== NIL ||
	    ex_FLAC__stream_decoder_set_metadata_respond		== NIL ||
	    ex_FLAC__stream_decoder_init_stream				== NIL ||
	    ex_FLAC__stream_decoder_init_ogg_stream			== NIL ||
	    ex_FLAC__stream_decoder_finish				== NIL ||
	    ex_FLAC__stream_decoder_get_channels			== NIL ||
	    ex_FLAC__stream_decoder_get_bits_per_sample			== NIL ||
	    ex_FLAC__stream_decoder_get_sample_rate			== NIL ||
	    ex_FLAC__stream_decoder_seek_absolute			== NIL ||
	    ex_FLAC__stream_decoder_process_until_end_of_metadata	== NIL ||
	    ex_FLAC__stream_decoder_process_until_end_of_stream		== NIL ||

	    ex_FLAC__VERSION_STRING					== NIL) { FreeFLACDLL(); return False; }

	return True;
}

Void FreeFLACDLL()
{
	BoCA::Utilities::FreeCodecDLL(flacdll);

	flacdll = NIL;
}
