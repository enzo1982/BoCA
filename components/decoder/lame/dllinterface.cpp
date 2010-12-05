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

LAME_DECODE_INIT		 ex_lame_decode_init			= NIL;
LAME_DECODE_EXIT		 ex_lame_decode_exit			= NIL;
LAME_DECODE			 ex_lame_decode				= NIL;
LAME_DECODE_HEADERS		 ex_lame_decode_headers			= NIL;
GET_LAME_SHORT_VERSION		 ex_get_lame_short_version		= NIL;

DynamicLoader *lamedll	= NIL;

Bool LoadLAMEDLL()
{
#ifdef __WIN32__
	lamedll = BoCA::Utilities::LoadCodecDLL("LAME");
#else
	lamedll = BoCA::Utilities::LoadCodecDLL("mp3lame");
#endif

	if (lamedll == NIL) return False;

	ex_lame_decode_init			= (LAME_DECODE_INIT) lamedll->GetFunctionAddress("lame_decode_init");
	ex_lame_decode_exit			= (LAME_DECODE_EXIT) lamedll->GetFunctionAddress("lame_decode_exit");
	ex_lame_decode				= (LAME_DECODE) lamedll->GetFunctionAddress("lame_decode");
	ex_lame_decode_headers			= (LAME_DECODE_HEADERS) lamedll->GetFunctionAddress("lame_decode_headers");
	ex_get_lame_short_version		= (GET_LAME_SHORT_VERSION) lamedll->GetFunctionAddress("get_lame_short_version");

	if (ex_lame_decode_init			== NIL ||
	    ex_lame_decode_exit			== NIL ||
	    ex_lame_decode			== NIL ||
	    ex_lame_decode_headers		== NIL ||
	    ex_get_lame_short_version		== NIL) { FreeLAMEDLL(); return False; }

	return True;
}

Void FreeLAMEDLL()
{
	BoCA::Utilities::FreeCodecDLL(lamedll);

	lamedll = NIL;
}
