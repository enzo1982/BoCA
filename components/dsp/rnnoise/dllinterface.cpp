 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

RNNOISE_CREATE		 ex_rnnoise_create		= NIL;
RNNOISE_DESTROY		 ex_rnnoise_destroy		= NIL;
RNNOISE_PROCESS_FRAME	 ex_rnnoise_process_frame	= NIL;
RNNOISE_MODEL_FROM_FILE	 ex_rnnoise_model_from_file	= NIL;
RNNOISE_MODEL_FREE	 ex_rnnoise_model_free		= NIL;

DynamicLoader *rnnoisedll	= NIL;

Bool LoadRNNoiseDLL()
{
	rnnoisedll = BoCA::Utilities::LoadCodecDLL("rnnoise");

	if (rnnoisedll == NIL) return False;

	ex_rnnoise_create		= (RNNOISE_CREATE) rnnoisedll->GetFunctionAddress("rnnoise_create");
	ex_rnnoise_destroy		= (RNNOISE_DESTROY) rnnoisedll->GetFunctionAddress("rnnoise_destroy");
	ex_rnnoise_process_frame	= (RNNOISE_PROCESS_FRAME) rnnoisedll->GetFunctionAddress("rnnoise_process_frame");
	ex_rnnoise_model_from_file	= (RNNOISE_MODEL_FROM_FILE) rnnoisedll->GetFunctionAddress("rnnoise_model_from_file");
	ex_rnnoise_model_free		= (RNNOISE_MODEL_FREE) rnnoisedll->GetFunctionAddress("rnnoise_model_free");

	if (ex_rnnoise_create		== NIL ||
	    ex_rnnoise_destroy		== NIL ||
	    ex_rnnoise_process_frame	== NIL ||
	    ex_rnnoise_model_from_file	== NIL ||
	    ex_rnnoise_model_free	== NIL) { FreeRNNoiseDLL(); return False; }

	return True;
}

Void FreeRNNoiseDLL()
{
	BoCA::Utilities::FreeCodecDLL(rnnoisedll);

	rnnoisedll = NIL;
}
