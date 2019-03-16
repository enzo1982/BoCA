 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

MFSTARTUP				 ex_MFStartup				= NIL;
MFSHUTDOWN				 ex_MFShutdown				= NIL;
MFCREATEASYNCRESULT			 ex_MFCreateAsyncResult			= NIL;
MFINVOKECALLBACK			 ex_MFInvokeCallback			= NIL;
MFCREATEMEDIATYPE			 ex_MFCreateMediaType			= NIL;
MFINITMEDIATYPEFROMWAVEFORMATEX		 ex_MFInitMediaTypeFromWaveFormatEx	= NIL;

MFCREATESOURCEREADERFROMBYTESTREAM	 ex_MFCreateSourceReaderFromByteStream	= NIL;

DynamicLoader *mfplatdll	= NIL;
DynamicLoader *mfreadwritedll	= NIL;

Bool LoadMFPlatDLL()
{
	mfplatdll = new DynamicLoader("MFPlat");

	ex_MFStartup				= (MFSTARTUP) mfplatdll->GetFunctionAddress("MFStartup");
	ex_MFShutdown				= (MFSHUTDOWN) mfplatdll->GetFunctionAddress("MFShutdown");
	ex_MFCreateAsyncResult			= (MFCREATEASYNCRESULT) mfplatdll->GetFunctionAddress("MFCreateAsyncResult");
	ex_MFInvokeCallback			= (MFINVOKECALLBACK) mfplatdll->GetFunctionAddress("MFInvokeCallback");
	ex_MFCreateMediaType			= (MFCREATEMEDIATYPE) mfplatdll->GetFunctionAddress("MFCreateMediaType");
	ex_MFInitMediaTypeFromWaveFormatEx	= (MFINITMEDIATYPEFROMWAVEFORMATEX) mfplatdll->GetFunctionAddress("MFInitMediaTypeFromWaveFormatEx");

	if (ex_MFStartup			== NIL ||
	    ex_MFShutdown			== NIL ||
	    ex_MFCreateAsyncResult		== NIL ||
	    ex_MFInvokeCallback			== NIL ||
	    ex_MFCreateMediaType		== NIL ||
	    ex_MFInitMediaTypeFromWaveFormatEx	== NIL) { FreeMFPlatDLL(); return False; }

	return True;
}

Void FreeMFPlatDLL()
{
	Object::DeleteObject(mfplatdll);

	mfplatdll = NIL;
}

Bool LoadMFReadWriteDLL()
{
	mfreadwritedll = new DynamicLoader("MFReadWrite");

	ex_MFCreateSourceReaderFromByteStream = (MFCREATESOURCEREADERFROMBYTESTREAM) mfreadwritedll->GetFunctionAddress("MFCreateSourceReaderFromByteStream");

	if (ex_MFCreateSourceReaderFromByteStream == NIL) { FreeMFReadWriteDLL(); return False; }

	return True;
}

Void FreeMFReadWriteDLL()
{
	Object::DeleteObject(mfreadwritedll);

	mfreadwritedll = NIL;
}
