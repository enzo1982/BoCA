 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
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

VOGETAACENCAPI			 ex_voGetAACEncAPI		= NIL;

CMNMEMALLOC			 ex_cmnMemAlloc			= NIL;
CMNMEMFREE			 ex_cmnMemFree			= NIL;
CMNMEMSET			 ex_cmnMemSet			= NIL;
CMNMEMCOPY			 ex_cmnMemCopy			= NIL;
CMNMEMCHECK			 ex_cmnMemCheck			= NIL;
CMNMEMCOMPARE			 ex_cmnMemCompare		= NIL;
CMNMEMMOVE			 ex_cmnMemMove			= NIL;

DynamicLoader *voaacencdll	= NIL;

Bool LoadVOAACEncDLL()
{
	voaacencdll = BoCA::Utilities::LoadCodecDLL("vo-aacenc");

	if (voaacencdll == NIL) return False;

	ex_voGetAACEncAPI	= (VOGETAACENCAPI) voaacencdll->GetFunctionAddress("voGetAACEncAPI");

	ex_cmnMemAlloc		= (CMNMEMALLOC) voaacencdll->GetFunctionAddress("cmnMemAlloc");
	ex_cmnMemFree		= (CMNMEMFREE) voaacencdll->GetFunctionAddress("cmnMemFree");
	ex_cmnMemSet		= (CMNMEMSET) voaacencdll->GetFunctionAddress("cmnMemSet");
	ex_cmnMemCopy		= (CMNMEMCOPY) voaacencdll->GetFunctionAddress("cmnMemCopy");
	ex_cmnMemCheck		= (CMNMEMCHECK) voaacencdll->GetFunctionAddress("cmnMemCheck");
	ex_cmnMemCompare	= (CMNMEMCOMPARE) voaacencdll->GetFunctionAddress("cmnMemCompare");
	ex_cmnMemMove		= (CMNMEMMOVE) voaacencdll->GetFunctionAddress("cmnMemMove");

	if (ex_voGetAACEncAPI	== NIL ||

	    ex_cmnMemAlloc	== NIL ||
	    ex_cmnMemFree	== NIL ||
	    ex_cmnMemSet	== NIL ||
	    ex_cmnMemCopy	== NIL ||
	    ex_cmnMemCheck	== NIL ||
	    ex_cmnMemCompare	== NIL ||
	    ex_cmnMemMove	== NIL) { FreeVOAACEncDLL(); return False; }

	return True;
}

Void FreeVOAACEncDLL()
{
	BoCA::Utilities::FreeCodecDLL(voaacencdll);

	voaacencdll = NIL;
}
