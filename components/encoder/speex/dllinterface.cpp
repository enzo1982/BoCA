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

SPEEXBITSINIT			 ex_speex_bits_init			= NIL;
SPEEXBITSDESTROY		 ex_speex_bits_destroy			= NIL;
SPEEXBITSRESET			 ex_speex_bits_reset			= NIL;
SPEEXBITSNBYTES			 ex_speex_bits_nbytes			= NIL;
SPEEXBITSINSERTTERMINATOR	 ex_speex_bits_insert_terminator	= NIL;
SPEEXBITSWRITE			 ex_speex_bits_write			= NIL;
SPEEXENCODERINIT		 ex_speex_encoder_init			= NIL;
SPEEXENCODERDESTROY		 ex_speex_encoder_destroy		= NIL;
SPEEXENCODERCTL			 ex_speex_encoder_ctl			= NIL;
SPEEXENCODEINT			 ex_speex_encode_int			= NIL;
SPEEXENCODESTEREOINT		 ex_speex_encode_stereo_int		= NIL;
SPEEXINITHEADER			 ex_speex_init_header			= NIL;
SPEEXHEADERTOPACKET		 ex_speex_header_to_packet		= NIL;
SPEEXHEADERFREE			 ex_speex_header_free			= NIL;
SPEEXLIBCTL			 ex_speex_lib_ctl			= NIL;
SPEEXLIBGETMODE			 ex_speex_lib_get_mode			= NIL;

DynamicLoader *speexdll	= NIL;

Bool LoadSpeexDLL()
{
	speexdll = BoCA::Utilities::LoadCodecDLL("speex");

	if (speexdll == NIL) return False;

	ex_speex_bits_init		= (SPEEXBITSINIT) speexdll->GetFunctionAddress("speex_bits_init");
	ex_speex_bits_destroy		= (SPEEXBITSDESTROY) speexdll->GetFunctionAddress("speex_bits_destroy");
	ex_speex_bits_reset		= (SPEEXBITSRESET) speexdll->GetFunctionAddress("speex_bits_reset");
	ex_speex_bits_nbytes		= (SPEEXBITSNBYTES) speexdll->GetFunctionAddress("speex_bits_nbytes");
	ex_speex_bits_insert_terminator	= (SPEEXBITSINSERTTERMINATOR) speexdll->GetFunctionAddress("speex_bits_insert_terminator");
	ex_speex_bits_write		= (SPEEXBITSWRITE) speexdll->GetFunctionAddress("speex_bits_write");
	ex_speex_encoder_init		= (SPEEXENCODERINIT) speexdll->GetFunctionAddress("speex_encoder_init");
	ex_speex_encoder_destroy	= (SPEEXENCODERDESTROY) speexdll->GetFunctionAddress("speex_encoder_destroy");
	ex_speex_encoder_ctl		= (SPEEXENCODERCTL) speexdll->GetFunctionAddress("speex_encoder_ctl");
	ex_speex_encode_int		= (SPEEXENCODEINT) speexdll->GetFunctionAddress("speex_encode_int");
	ex_speex_encode_stereo_int	= (SPEEXENCODESTEREOINT) speexdll->GetFunctionAddress("speex_encode_stereo_int");
	ex_speex_init_header		= (SPEEXINITHEADER) speexdll->GetFunctionAddress("speex_init_header");
	ex_speex_header_to_packet	= (SPEEXHEADERTOPACKET) speexdll->GetFunctionAddress("speex_header_to_packet");
	ex_speex_header_free		= (SPEEXHEADERFREE) speexdll->GetFunctionAddress("speex_header_free");
	ex_speex_lib_ctl		= (SPEEXLIBCTL) speexdll->GetFunctionAddress("speex_lib_ctl");
	ex_speex_lib_get_mode		= (SPEEXLIBGETMODE) speexdll->GetFunctionAddress("speex_lib_get_mode");

	if (ex_speex_bits_init			== NIL ||
	    ex_speex_bits_destroy		== NIL ||
	    ex_speex_bits_reset			== NIL ||
	    ex_speex_bits_nbytes		== NIL ||
	    ex_speex_bits_insert_terminator	== NIL ||
	    ex_speex_bits_write			== NIL ||
	    ex_speex_encoder_init		== NIL ||
	    ex_speex_encoder_destroy		== NIL ||
	    ex_speex_encoder_ctl		== NIL ||
	    ex_speex_encode_int			== NIL ||
	    ex_speex_encode_stereo_int		== NIL ||
	    ex_speex_init_header		== NIL ||
	    ex_speex_header_to_packet		== NIL ||
	    ex_speex_header_free		== NIL ||
	    ex_speex_lib_ctl			== NIL ||
	    ex_speex_lib_get_mode		== NIL) { FreeSpeexDLL(); return False; }

	return True;
}

Void FreeSpeexDLL()
{
	BoCA::Utilities::FreeCodecDLL(speexdll);

	speexdll = NIL;
}
