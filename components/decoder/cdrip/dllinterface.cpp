 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

CR_INIT				 ex_CR_Init			= NIL;
CR_DEINIT			 ex_CR_DeInit			= NIL;
CR_ISINITIALIZED		 ex_CR_IsInitialized		= NIL;
CR_READTOC			 ex_CR_ReadToc			= NIL;
CR_GETNUMTOCENTRIES		 ex_CR_GetNumTocEntries		= NIL;
CR_GETTOCENTRY			 ex_CR_GetTocEntry		= NIL;
CR_OPENRIPPER			 ex_CR_OpenRipper		= NIL;
CR_CLOSERIPPER			 ex_CR_CloseRipper		= NIL;
CR_RIPCHUNK			 ex_CR_RipChunk			= NIL;
CR_GETNUMCDROM			 ex_CR_GetNumCDROM		= NIL;
CR_SETACTIVECDROM		 ex_CR_SetActiveCDROM		= NIL;
CR_GETCDROMPARAMETERS		 ex_CR_GetCDROMParameters	= NIL;
CR_SETCDROMPARAMETERS		 ex_CR_SetCDROMParameters	= NIL;
CR_LOCKCD			 ex_CR_LockCD			= NIL;
CR_READCDTEXT			 ex_CR_ReadCDText		= NIL;
CR_READANDGETISRC		 ex_CR_ReadAndGetISRC		= NIL;

DynamicLoader *cdripdll	= NIL;

Bool LoadCDRipDLL()
{
#ifdef __WIN32__
	if (!File(String(GUI::Application::GetApplicationDirectory()).Append("CDRip.dll")).Exists()) return False;
#endif

	cdripdll = new DynamicLoader("CDRip");

	ex_CR_Init			= (CR_INIT) cdripdll->GetFunctionAddress("CR_Init");
	ex_CR_DeInit			= (CR_DEINIT) cdripdll->GetFunctionAddress("CR_DeInit");
	ex_CR_IsInitialized		= (CR_ISINITIALIZED) cdripdll->GetFunctionAddress("CR_IsInitialized");
	ex_CR_ReadToc			= (CR_READTOC) cdripdll->GetFunctionAddress("CR_ReadToc");
	ex_CR_GetNumTocEntries		= (CR_GETNUMTOCENTRIES) cdripdll->GetFunctionAddress("CR_GetNumTocEntries");
	ex_CR_GetTocEntry		= (CR_GETTOCENTRY) cdripdll->GetFunctionAddress("CR_GetTocEntry");
	ex_CR_OpenRipper		= (CR_OPENRIPPER) cdripdll->GetFunctionAddress("CR_OpenRipper");
	ex_CR_CloseRipper		= (CR_CLOSERIPPER) cdripdll->GetFunctionAddress("CR_CloseRipper");
	ex_CR_RipChunk			= (CR_RIPCHUNK) cdripdll->GetFunctionAddress("CR_RipChunk");
	ex_CR_GetNumCDROM		= (CR_GETNUMCDROM) cdripdll->GetFunctionAddress("CR_GetNumCDROM");
	ex_CR_SetActiveCDROM		= (CR_SETACTIVECDROM) cdripdll->GetFunctionAddress("CR_SetActiveCDROM");
	ex_CR_GetCDROMParameters	= (CR_GETCDROMPARAMETERS) cdripdll->GetFunctionAddress("CR_GetCDROMParameters");
	ex_CR_SetCDROMParameters	= (CR_SETCDROMPARAMETERS) cdripdll->GetFunctionAddress("CR_SetCDROMParameters");
	ex_CR_LockCD			= (CR_LOCKCD) cdripdll->GetFunctionAddress("CR_LockCD");
	ex_CR_ReadCDText		= (CR_READCDTEXT) cdripdll->GetFunctionAddress("CR_ReadCDText");
	ex_CR_ReadAndGetISRC		= (CR_READANDGETISRC) cdripdll->GetFunctionAddress("CR_ReadAndGetISRC");

	if (ex_CR_Init				== NIL ||
	    ex_CR_DeInit			== NIL ||
	    ex_CR_IsInitialized			== NIL ||
	    ex_CR_ReadToc			== NIL ||
	    ex_CR_GetNumTocEntries		== NIL ||
	    ex_CR_GetTocEntry			== NIL ||
	    ex_CR_OpenRipper			== NIL ||
	    ex_CR_CloseRipper			== NIL ||
	    ex_CR_RipChunk			== NIL ||
	    ex_CR_GetNumCDROM			== NIL ||
	    ex_CR_SetActiveCDROM		== NIL ||
	    ex_CR_GetCDROMParameters		== NIL ||
	    ex_CR_SetCDROMParameters		== NIL ||
	    ex_CR_LockCD			== NIL ||
	    ex_CR_ReadCDText			== NIL ||
	    ex_CR_ReadAndGetISRC		== NIL) { FreeCDRipDLL(); return False; }

	return True;
}

Void FreeCDRipDLL()
{
	Object::DeleteObject(cdripdll);

	cdripdll = NIL;
}
