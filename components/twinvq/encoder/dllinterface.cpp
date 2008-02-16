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

TVQGETVERSIONID			 ex_TvqGetVersionID			= NIL;
TVQENCINITIALIZE		 ex_TvqEncInitialize			= NIL;
TVQENCTERMINATE			 ex_TvqEncTerminate			= NIL;
TVQENCGETFRAMESIZE		 ex_TvqEncGetFrameSize			= NIL;
TVQENCGETNUMCHANNELS		 ex_TvqEncGetNumChannels		= NIL;
TVQENCGETCONFINFO		 ex_TvqEncGetConfInfo			= NIL;
TVQENCGETVECTORINFO		 ex_TvqEncGetVectorInfo			= NIL;
TVQENCUPDATEVECTORINFO		 ex_TvqEncUpdateVectorInfo		= NIL;
TVQENCODEFRAME			 ex_TvqEncodeFrame			= NIL;

DynamicLoader *twinvqdll	= NIL;

Bool LoadTwinVQDLL()
{
	twinvqdll = new DynamicLoader("encoders/TVQenc");

	ex_TvqGetVersionID		= (TVQGETVERSIONID) twinvqdll->GetFunctionAddress("TvqGetVersionID");
	ex_TvqEncInitialize		= (TVQENCINITIALIZE) twinvqdll->GetFunctionAddress("TvqEncInitialize");
	ex_TvqEncTerminate		= (TVQENCTERMINATE) twinvqdll->GetFunctionAddress("TvqEncTerminate");
	ex_TvqEncGetFrameSize		= (TVQENCGETFRAMESIZE) twinvqdll->GetFunctionAddress("TvqEncGetFrameSize");
	ex_TvqEncGetNumChannels		= (TVQENCGETNUMCHANNELS) twinvqdll->GetFunctionAddress("TvqEncGetNumChannels");
	ex_TvqEncGetConfInfo		= (TVQENCGETCONFINFO) twinvqdll->GetFunctionAddress("TvqEncGetConfInfo");
	ex_TvqEncGetVectorInfo		= (TVQENCGETVECTORINFO) twinvqdll->GetFunctionAddress("TvqEncGetVectorInfo");
	ex_TvqEncUpdateVectorInfo	= (TVQENCUPDATEVECTORINFO) twinvqdll->GetFunctionAddress("TvqEncUpdateVectorInfo");
	ex_TvqEncodeFrame		= (TVQENCODEFRAME) twinvqdll->GetFunctionAddress("TvqEncodeFrame");

	if (ex_TvqGetVersionID		== NIL ||
	    ex_TvqEncInitialize		== NIL ||
	    ex_TvqEncTerminate		== NIL ||
	    ex_TvqEncGetFrameSize	== NIL ||
	    ex_TvqEncGetNumChannels	== NIL ||
	    ex_TvqEncGetConfInfo	== NIL ||
	    ex_TvqEncGetVectorInfo	== NIL ||
	    ex_TvqEncUpdateVectorInfo	== NIL ||
	    ex_TvqEncodeFrame		== NIL) { FreeTwinVQDLL(); return False; }

	return True;
}

Void FreeTwinVQDLL()
{
	Object::DeleteObject(twinvqdll);
}
