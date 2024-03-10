 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2024 Robert Kausch <robert.kausch@freac.org>
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

OGGSTREAMINIT			 ex_ogg_stream_init			= NIL;
OGGSTREAMPACKETOUT		 ex_ogg_stream_packetout		= NIL;
OGGSTREAMPAGEIN			 ex_ogg_stream_pagein			= NIL;
OGGPAGEEOS			 ex_ogg_page_eos			= NIL;
OGGPAGESERIALNO			 ex_ogg_page_serialno			= NIL;
OGGPAGEGRANULEPOS		 ex_ogg_page_granulepos			= NIL;
OGGSTREAMCLEAR			 ex_ogg_stream_clear			= NIL;
OGGSYNCINIT			 ex_ogg_sync_init			= NIL;
OGGSYNCBUFFER			 ex_ogg_sync_buffer			= NIL;
OGGSYNCWROTE			 ex_ogg_sync_wrote			= NIL;
OGGSYNCPAGEOUT			 ex_ogg_sync_pageout			= NIL;
OGGSYNCPAGESEEK			 ex_ogg_sync_pageseek			= NIL;
OGGSYNCRESET			 ex_ogg_sync_reset			= NIL;
OGGSYNCCLEAR			 ex_ogg_sync_clear			= NIL;

OPUSMULTISTREAMDECODERCREATE	 ex_opus_multistream_decoder_create	= NIL;
OPUSMULTISTREAMDECODE		 ex_opus_multistream_decode		= NIL;
OPUSMULTISTREAMDECODERCTL	 ex_opus_multistream_decoder_ctl	= NIL;
OPUSMULTISTREAMDECODERDESTROY	 ex_opus_multistream_decoder_destroy	= NIL;
OPUSDECODERCTL			 ex_opus_decoder_ctl			= NIL;
OPUSGETVERSIONSTRING		 ex_opus_get_version_string		= NIL;

DynamicLoader *oggdll	= NIL;
DynamicLoader *opusdll	= NIL;

Bool LoadOggDLL()
{
	oggdll = BoCA::Utilities::LoadCodecDLL("ogg");

	if (oggdll == NIL) return False;

	ex_ogg_stream_init	= (OGGSTREAMINIT) oggdll->GetFunctionAddress("ogg_stream_init");
	ex_ogg_stream_packetout	= (OGGSTREAMPACKETOUT) oggdll->GetFunctionAddress("ogg_stream_packetout");
	ex_ogg_stream_pagein	= (OGGSTREAMPAGEIN) oggdll->GetFunctionAddress("ogg_stream_pagein");
	ex_ogg_page_eos		= (OGGPAGEEOS) oggdll->GetFunctionAddress("ogg_page_eos");
	ex_ogg_page_serialno	= (OGGPAGESERIALNO) oggdll->GetFunctionAddress("ogg_page_serialno");
	ex_ogg_page_granulepos	= (OGGPAGEGRANULEPOS) oggdll->GetFunctionAddress("ogg_page_granulepos");
	ex_ogg_stream_clear	= (OGGSTREAMCLEAR) oggdll->GetFunctionAddress("ogg_stream_clear");
	ex_ogg_sync_init	= (OGGSYNCINIT) oggdll->GetFunctionAddress("ogg_sync_init");
	ex_ogg_sync_buffer	= (OGGSYNCBUFFER) oggdll->GetFunctionAddress("ogg_sync_buffer");
	ex_ogg_sync_wrote	= (OGGSYNCWROTE) oggdll->GetFunctionAddress("ogg_sync_wrote");
	ex_ogg_sync_pageout	= (OGGSYNCPAGEOUT) oggdll->GetFunctionAddress("ogg_sync_pageout");
	ex_ogg_sync_pageseek	= (OGGSYNCPAGESEEK) oggdll->GetFunctionAddress("ogg_sync_pageseek");
	ex_ogg_sync_reset	= (OGGSYNCRESET) oggdll->GetFunctionAddress("ogg_sync_reset");
	ex_ogg_sync_clear	= (OGGSYNCCLEAR) oggdll->GetFunctionAddress("ogg_sync_clear");

	if (ex_ogg_stream_init		== NIL ||
	    ex_ogg_stream_packetout	== NIL ||
	    ex_ogg_stream_pagein	== NIL ||
	    ex_ogg_page_eos		== NIL ||
	    ex_ogg_page_serialno	== NIL ||
	    ex_ogg_page_granulepos	== NIL ||
	    ex_ogg_stream_clear		== NIL ||
	    ex_ogg_sync_init		== NIL ||
	    ex_ogg_sync_buffer		== NIL ||
	    ex_ogg_sync_wrote		== NIL ||
	    ex_ogg_sync_pageout		== NIL ||
	    ex_ogg_sync_pageseek	== NIL ||
	    ex_ogg_sync_reset		== NIL ||
	    ex_ogg_sync_clear		== NIL) { FreeOggDLL(); return False; }

	return True;
}

Void FreeOggDLL()
{
	BoCA::Utilities::FreeCodecDLL(oggdll);

	oggdll = NIL;
}

Bool LoadOpusDLL()
{
	opusdll = BoCA::Utilities::LoadCodecDLL("opus");

	if (opusdll == NIL) return False;

	ex_opus_multistream_decoder_create	= (OPUSMULTISTREAMDECODERCREATE) opusdll->GetFunctionAddress("opus_multistream_decoder_create");
	ex_opus_multistream_decode		= (OPUSMULTISTREAMDECODE) opusdll->GetFunctionAddress("opus_multistream_decode");
	ex_opus_multistream_decoder_ctl		= (OPUSMULTISTREAMDECODERCTL) opusdll->GetFunctionAddress("opus_multistream_decoder_ctl");
	ex_opus_multistream_decoder_destroy	= (OPUSMULTISTREAMDECODERDESTROY) opusdll->GetFunctionAddress("opus_multistream_decoder_destroy");
	ex_opus_decoder_ctl			= (OPUSDECODERCTL) opusdll->GetFunctionAddress("opus_decoder_ctl");
	ex_opus_get_version_string		= (OPUSGETVERSIONSTRING) opusdll->GetFunctionAddress("opus_get_version_string");

	if (ex_opus_multistream_decoder_create	== NIL ||
	    ex_opus_multistream_decode		== NIL ||
	    ex_opus_multistream_decoder_ctl	== NIL ||
	    ex_opus_multistream_decoder_destroy	== NIL ||
	    ex_opus_decoder_ctl			== NIL ||
	    ex_opus_get_version_string		== NIL) { FreeOpusDLL(); return False; }

	return True;
}

Void FreeOpusDLL()
{
	BoCA::Utilities::FreeCodecDLL(opusdll);

	opusdll = NIL;
}
