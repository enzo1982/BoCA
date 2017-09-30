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

#include "dllinterface.h"

CR_INIT				 ex_CR_Init			= NIL;
CR_DEINIT			 ex_CR_DeInit			= NIL;
CR_ISINITIALIZED		 ex_CR_IsInitialized		= NIL;
CR_GETNUMCDROM			 ex_CR_GetNumCDROM		= NIL;
CR_OPENCDROM			 ex_CR_OpenCDROM		= NIL;
CR_CLOSECDROM			 ex_CR_CloseCDROM		= NIL;
CR_READTOC			 ex_CR_ReadToc			= NIL;
CR_GETNUMTOCENTRIES		 ex_CR_GetNumTocEntries		= NIL;
CR_GETTOCENTRY			 ex_CR_GetTocEntry		= NIL;
CR_GETCDROMPARAMETERS		 ex_CR_GetCDROMParameters	= NIL;
CR_ISMEDIALOADED		 ex_CR_IsMediaLoaded		= NIL;
CR_EJECTCD			 ex_CR_EjectCD			= NIL;

DynamicLoader *cdripdll	= NIL;

Bool LoadCDRipDLL()
{
#ifdef __WIN32__
	if (!File(GUI::Application::GetApplicationDirectory().Append("CDRip.dll")).Exists()) return False;
#endif

	cdripdll = new DynamicLoader("CDRip");

	ex_CR_Init			= (CR_INIT) cdripdll->GetFunctionAddress("CR_Init");
	ex_CR_DeInit			= (CR_DEINIT) cdripdll->GetFunctionAddress("CR_DeInit");
	ex_CR_IsInitialized		= (CR_ISINITIALIZED) cdripdll->GetFunctionAddress("CR_IsInitialized");
	ex_CR_GetNumCDROM		= (CR_GETNUMCDROM) cdripdll->GetFunctionAddress("CR_GetNumCDROM");
	ex_CR_OpenCDROM			= (CR_OPENCDROM) cdripdll->GetFunctionAddress("CR_OpenCDROM");
	ex_CR_CloseCDROM		= (CR_CLOSECDROM) cdripdll->GetFunctionAddress("CR_CloseCDROM");
	ex_CR_ReadToc			= (CR_READTOC) cdripdll->GetFunctionAddress("CR_ReadToc");
	ex_CR_GetNumTocEntries		= (CR_GETNUMTOCENTRIES) cdripdll->GetFunctionAddress("CR_GetNumTocEntries");
	ex_CR_GetTocEntry		= (CR_GETTOCENTRY) cdripdll->GetFunctionAddress("CR_GetTocEntry");
	ex_CR_GetCDROMParameters	= (CR_GETCDROMPARAMETERS) cdripdll->GetFunctionAddress("CR_GetCDROMParameters");
	ex_CR_IsMediaLoaded		= (CR_ISMEDIALOADED) cdripdll->GetFunctionAddress("CR_IsMediaLoaded");
	ex_CR_EjectCD			= (CR_EJECTCD) cdripdll->GetFunctionAddress("CR_EjectCD");

	if (ex_CR_Init				== NIL ||
	    ex_CR_DeInit			== NIL ||
	    ex_CR_IsInitialized			== NIL ||
	    ex_CR_GetNumCDROM			== NIL ||
	    ex_CR_OpenCDROM			== NIL ||
	    ex_CR_CloseCDROM			== NIL ||
	    ex_CR_ReadToc			== NIL ||
	    ex_CR_GetNumTocEntries		== NIL ||
	    ex_CR_GetTocEntry			== NIL ||
	    ex_CR_GetCDROMParameters		== NIL ||
	    ex_CR_IsMediaLoaded			== NIL ||
	    ex_CR_EjectCD			== NIL) { FreeCDRipDLL(); return False; }

	return True;
}

Void FreeCDRipDLL()
{
	Object::DeleteObject(cdripdll);

	cdripdll = NIL;
}
