 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

RUBBERBAND_NEW			 ex_rubberband_new			= NIL;
RUBBERBAND_DELETE		 ex_rubberband_delete			= NIL;
RUBBERBAND_GET_LATENCY		 ex_rubberband_get_latency		= NIL;
RUBBERBAND_GET_SAMPLES_REQUIRED	 ex_rubberband_get_samples_required	= NIL;
RUBBERBAND_PROCESS		 ex_rubberband_process			= NIL;
RUBBERBAND_AVAILABLE		 ex_rubberband_available		= NIL;
RUBBERBAND_RETRIEVE		 ex_rubberband_retrieve			= NIL;

DynamicLoader *rbdll	= NIL;

Bool LoadRubberBandDLL()
{
	rbdll = BoCA::Utilities::LoadCodecDLL("rubberband");

	if (rbdll == NIL) return False;

	ex_rubberband_new			= (RUBBERBAND_NEW) rbdll->GetFunctionAddress("rubberband_new");
	ex_rubberband_delete			= (RUBBERBAND_DELETE) rbdll->GetFunctionAddress("rubberband_delete");
	ex_rubberband_get_latency		= (RUBBERBAND_GET_LATENCY) rbdll->GetFunctionAddress("rubberband_get_latency");
	ex_rubberband_get_samples_required	= (RUBBERBAND_GET_SAMPLES_REQUIRED) rbdll->GetFunctionAddress("rubberband_get_samples_required");
	ex_rubberband_process			= (RUBBERBAND_PROCESS) rbdll->GetFunctionAddress("rubberband_process");
	ex_rubberband_available			= (RUBBERBAND_AVAILABLE) rbdll->GetFunctionAddress("rubberband_available");
	ex_rubberband_retrieve			= (RUBBERBAND_RETRIEVE) rbdll->GetFunctionAddress("rubberband_retrieve");

	if (ex_rubberband_new			== NIL ||
	    ex_rubberband_delete		== NIL ||
	    ex_rubberband_get_latency		== NIL ||
	    ex_rubberband_get_samples_required	== NIL ||
	    ex_rubberband_process		== NIL ||
	    ex_rubberband_available		== NIL ||
	    ex_rubberband_retrieve		== NIL) { FreeRubberBandDLL(); return False; }

	return True;
}

Void FreeRubberBandDLL()
{
	BoCA::Utilities::FreeCodecDLL(rbdll);

	rbdll = NIL;
}
