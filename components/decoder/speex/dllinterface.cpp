 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
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

SPEEXBITSINIT		 ex_speex_bits_init		= NIL;
SPEEXBITSDESTROY	 ex_speex_bits_destroy		= NIL;
SPEEXBITSREADFROM	 ex_speex_bits_read_from	= NIL;
SPEEXDECODERINIT	 ex_speex_decoder_init		= NIL;
SPEEXDECODERDESTROY	 ex_speex_decoder_destroy	= NIL;
SPEEXDECODERCTL		 ex_speex_decoder_ctl		= NIL;
SPEEXDECODEINT		 ex_speex_decode_int		= NIL;
SPEEXDECODESTEREOINT	 ex_speex_decode_stereo_int	= NIL;
SPEEXPACKETTOHEADER	 ex_speex_packet_to_header	= NIL;
SPEEXLIBCTL		 ex_speex_lib_ctl		= NIL;
SPEEXLIBGETMODE		 ex_speex_lib_get_mode		= NIL;

DynamicLoader *speexdll	= NIL;

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
	ex_speex_lib_ctl		= (SPEEXLIBCTL) speexdll->GetFunctionAddress("speex_lib_ctl");
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
	    ex_speex_lib_ctl		== NIL ||
	    ex_speex_lib_get_mode	== NIL) { FreeSpeexDLL(); return False; }

	return True;
}

Void FreeSpeexDLL()
{
	BoCA::Utilities::FreeCodecDLL(speexdll);

	speexdll = NIL;
}
