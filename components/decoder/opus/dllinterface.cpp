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

using namespace BoCA;

OPUSMULTISTREAMDECODERCREATE	 ex_opus_multistream_decoder_create	= NIL;
OPUSMULTISTREAMDECODE		 ex_opus_multistream_decode		= NIL;
OPUSMULTISTREAMDECODEFLOAT	 ex_opus_multistream_decode_float	= NIL;
OPUSMULTISTREAMDECODERCTL	 ex_opus_multistream_decoder_ctl	= NIL;
OPUSMULTISTREAMDECODERDESTROY	 ex_opus_multistream_decoder_destroy	= NIL;
OPUSDECODERCREATE		 ex_opus_decoder_create			= NIL;
OPUSDECODERCTL			 ex_opus_decoder_ctl			= NIL;
OPUSDECODERDESTROY		 ex_opus_decoder_destroy		= NIL;
OPUSGETVERSIONSTRING		 ex_opus_get_version_string		= NIL;

DynamicLoader *opusdll	= NIL;

Bool LoadOpusDLL()
{
	opusdll = BoCA::Utilities::LoadCodecDLL("opus");

	if (opusdll == NIL) return False;

	ex_opus_multistream_decoder_create	= (OPUSMULTISTREAMDECODERCREATE) opusdll->GetFunctionAddress("opus_multistream_decoder_create");
	ex_opus_multistream_decode		= (OPUSMULTISTREAMDECODE) opusdll->GetFunctionAddress("opus_multistream_decode");
	ex_opus_multistream_decode_float	= (OPUSMULTISTREAMDECODEFLOAT) opusdll->GetFunctionAddress("opus_multistream_decode_float");
	ex_opus_multistream_decoder_ctl		= (OPUSMULTISTREAMDECODERCTL) opusdll->GetFunctionAddress("opus_multistream_decoder_ctl");
	ex_opus_multistream_decoder_destroy	= (OPUSMULTISTREAMDECODERDESTROY) opusdll->GetFunctionAddress("opus_multistream_decoder_destroy");
	ex_opus_decoder_create			= (OPUSDECODERCREATE) opusdll->GetFunctionAddress("opus_decoder_create");
	ex_opus_decoder_ctl			= (OPUSDECODERCTL) opusdll->GetFunctionAddress("opus_decoder_ctl");
	ex_opus_decoder_destroy			= (OPUSDECODERDESTROY) opusdll->GetFunctionAddress("opus_decoder_destroy");
	ex_opus_get_version_string		= (OPUSGETVERSIONSTRING) opusdll->GetFunctionAddress("opus_get_version_string");

	if (ex_opus_multistream_decoder_create	== NIL ||
	    ex_opus_multistream_decode		== NIL ||
	    ex_opus_multistream_decode_float	== NIL ||
	    ex_opus_multistream_decoder_ctl	== NIL ||
	    ex_opus_multistream_decoder_destroy	== NIL ||
	    ex_opus_decoder_create		== NIL ||
	    ex_opus_decoder_ctl			== NIL ||
	    ex_opus_decoder_destroy		== NIL ||
	    ex_opus_get_version_string		== NIL) { FreeOpusDLL(); return False; }

	return True;
}

Void FreeOpusDLL()
{
	BoCA::Utilities::FreeCodecDLL(opusdll);

	opusdll = NIL;
}
