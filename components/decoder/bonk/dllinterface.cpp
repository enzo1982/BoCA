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

BONKDECODERCREATE		 ex_bonk_decoder_create			= NIL;
BONKDECODERINIT			 ex_bonk_decoder_init			= NIL;
BONKDECODERDECODEPACKET		 ex_bonk_decoder_decode_packet		= NIL;
BONKDECODERFINISH		 ex_bonk_decoder_finish			= NIL;
BONKDECODERCLOSE		 ex_bonk_decoder_close			= NIL;
BONKDECODERGETID3DATA		 ex_bonk_decoder_get_id3_data		= NIL;
BONKDECODERINITSEEKTABLE	 ex_bonk_decoder_init_seektable		= NIL;
BONKDECODERSEEKTO		 ex_bonk_decoder_seek_to		= NIL;
BONKGETVERSIONSTRING		 ex_bonk_get_version_string		= NIL;

DynamicLoader *bonkdll	= NIL;

Bool LoadBonkDLL()
{
#ifdef __WIN32__
	bonkdll = BoCA::Utilities::LoadCodecDLL("Bonk");
#else
	bonkdll = BoCA::Utilities::LoadCodecDLL("bonk");
#endif

	if (bonkdll == NIL) return False;

	ex_bonk_decoder_create			= (BONKDECODERCREATE) bonkdll->GetFunctionAddress("bonk_decoder_create");
	ex_bonk_decoder_init			= (BONKDECODERINIT) bonkdll->GetFunctionAddress("bonk_decoder_init");
	ex_bonk_decoder_decode_packet		= (BONKDECODERDECODEPACKET) bonkdll->GetFunctionAddress("bonk_decoder_decode_packet");
	ex_bonk_decoder_finish			= (BONKDECODERFINISH) bonkdll->GetFunctionAddress("bonk_decoder_finish");
	ex_bonk_decoder_close			= (BONKDECODERCLOSE) bonkdll->GetFunctionAddress("bonk_decoder_close");
	ex_bonk_decoder_get_id3_data		= (BONKDECODERGETID3DATA) bonkdll->GetFunctionAddress("bonk_decoder_get_id3_data");
	ex_bonk_decoder_init_seektable		= (BONKDECODERINITSEEKTABLE) bonkdll->GetFunctionAddress("bonk_decoder_init_seektable");
	ex_bonk_decoder_seek_to			= (BONKDECODERSEEKTO) bonkdll->GetFunctionAddress("bonk_decoder_seek_to");
	ex_bonk_get_version_string		= (BONKGETVERSIONSTRING) bonkdll->GetFunctionAddress("bonk_get_version_string");

	if (ex_bonk_decoder_create			== NIL ||
	    ex_bonk_decoder_init			== NIL ||
	    ex_bonk_decoder_decode_packet		== NIL ||
	    ex_bonk_decoder_finish			== NIL ||
	    ex_bonk_decoder_close			== NIL ||
	    ex_bonk_decoder_get_id3_data		== NIL ||
	    ex_bonk_decoder_init_seektable		== NIL ||
	    ex_bonk_decoder_seek_to			== NIL ||
	    ex_bonk_get_version_string			== NIL) { FreeBonkDLL(); return False; }

	return True;
}

Void FreeBonkDLL()
{
	BoCA::Utilities::FreeCodecDLL(bonkdll);

	bonkdll = NIL;
}
