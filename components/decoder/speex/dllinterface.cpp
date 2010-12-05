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

SPEEXBITSINIT		 ex_speex_bits_init		= NIL;
SPEEXBITSDESTROY	 ex_speex_bits_destroy		= NIL;
SPEEXBITSREADFROM	 ex_speex_bits_read_from	= NIL;
SPEEXDECODERINIT	 ex_speex_decoder_init		= NIL;
SPEEXDECODERDESTROY	 ex_speex_decoder_destroy	= NIL;
SPEEXDECODERCTL		 ex_speex_decoder_ctl		= NIL;
SPEEXDECODEINT		 ex_speex_decode_int		= NIL;
SPEEXDECODESTEREOINT	 ex_speex_decode_stereo_int	= NIL;
SPEEXPACKETTOHEADER	 ex_speex_packet_to_header	= NIL;
SPEEXLIBGETMODE		 ex_speex_lib_get_mode		= NIL;

DynamicLoader *oggdll	= NIL;
DynamicLoader *speexdll	= NIL;

Bool LoadOggDLL()
{
	oggdll = BoCA::Utilities::LoadCodecDLL("ogg");

	if (oggdll == NIL) return False;

	ex_ogg_stream_init		= (OGGSTREAMINIT) oggdll->GetFunctionAddress("ogg_stream_init");
	ex_ogg_stream_packetout		= (OGGSTREAMPACKETOUT) oggdll->GetFunctionAddress("ogg_stream_packetout");
	ex_ogg_stream_pagein		= (OGGSTREAMPAGEIN) oggdll->GetFunctionAddress("ogg_stream_pagein");
	ex_ogg_page_eos			= (OGGPAGEEOS) oggdll->GetFunctionAddress("ogg_page_eos");
	ex_ogg_page_serialno		= (OGGPAGESERIALNO) oggdll->GetFunctionAddress("ogg_page_serialno");
	ex_ogg_stream_clear		= (OGGSTREAMCLEAR) oggdll->GetFunctionAddress("ogg_stream_clear");
	ex_ogg_sync_init		= (OGGSYNCINIT) oggdll->GetFunctionAddress("ogg_sync_init");
	ex_ogg_sync_buffer		= (OGGSYNCBUFFER) oggdll->GetFunctionAddress("ogg_sync_buffer");
	ex_ogg_sync_wrote		= (OGGSYNCWROTE) oggdll->GetFunctionAddress("ogg_sync_wrote");
	ex_ogg_sync_pageout		= (OGGSYNCPAGEOUT) oggdll->GetFunctionAddress("ogg_sync_pageout");
	ex_ogg_sync_clear		= (OGGSYNCCLEAR) oggdll->GetFunctionAddress("ogg_sync_clear");

	if (ex_ogg_stream_init			== NIL ||
	    ex_ogg_stream_packetout		== NIL ||
	    ex_ogg_stream_pagein		== NIL ||
	    ex_ogg_page_eos			== NIL ||
	    ex_ogg_page_serialno		== NIL ||
	    ex_ogg_stream_clear			== NIL ||
	    ex_ogg_sync_init			== NIL ||
	    ex_ogg_sync_buffer			== NIL ||
	    ex_ogg_sync_wrote			== NIL ||
	    ex_ogg_sync_pageout			== NIL ||
	    ex_ogg_sync_clear			== NIL) { FreeOggDLL(); return False; }

	return True;
}

Void FreeOggDLL()
{
	BoCA::Utilities::FreeCodecDLL(oggdll);

	oggdll = NIL;
}

Bool LoadSpeexDLL()
{
	speexdll = BoCA::Utilities::LoadCodecDLL("speex");

	if (speexdll == NIL) return False;

	ex_speex_bits_init		= (SPEEXBITSINIT) speexdll->GetFunctionAddress("speex_bits_init");
	ex_speex_bits_destroy		= (SPEEXBITSDESTROY) speexdll->GetFunctionAddress("speex_bits_destroy");
	ex_speex_bits_read_from		= (SPEEXBITSREADFROM) speexdll->GetFunctionAddress("speex_bits_read_from");
	ex_speex_decoder_init		= (SPEEXDECODERINIT) speexdll->GetFunctionAddress("speex_decoder_init");
	ex_speex_decoder_destroy	= (SPEEXDECODERDESTROY) speexdll->GetFunctionAddress("speex_decoder_destroy");
	ex_speex_decoder_ctl		= (SPEEXDECODERCTL) speexdll->GetFunctionAddress("speex_decoder_ctl");
	ex_speex_decode_int		= (SPEEXDECODEINT) speexdll->GetFunctionAddress("speex_decode_int");
	ex_speex_decode_stereo_int	= (SPEEXDECODESTEREOINT) speexdll->GetFunctionAddress("speex_decode_stereo_int");
	ex_speex_packet_to_header	= (SPEEXPACKETTOHEADER) speexdll->GetFunctionAddress("speex_packet_to_header");
	ex_speex_lib_get_mode		= (SPEEXLIBGETMODE) speexdll->GetFunctionAddress("speex_lib_get_mode");

	if (ex_speex_bits_init		== NIL ||
	    ex_speex_bits_destroy	== NIL ||
	    ex_speex_bits_read_from	== NIL ||
	    ex_speex_decoder_init	== NIL ||
	    ex_speex_decoder_destroy	== NIL ||
	    ex_speex_decoder_ctl	== NIL ||
	    ex_speex_decode_int		== NIL ||
	    ex_speex_decode_stereo_int	== NIL ||
	    ex_speex_packet_to_header	== NIL ||
	    ex_speex_lib_get_mode	== NIL) { FreeSpeexDLL(); return False; }

	return True;
}

Void FreeSpeexDLL()
{
	BoCA::Utilities::FreeCodecDLL(speexdll);

	speexdll = NIL;
}
