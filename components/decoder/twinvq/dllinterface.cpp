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

TVQINITIALIZE			 ex_TvqInitialize		= NIL;
TVQTERMINATE			 ex_TvqTerminate		= NIL;
TVQGETFRAMESIZE			 ex_TvqGetFrameSize		= NIL;
TVQGETNUMCHANNELS		 ex_TvqGetNumChannels		= NIL;
TVQGETBITRATE			 ex_TvqGetBitRate		= NIL;
TVQGETSAMPLINGRATE		 ex_TvqGetSamplingRate		= NIL;
TVQDECODEFRAME			 ex_TvqDecodeFrame		= NIL;
TVQGETVECTORINFO		 ex_TvqGetVectorInfo		= NIL;
TVQWTYPETOBTYPE			 ex_TvqWtypeToBtype		= NIL;
TVQUPDATEVECTORINFO		 ex_TvqUpdateVectorInfo		= NIL;
TVQCHECKVERSION			 ex_TvqCheckVersion		= NIL;
TVQGETCONFINFO			 ex_TvqGetConfInfo		= NIL;
TVQGETNUMFIXEDBITSPERFRAME	 ex_TvqGetNumFixedBitsPerFrame	= NIL;

DynamicLoader *twinvqdll	= NIL;

Bool LoadTwinVQDLL()
{
	twinvqdll = BoCA::Utilities::LoadCodecDLL("TVQdec");

	if (twinvqdll == NIL) return False;

	ex_TvqInitialize		= (TVQINITIALIZE) twinvqdll->GetFunctionAddress("TvqInitialize");
	ex_TvqTerminate			= (TVQTERMINATE) twinvqdll->GetFunctionAddress("TvqTerminate");
	ex_TvqGetFrameSize		= (TVQGETFRAMESIZE) twinvqdll->GetFunctionAddress("TvqGetFrameSize");
	ex_TvqGetNumChannels		= (TVQGETNUMCHANNELS) twinvqdll->GetFunctionAddress("TvqGetNumChannels");
	ex_TvqGetBitRate		= (TVQGETBITRATE) twinvqdll->GetFunctionAddress("TvqGetBitRate");
	ex_TvqGetSamplingRate		= (TVQGETSAMPLINGRATE) twinvqdll->GetFunctionAddress("TvqGetSamplingRate");
	ex_TvqDecodeFrame		= (TVQDECODEFRAME) twinvqdll->GetFunctionAddress("TvqDecodeFrame");
	ex_TvqGetVectorInfo		= (TVQGETVECTORINFO) twinvqdll->GetFunctionAddress("TvqGetVectorInfo");
	ex_TvqWtypeToBtype		= (TVQWTYPETOBTYPE) twinvqdll->GetFunctionAddress("TvqWtypeToBtype");
	ex_TvqUpdateVectorInfo		= (TVQUPDATEVECTORINFO) twinvqdll->GetFunctionAddress("TvqUpdateVectorInfo");
	ex_TvqCheckVersion		= (TVQCHECKVERSION) twinvqdll->GetFunctionAddress("TvqCheckVersion");
	ex_TvqGetConfInfo		= (TVQGETCONFINFO) twinvqdll->GetFunctionAddress("TvqGetConfInfo");
	ex_TvqGetNumFixedBitsPerFrame	= (TVQGETNUMFIXEDBITSPERFRAME) twinvqdll->GetFunctionAddress("TvqGetNumFixedBitsPerFrame");

	if (ex_TvqInitialize			== NIL ||
	    ex_TvqTerminate			== NIL ||
	    ex_TvqGetFrameSize			== NIL ||
	    ex_TvqGetNumChannels		== NIL ||
	    ex_TvqGetBitRate			== NIL ||
	    ex_TvqGetSamplingRate		== NIL ||
	    ex_TvqDecodeFrame			== NIL ||
	    ex_TvqGetVectorInfo			== NIL ||
	    ex_TvqWtypeToBtype			== NIL ||
	    ex_TvqUpdateVectorInfo		== NIL ||
	    ex_TvqCheckVersion			== NIL ||
	    ex_TvqGetConfInfo			== NIL ||
	    ex_TvqGetNumFixedBitsPerFrame	== NIL) { FreeTwinVQDLL(); return False; }

	return True;
}

Void FreeTwinVQDLL()
{
	Object::DeleteObject(twinvqdll);

	twinvqdll = NIL;
}
