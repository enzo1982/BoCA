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

BEINITSTREAM	 ex_beInitStream	= NIL;
BEENCODECHUNK	 ex_beEncodeChunk	= NIL;
BEDEINITSTREAM	 ex_beDeinitStream	= NIL;
BECLOSESTREAM	 ex_beCloseStream	= NIL;
BEVERSION	 ex_beVersion		= NIL;

DynamicLoader *bladedll	= NIL;

Bool LoadBladeDLL()
{
	bladedll = BoCA::Utilities::LoadCodecDLL("BladeEnc");

	if (bladedll == NIL) return False;

	ex_beInitStream		= (BEINITSTREAM) bladedll->GetFunctionAddress("beInitStream");
	ex_beEncodeChunk	= (BEENCODECHUNK) bladedll->GetFunctionAddress("beEncodeChunk");
	ex_beDeinitStream	= (BEDEINITSTREAM) bladedll->GetFunctionAddress("beDeinitStream");
	ex_beCloseStream	= (BECLOSESTREAM) bladedll->GetFunctionAddress("beCloseStream");
	ex_beVersion		= (BEVERSION) bladedll->GetFunctionAddress("beVersion");

	if (ex_beInitStream	== NIL ||
	    ex_beEncodeChunk	== NIL ||
	    ex_beDeinitStream	== NIL ||
	    ex_beCloseStream	== NIL ||
	    ex_beVersion	== NIL) { FreeBladeDLL(); return False; }

	return True;
}

Void FreeBladeDLL()
{
	Object::DeleteObject(bladedll);

	bladedll = NIL;
}
