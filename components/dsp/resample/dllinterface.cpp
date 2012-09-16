 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

SRC_NEW				 ex_src_new			= NIL;
SRC_DELETE			 ex_src_delete			= NIL;
SRC_PROCESS			 ex_src_process			= NIL;
SRC_SHORT_TO_FLOAT_ARRAY	 ex_src_short_to_float_array	= NIL;
SRC_INT_TO_FLOAT_ARRAY		 ex_src_int_to_float_array	= NIL;
SRC_FLOAT_TO_SHORT_ARRAY	 ex_src_float_to_short_array	= NIL;
SRC_FLOAT_TO_INT_ARRAY		 ex_src_float_to_int_array	= NIL;
SRC_GET_NAME			 ex_src_get_name		= NIL;
SRC_GET_DESCRIPTION		 ex_src_get_description		= NIL;
SRC_STRERROR			 ex_src_strerror		= NIL;

DynamicLoader *srcdll	= NIL;

Bool LoadSRCDLL()
{
	srcdll = BoCA::Utilities::LoadCodecDLL("samplerate");

	if (srcdll == NIL) return False;

	ex_src_new			= (SRC_NEW) srcdll->GetFunctionAddress("src_new");
	ex_src_delete			= (SRC_DELETE) srcdll->GetFunctionAddress("src_delete");
	ex_src_process			= (SRC_PROCESS) srcdll->GetFunctionAddress("src_process");
	ex_src_short_to_float_array	= (SRC_SHORT_TO_FLOAT_ARRAY) srcdll->GetFunctionAddress("src_short_to_float_array");
	ex_src_int_to_float_array	= (SRC_INT_TO_FLOAT_ARRAY) srcdll->GetFunctionAddress("src_int_to_float_array");
	ex_src_float_to_short_array	= (SRC_FLOAT_TO_SHORT_ARRAY) srcdll->GetFunctionAddress("src_float_to_short_array");
	ex_src_float_to_int_array	= (SRC_FLOAT_TO_INT_ARRAY) srcdll->GetFunctionAddress("src_float_to_int_array");
	ex_src_get_name			= (SRC_GET_NAME) srcdll->GetFunctionAddress("src_get_name");
	ex_src_get_description		= (SRC_GET_DESCRIPTION) srcdll->GetFunctionAddress("src_get_description");
	ex_src_strerror			= (SRC_STRERROR) srcdll->GetFunctionAddress("src_strerror");

	if (ex_src_new			== NIL ||
	    ex_src_delete		== NIL ||
	    ex_src_process		== NIL ||
	    ex_src_short_to_float_array	== NIL ||
	    ex_src_int_to_float_array	== NIL ||
	    ex_src_float_to_short_array	== NIL ||
	    ex_src_float_to_int_array	== NIL ||
	    ex_src_get_name		== NIL ||
	    ex_src_get_description	== NIL ||
	    ex_src_strerror		== NIL) { FreeSRCDLL(); return False; }

	return True;
}

Void FreeSRCDLL()
{
	Object::DeleteObject(srcdll);

	srcdll = NIL;
}
