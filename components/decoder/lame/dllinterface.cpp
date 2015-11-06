 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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

HIP_DECODE_INIT		 ex_hip_decode_init	= NIL;
HIP_DECODE_EXIT		 ex_hip_decode_exit	= NIL;
HIP_DECODE		 ex_hip_decode		= NIL;
HIP_DECODE_HEADERS	 ex_hip_decode_headers	= NIL;

DynamicLoader *lamedll	= NIL;

Bool LoadLAMEDLL()
{
#ifdef __WIN32__
	lamedll = BoCA::Utilities::LoadCodecDLL("LAME");
#else
	lamedll = BoCA::Utilities::LoadCodecDLL("mp3lame");
#endif

	if (lamedll == NIL) return False;

	ex_hip_decode_init		= (HIP_DECODE_INIT) lamedll->GetFunctionAddress("hip_decode_init");
	ex_hip_decode_exit		= (HIP_DECODE_EXIT) lamedll->GetFunctionAddress("hip_decode_exit");
	ex_hip_decode			= (HIP_DECODE) lamedll->GetFunctionAddress("hip_decode");
	ex_hip_decode_headers		= (HIP_DECODE_HEADERS) lamedll->GetFunctionAddress("hip_decode_headers");

	if (ex_hip_decode_init		== NIL ||
	    ex_hip_decode_exit		== NIL ||
	    ex_hip_decode		== NIL ||
	    ex_hip_decode_headers	== NIL) { FreeLAMEDLL(); return False; }

	return True;
}

Void FreeLAMEDLL()
{
	BoCA::Utilities::FreeCodecDLL(lamedll);

	lamedll = NIL;
}
