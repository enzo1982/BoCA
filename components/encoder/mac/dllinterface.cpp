 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2023 Robert Kausch <robert.kausch@freac.org>
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

APECOMPRESS_CREATE		 ex_APECompress_Create		= NIL;
APECOMPRESS_DESTROY		 ex_APECompress_Destroy		= NIL;
APECOMPRESS_STARTW		 ex_APECompress_StartW		= NIL;
APECOMPRESS_ADDDATA		 ex_APECompress_AddData		= NIL;
APECOMPRESS_FINISH		 ex_APECompress_Finish		= NIL;

GETLIBRARYVERSIONSTRING		 ex_GetLibraryVersionString	= NIL;
GETLIBRARYINTERFACEVERSION	 ex_GetLibraryInterfaceVersion	= NIL;

DynamicLoader *macdll	= NIL;

Bool LoadMACDLL()
{
#ifdef __WIN32__
	macdll = BoCA::Utilities::LoadCodecDLL("MACDll");
#else
	macdll = BoCA::Utilities::LoadCodecDLL("MAC");
#endif

	if (macdll == NIL) return False;

	ex_APECompress_Create		= (APECOMPRESS_CREATE) macdll->GetFunctionAddress("c_APECompress_Create");
	ex_APECompress_Destroy		= (APECOMPRESS_DESTROY) macdll->GetFunctionAddress("c_APECompress_Destroy");
	ex_APECompress_StartW		= (APECOMPRESS_STARTW) macdll->GetFunctionAddress("c_APECompress_StartW");
	ex_APECompress_AddData		= (APECOMPRESS_ADDDATA) macdll->GetFunctionAddress("c_APECompress_AddData");
	ex_APECompress_Finish		= (APECOMPRESS_FINISH) macdll->GetFunctionAddress("c_APECompress_Finish");

	ex_GetLibraryVersionString	= (GETLIBRARYVERSIONSTRING) macdll->GetFunctionAddress("GetLibraryVersionString");
	ex_GetLibraryInterfaceVersion	= (GETLIBRARYINTERFACEVERSION) macdll->GetFunctionAddress("GetLibraryInterfaceVersion");

	if (ex_APECompress_Create		== NIL ||
	    ex_APECompress_Destroy		== NIL ||
	    ex_APECompress_StartW		== NIL ||
	    ex_APECompress_AddData		== NIL ||
	    ex_APECompress_Finish		== NIL ||

	    ex_GetLibraryVersionString		== NIL ||
	    ex_GetLibraryInterfaceVersion	== NIL) { FreeMACDLL(); return False; }

	/* Check library interface version.
	 */
	unsigned int	 interfaceVersion = ex_GetLibraryInterfaceVersion();

	if (interfaceVersion > 10) { FreeMACDLL(); return False; }

	return True;
}

Void FreeMACDLL()
{
	Object::DeleteObject(macdll);

	macdll = NIL;
}
