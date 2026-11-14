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

AACENCOPEN			 ex_aacEncOpen			= NIL;
AACENCCLOSE			 ex_aacEncClose			= NIL;
AACENCENCODE			 ex_aacEncEncode		= NIL;
AACENCINFO			 ex_aacEncInfo			= NIL;
AACENCGETLIBINFO		 ex_aacEncGetLibInfo		= NIL;
AACENCODER_GETPARAM		 ex_aacEncoder_GetParam		= NIL;
AACENCODER_SETPARAM		 ex_aacEncoder_SetParam		= NIL;

DynamicLoader *fdkaacdll	= NIL;

Bool LoadFDKAACDLL()
{
	fdkaacdll = BoCA::Utilities::LoadCodecDLL("fdk-aac");

	if (fdkaacdll == NIL) return False;

	ex_aacEncOpen		= (AACENCOPEN) fdkaacdll->GetFunctionAddress("aacEncOpen");
	ex_aacEncClose		= (AACENCCLOSE) fdkaacdll->GetFunctionAddress("aacEncClose");
	ex_aacEncEncode		= (AACENCENCODE) fdkaacdll->GetFunctionAddress("aacEncEncode");
	ex_aacEncInfo		= (AACENCINFO) fdkaacdll->GetFunctionAddress("aacEncInfo");
	ex_aacEncGetLibInfo	= (AACENCGETLIBINFO) fdkaacdll->GetFunctionAddress("aacEncGetLibInfo");
	ex_aacEncoder_GetParam	= (AACENCODER_GETPARAM) fdkaacdll->GetFunctionAddress("aacEncoder_GetParam");
	ex_aacEncoder_SetParam	= (AACENCODER_SETPARAM) fdkaacdll->GetFunctionAddress("aacEncoder_SetParam");

	if (ex_aacEncOpen		== NIL ||
	    ex_aacEncClose		== NIL ||
	    ex_aacEncEncode		== NIL ||
	    ex_aacEncInfo		== NIL ||
	    ex_aacEncGetLibInfo		== NIL ||
	    ex_aacEncoder_GetParam	== NIL ||
	    ex_aacEncoder_SetParam	== NIL) { FreeFDKAACDLL(); return False; }

	return True;
}

Void FreeFDKAACDLL()
{
	BoCA::Utilities::FreeCodecDLL(fdkaacdll);

	fdkaacdll = NIL;
}
