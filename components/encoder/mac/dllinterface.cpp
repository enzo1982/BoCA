 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

APECOMPRESS_CREATE	 ex_APECompress_Create	= NIL;
APECOMPRESS_DESTROY	 ex_APECompress_Destroy	= NIL;
APECOMPRESS_START	 ex_APECompress_Start	= NIL;
APECOMPRESS_ADDDATA	 ex_APECompress_AddData	= NIL;
APECOMPRESS_FINISH	 ex_APECompress_Finish	= NIL;
APEGETVERSIONNUMBER	 ex_APEGetVersionNumber	= NIL;

DynamicLoader *macdll	= NIL;

Bool LoadMACDLL()
{
	macdll = new DynamicLoader("codecs/MACDll");

	ex_APECompress_Create	= (APECOMPRESS_CREATE) macdll->GetFunctionAddress("c_APECompress_Create");
	ex_APECompress_Destroy	= (APECOMPRESS_DESTROY) macdll->GetFunctionAddress("c_APECompress_Destroy");
	ex_APECompress_Start	= (APECOMPRESS_START) macdll->GetFunctionAddress("c_APECompress_Start");
	ex_APECompress_AddData	= (APECOMPRESS_ADDDATA) macdll->GetFunctionAddress("c_APECompress_AddData");
	ex_APECompress_Finish	= (APECOMPRESS_FINISH) macdll->GetFunctionAddress("c_APECompress_Finish");
	ex_APEGetVersionNumber	= (APEGETVERSIONNUMBER) macdll->GetFunctionAddress("GetVersionNumber");

	if (ex_APECompress_Create	== NIL ||
	    ex_APECompress_Destroy	== NIL ||
	    ex_APECompress_Start	== NIL ||
	    ex_APECompress_AddData	== NIL ||
	    ex_APECompress_Finish	== NIL ||
	    ex_APEGetVersionNumber	== NIL) { FreeMACDLL(); return False; }

	return True;
}

Void FreeMACDLL()
{
	Object::DeleteObject(macdll);

	macdll = NIL;
}
