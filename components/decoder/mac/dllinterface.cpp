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

APEDECOMPRESS_CREATEW		 ex_APEDecompress_CreateW	= NIL;
APEDECOMPRESS_DESTROY		 ex_APEDecompress_Destroy	= NIL;
APEDECOMPRESS_SEEK		 ex_APEDecompress_Seek		= NIL;
APEDECOMPRESS_GETDATA		 ex_APEDecompress_GetData	= NIL;
APEDECOMPRESS_GETINFO		 ex_APEDecompress_GetInfo	= NIL;

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

	ex_APEDecompress_CreateW	= (APEDECOMPRESS_CREATEW) macdll->GetFunctionAddress("c_APEDecompress_CreateW");
	ex_APEDecompress_Destroy	= (APEDECOMPRESS_DESTROY) macdll->GetFunctionAddress("c_APEDecompress_Destroy");
	ex_APEDecompress_Seek		= (APEDECOMPRESS_SEEK) macdll->GetFunctionAddress("c_APEDecompress_Seek");
	ex_APEDecompress_GetData	= (APEDECOMPRESS_GETDATA) macdll->GetFunctionAddress("c_APEDecompress_GetData");
	ex_APEDecompress_GetInfo	= (APEDECOMPRESS_GETINFO) macdll->GetFunctionAddress("c_APEDecompress_GetInfo");

	ex_GetLibraryVersionString	= (GETLIBRARYVERSIONSTRING) macdll->GetFunctionAddress("GetLibraryVersionString");
	ex_GetLibraryInterfaceVersion	= (GETLIBRARYINTERFACEVERSION) macdll->GetFunctionAddress("GetLibraryInterfaceVersion");

	if (ex_APEDecompress_CreateW		== NIL ||
	    ex_APEDecompress_Destroy		== NIL ||
	    ex_APEDecompress_Seek		== NIL ||
	    ex_APEDecompress_GetData		== NIL ||
	    ex_APEDecompress_GetInfo		== NIL ||

	    ex_GetLibraryVersionString		== NIL ||
	    ex_GetLibraryInterfaceVersion	== NIL) { FreeMACDLL(); return False; }

	/* Check library interface version.
	 */
	unsigned int	 interfaceVersion = ex_GetLibraryInterfaceVersion();

	if (interfaceVersion != 9) { FreeMACDLL(); return False; }

	return True;
}

Void FreeMACDLL()
{
	Object::DeleteObject(macdll);

	macdll = NIL;
}
