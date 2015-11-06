 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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

WMCREATEWRITER		 ex_WMCreateWriter		= NIL;
WMCREATEWRITERFILESINK	 ex_WMCreateWriterFileSink	= NIL;
WMCREATEPROFILEMANAGER	 ex_WMCreateProfileManager	= NIL;

DynamicLoader *wmvcoredll	= NIL;

Bool LoadWMVCoreDLL()
{
	wmvcoredll = new DynamicLoader("WMVCore");

	ex_WMCreateWriter		= (WMCREATEWRITER) wmvcoredll->GetFunctionAddress("WMCreateWriter");
	ex_WMCreateWriterFileSink	= (WMCREATEWRITERFILESINK) wmvcoredll->GetFunctionAddress("WMCreateWriterFileSink");
	ex_WMCreateProfileManager	= (WMCREATEPROFILEMANAGER) wmvcoredll->GetFunctionAddress("WMCreateProfileManager");

	if (ex_WMCreateWriter		== NIL ||
	    ex_WMCreateWriterFileSink	== NIL ||
	    ex_WMCreateProfileManager	== NIL) { FreeWMVCoreDLL(); return False; }

	return True;
}

Void FreeWMVCoreDLL()
{
	Object::DeleteObject(wmvcoredll);

	wmvcoredll = NIL;
}
