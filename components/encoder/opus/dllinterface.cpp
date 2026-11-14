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

OPUSMULTISTREAMSURROUNDENCODERCREATE	 ex_opus_multistream_surround_encoder_create	= NIL;
OPUSMULTISTREAMENCODE			 ex_opus_multistream_encode			= NIL;
OPUSMULTISTREAMENCODEFLOAT		 ex_opus_multistream_encode_float		= NIL;
OPUSMULTISTREAMENCODERCTL		 ex_opus_multistream_encoder_ctl		= NIL;
OPUSMULTISTREAMENCODERDESTROY		 ex_opus_multistream_encoder_destroy		= NIL;
OPUSGETVERSIONSTRING			 ex_opus_get_version_string			= NIL;

DynamicLoader *opusdll	= NIL;

Bool LoadOpusDLL()
{
	opusdll = BoCA::Utilities::LoadCodecDLL("opus");

	if (opusdll == NIL) return False;

	ex_opus_multistream_surround_encoder_create	= (OPUSMULTISTREAMSURROUNDENCODERCREATE) opusdll->GetFunctionAddress("opus_multistream_surround_encoder_create");
	ex_opus_multistream_encode			= (OPUSMULTISTREAMENCODE) opusdll->GetFunctionAddress("opus_multistream_encode");
	ex_opus_multistream_encode_float		= (OPUSMULTISTREAMENCODEFLOAT) opusdll->GetFunctionAddress("opus_multistream_encode_float");
	ex_opus_multistream_encoder_ctl			= (OPUSMULTISTREAMENCODERCTL) opusdll->GetFunctionAddress("opus_multistream_encoder_ctl");
	ex_opus_multistream_encoder_destroy		= (OPUSMULTISTREAMENCODERDESTROY) opusdll->GetFunctionAddress("opus_multistream_encoder_destroy");
	ex_opus_get_version_string			= (OPUSGETVERSIONSTRING) opusdll->GetFunctionAddress("opus_get_version_string");

	if (ex_opus_multistream_surround_encoder_create	== NIL ||
	    ex_opus_multistream_encode			== NIL ||
	    ex_opus_multistream_encode_float		== NIL ||
	    ex_opus_multistream_encoder_ctl		== NIL ||
	    ex_opus_multistream_encoder_destroy		== NIL ||
	    ex_opus_get_version_string			== NIL) { FreeOpusDLL(); return False; }

	return True;
}

Void FreeOpusDLL()
{
	BoCA::Utilities::FreeCodecDLL(opusdll);

	opusdll = NIL;
}
