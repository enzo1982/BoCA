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

SRC_NEW			 ex_src_new		= NIL;
SRC_DELETE		 ex_src_delete		= NIL;
SRC_PROCESS		 ex_src_process		= NIL;
SRC_GET_NAME		 ex_src_get_name	= NIL;
SRC_GET_DESCRIPTION	 ex_src_get_description	= NIL;
SRC_STRERROR		 ex_src_strerror	= NIL;

DynamicLoader *srcdll	= NIL;

Bool LoadSRCDLL()
{
	srcdll = BoCA::Utilities::LoadCodecDLL("samplerate");

	if (srcdll == NIL) return False;

	ex_src_new		= (SRC_NEW) srcdll->GetFunctionAddress("src_new");
	ex_src_delete		= (SRC_DELETE) srcdll->GetFunctionAddress("src_delete");
	ex_src_process		= (SRC_PROCESS) srcdll->GetFunctionAddress("src_process");
	ex_src_get_name		= (SRC_GET_NAME) srcdll->GetFunctionAddress("src_get_name");
	ex_src_get_description	= (SRC_GET_DESCRIPTION) srcdll->GetFunctionAddress("src_get_description");
	ex_src_strerror		= (SRC_STRERROR) srcdll->GetFunctionAddress("src_strerror");

	if (ex_src_new			== NIL ||
	    ex_src_delete		== NIL ||
	    ex_src_process		== NIL ||
	    ex_src_get_name		== NIL ||
	    ex_src_get_description	== NIL ||
	    ex_src_strerror		== NIL) { FreeSRCDLL(); return False; }

	return True;
}

Void FreeSRCDLL()
{
	BoCA::Utilities::FreeCodecDLL(srcdll);

	srcdll = NIL;
}
