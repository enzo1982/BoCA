 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

MP4CREATECALLBACKS		 ex_MP4CreateCallbacks		= NIL;
MP4CLOSE			 ex_MP4Close			= NIL;
MP4OPTIMIZE			 ex_MP4Optimize			= NIL;

MP4SETTRACKESCONFIGURATION	 ex_MP4SetTrackESConfiguration	= NIL;
MP4SETAUDIOPROFILELEVEL		 ex_MP4SetAudioProfileLevel	= NIL;
MP4ADDAUDIOTRACK		 ex_MP4AddAudioTrack		= NIL;
MP4WRITESAMPLE			 ex_MP4WriteSample		= NIL;

MP4ITMFITEMALLOC		 ex_MP4ItmfItemAlloc		= NIL;
MP4ITMFITEMFREE			 ex_MP4ItmfItemFree		= NIL;
MP4ITMFADDITEM			 ex_MP4ItmfAddItem		= NIL;

DynamicLoader *voaacencdll	= NIL;
DynamicLoader *mp4v2dll		= NIL;

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

Bool LoadMP4v2DLL()
{
	mp4v2dll = BoCA::Utilities::LoadCodecDLL("mp4v2");

	if (mp4v2dll == NIL) return False;

	ex_MP4CreateCallbacks		= (MP4CREATECALLBACKS) mp4v2dll->GetFunctionAddress("MP4CreateCallbacks");
	ex_MP4Close			= (MP4CLOSE) mp4v2dll->GetFunctionAddress("MP4Close");
	ex_MP4Optimize			= (MP4OPTIMIZE) mp4v2dll->GetFunctionAddress("MP4Optimize");

	ex_MP4SetTrackESConfiguration	= (MP4SETTRACKESCONFIGURATION) mp4v2dll->GetFunctionAddress("MP4SetTrackESConfiguration");
	ex_MP4SetAudioProfileLevel	= (MP4SETAUDIOPROFILELEVEL) mp4v2dll->GetFunctionAddress("MP4SetAudioProfileLevel");
	ex_MP4AddAudioTrack		= (MP4ADDAUDIOTRACK) mp4v2dll->GetFunctionAddress("MP4AddAudioTrack");
	ex_MP4WriteSample		= (MP4WRITESAMPLE) mp4v2dll->GetFunctionAddress("MP4WriteSample");

	ex_MP4ItmfItemAlloc		= (MP4ITMFITEMALLOC) mp4v2dll->GetFunctionAddress("MP4ItmfItemAlloc");
	ex_MP4ItmfItemFree		= (MP4ITMFITEMFREE) mp4v2dll->GetFunctionAddress("MP4ItmfItemFree");
	ex_MP4ItmfAddItem		= (MP4ITMFADDITEM) mp4v2dll->GetFunctionAddress("MP4ItmfAddItem");

	if (ex_MP4CreateCallbacks		== NIL ||
	    ex_MP4Close				== NIL ||
	    ex_MP4Optimize			== NIL ||

	    ex_MP4SetTrackESConfiguration	== NIL ||
	    ex_MP4SetAudioProfileLevel		== NIL ||
	    ex_MP4AddAudioTrack			== NIL ||
	    ex_MP4WriteSample			== NIL ||

	    ex_MP4ItmfItemAlloc			== NIL ||
	    ex_MP4ItmfItemFree			== NIL ||
	    ex_MP4ItmfAddItem			== NIL) { FreeMP4v2DLL(); return False; }

	return True;
}

Void FreeMP4v2DLL()
{
	BoCA::Utilities::FreeCodecDLL(mp4v2dll);

	mp4v2dll = NIL;
}
