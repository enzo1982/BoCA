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

BONKENCODERCREATE		 ex_bonk_encoder_create			= NIL;
BONKENCODERINIT			 ex_bonk_encoder_init			= NIL;
BONKENCODERENCODEPACKET		 ex_bonk_encoder_encode_packet		= NIL;
BONKENCODERFINISH		 ex_bonk_encoder_finish			= NIL;
BONKENCODERCLOSE		 ex_bonk_encoder_close			= NIL;
BONKENCODERGETSAMPLECOUNT	 ex_bonk_encoder_get_sample_count	= NIL;
BONKENCODERGETSAMPLECOUNTOFFSET	 ex_bonk_encoder_get_sample_count_offset= NIL;
BONKENCODERSETID3DATA		 ex_bonk_encoder_set_id3_data		= NIL;
BONKGETVERSIONSTRING		 ex_bonk_get_version_string		= NIL;

DynamicLoader *bonkdll	= NIL;

Bool LoadBonkDLL()
{
	if (Config::Get()->GetIntValue("OpenMP", "EnableOpenMP", True) && CPU().GetNumCores() >= 2 && CPU().HasSSE3())
	{
		bonkdll = new DynamicLoader("codecs/Bonk-OpenMP");

		if (bonkdll->GetSystemModuleHandle() == NIL) FreeBonkDLL();
	}

	if (bonkdll == NIL) bonkdll = new DynamicLoader("codecs/Bonk");

	ex_bonk_encoder_create			= (BONKENCODERCREATE) bonkdll->GetFunctionAddress("bonk_encoder_create");
	ex_bonk_encoder_init			= (BONKENCODERINIT) bonkdll->GetFunctionAddress("bonk_encoder_init");
	ex_bonk_encoder_encode_packet		= (BONKENCODERENCODEPACKET) bonkdll->GetFunctionAddress("bonk_encoder_encode_packet");
	ex_bonk_encoder_finish			= (BONKENCODERFINISH) bonkdll->GetFunctionAddress("bonk_encoder_finish");
	ex_bonk_encoder_close			= (BONKENCODERCLOSE) bonkdll->GetFunctionAddress("bonk_encoder_close");
	ex_bonk_encoder_get_sample_count	= (BONKENCODERGETSAMPLECOUNT) bonkdll->GetFunctionAddress("bonk_encoder_get_sample_count");
	ex_bonk_encoder_get_sample_count_offset	= (BONKENCODERGETSAMPLECOUNTOFFSET) bonkdll->GetFunctionAddress("bonk_encoder_get_sample_count_offset");
	ex_bonk_encoder_set_id3_data		= (BONKENCODERSETID3DATA) bonkdll->GetFunctionAddress("bonk_encoder_set_id3_data");
	ex_bonk_get_version_string		= (BONKGETVERSIONSTRING) bonkdll->GetFunctionAddress("bonk_get_version_string");

	if (ex_bonk_encoder_create			== NIL ||
	    ex_bonk_encoder_init			== NIL ||
	    ex_bonk_encoder_encode_packet		== NIL ||
	    ex_bonk_encoder_finish			== NIL ||
	    ex_bonk_encoder_close			== NIL ||
	    ex_bonk_encoder_get_sample_count		== NIL ||
	    ex_bonk_encoder_get_sample_count_offset	== NIL ||
	    ex_bonk_encoder_set_id3_data		== NIL ||
	    ex_bonk_get_version_string			== NIL) { FreeBonkDLL(); return False; }

	return True;
}

Void FreeBonkDLL()
{
	Object::DeleteObject(bonkdll);

	bonkdll = NIL;
}
