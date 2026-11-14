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

FAACENCOPEN			 ex_faacEncOpen				= NIL;
FAACENCGETCURRENTCONFIGURATION	 ex_faacEncGetCurrentConfiguration	= NIL;
FAACENCSETCONFIGURATION		 ex_faacEncSetConfiguration		= NIL;
FAACENCGETDECODERSPECIFICINFO	 ex_faacEncGetDecoderSpecificInfo	= NIL;
FAACENCENCODE			 ex_faacEncEncode			= NIL;
FAACENCCLOSE			 ex_faacEncClose			= NIL;

DynamicLoader *faacdll	= NIL;

Bool LoadFAACDLL()
{
	faacdll = BoCA::Utilities::LoadCodecDLL("faac");

	if (faacdll == NIL) return False;

	ex_faacEncOpen				= (FAACENCOPEN) faacdll->GetFunctionAddress("faacEncOpen");
	ex_faacEncGetCurrentConfiguration	= (FAACENCGETCURRENTCONFIGURATION) faacdll->GetFunctionAddress("faacEncGetCurrentConfiguration");
	ex_faacEncSetConfiguration		= (FAACENCSETCONFIGURATION) faacdll->GetFunctionAddress("faacEncSetConfiguration");
	ex_faacEncGetDecoderSpecificInfo	= (FAACENCGETDECODERSPECIFICINFO) faacdll->GetFunctionAddress("faacEncGetDecoderSpecificInfo");
	ex_faacEncEncode			= (FAACENCENCODE) faacdll->GetFunctionAddress("faacEncEncode");
	ex_faacEncClose				= (FAACENCCLOSE) faacdll->GetFunctionAddress("faacEncClose");

	if (ex_faacEncOpen			== NIL ||
	    ex_faacEncGetCurrentConfiguration	== NIL ||
	    ex_faacEncSetConfiguration		== NIL ||
	    ex_faacEncGetDecoderSpecificInfo	== NIL ||
	    ex_faacEncEncode			== NIL ||
	    ex_faacEncClose			== NIL) { FreeFAACDLL(); return False; }

	return True;
}

Void FreeFAACDLL()
{
	BoCA::Utilities::FreeCodecDLL(faacdll);

	faacdll = NIL;
}
