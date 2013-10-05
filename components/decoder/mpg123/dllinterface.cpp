 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
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

MPG123_INIT			 ex_mpg123_init			= NIL;
MPG123_EXIT			 ex_mpg123_exit			= NIL;

MPG123_NEW			 ex_mpg123_new			= NIL;
MPG123_DELETE			 ex_mpg123_delete		= NIL;

MPG123_OPEN_FEED		 ex_mpg123_open_feed		= NIL;
MPG123_DECODE			 ex_mpg123_decode		= NIL;
MPG123_FEED			 ex_mpg123_feed			= NIL;
MPG123_FEEDSEEK			 ex_mpg123_feedseek		= NIL;

MPG123_GETFORMAT		 ex_mpg123_getformat		= NIL;
MPG123_INFO			 ex_mpg123_info			= NIL;
MPG123_SPF			 ex_mpg123_spf			= NIL;

MPG123_SUPPORTED_DECODERS	 ex_mpg123_supported_decoders	= NIL;
MPG123_DECODER			 ex_mpg123_decoder		= NIL;

DynamicLoader *mpg123dll = NIL;

Bool LoadMPG123DLL()
{
	mpg123dll = BoCA::Utilities::LoadCodecDLL("mpg123");

	if (mpg123dll == NIL) return False;

	ex_mpg123_init			= (MPG123_INIT) mpg123dll->GetFunctionAddress("mpg123_init");
	ex_mpg123_exit			= (MPG123_EXIT) mpg123dll->GetFunctionAddress("mpg123_exit");

	ex_mpg123_new			= (MPG123_NEW) mpg123dll->GetFunctionAddress("mpg123_new");
	ex_mpg123_delete		= (MPG123_DELETE) mpg123dll->GetFunctionAddress("mpg123_delete");

	ex_mpg123_open_feed		= (MPG123_OPEN_FEED) mpg123dll->GetFunctionAddress("mpg123_open_feed");
	ex_mpg123_decode		= (MPG123_DECODE) mpg123dll->GetFunctionAddress("mpg123_decode");
	ex_mpg123_feed			= (MPG123_FEED) mpg123dll->GetFunctionAddress("mpg123_feed");
	ex_mpg123_feedseek		= (MPG123_FEEDSEEK) mpg123dll->GetFunctionAddress("mpg123_feedseek");

	ex_mpg123_getformat		= (MPG123_GETFORMAT) mpg123dll->GetFunctionAddress("mpg123_getformat");
	ex_mpg123_info			= (MPG123_INFO) mpg123dll->GetFunctionAddress("mpg123_info");
	ex_mpg123_spf			= (MPG123_SPF) mpg123dll->GetFunctionAddress("mpg123_spf");

	ex_mpg123_supported_decoders	= (MPG123_SUPPORTED_DECODERS) mpg123dll->GetFunctionAddress("mpg123_supported_decoders");
	ex_mpg123_decoder		= (MPG123_DECODER) mpg123dll->GetFunctionAddress("mpg123_decoder");

	if (ex_mpg123_init			== NIL ||
	    ex_mpg123_exit			== NIL ||

	    ex_mpg123_new			== NIL ||
	    ex_mpg123_delete			== NIL ||

	    ex_mpg123_open_feed			== NIL ||
	    ex_mpg123_decode			== NIL ||
	    ex_mpg123_feed			== NIL ||
	    ex_mpg123_feedseek			== NIL ||

	    ex_mpg123_getformat			== NIL ||
	    ex_mpg123_info			== NIL ||
	    ex_mpg123_spf			== NIL ||

	    ex_mpg123_supported_decoders	== NIL ||
	    ex_mpg123_decoder			== NIL) { FreeMPG123DLL(); return False; }

	return True;
}

Void FreeMPG123DLL()
{
	BoCA::Utilities::FreeCodecDLL(mpg123dll);

	mpg123dll = NIL;
}
