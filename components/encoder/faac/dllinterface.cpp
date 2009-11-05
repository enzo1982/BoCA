 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

using namespace BoCA;

FAACENCOPEN			 ex_faacEncOpen				= NIL;
FAACENCGETCURRENTCONFIGURATION	 ex_faacEncGetCurrentConfiguration	= NIL;
FAACENCSETCONFIGURATION		 ex_faacEncSetConfiguration		= NIL;
FAACENCGETDECODERSPECIFICINFO	 ex_faacEncGetDecoderSpecificInfo	= NIL;
FAACENCENCODE			 ex_faacEncEncode			= NIL;
FAACENCCLOSE			 ex_faacEncClose			= NIL;

MP4CREATEEX			 ex_MP4CreateEx				= NIL;
MP4CLOSE			 ex_MP4Close				= NIL;
MP4SETTRACKESCONFIGURATION	 ex_MP4SetTrackESConfiguration		= NIL;
MP4SETAUDIOPROFILELEVEL		 ex_MP4SetAudioProfileLevel		= NIL;
MP4ADDAUDIOTRACK		 ex_MP4AddAudioTrack			= NIL;
MP4WRITESAMPLE			 ex_MP4WriteSample			= NIL;

DynamicLoader *faacdll	= NIL;
DynamicLoader *mp4v2dll	= NIL;

Bool LoadFAACDLL()
{
	if (Config::Get()->GetIntValue("OpenMP", "EnableOpenMP", True) &&
	    CPU().GetNumCores() >= 2 && CPU().HasSSE3()) faacdll = new DynamicLoader("codecs/FAAC-OpenMP");
	else						 faacdll = new DynamicLoader("codecs/FAAC");

	ex_faacEncOpen				= (FAACENCOPEN) faacdll->GetFunctionAddress("faacEncOpen");
	ex_faacEncGetCurrentConfiguration	= (FAACENCGETCURRENTCONFIGURATION) faacdll->GetFunctionAddress("faacEncGetCurrentConfiguration");
	ex_faacEncSetConfiguration		= (FAACENCSETCONFIGURATION) faacdll->GetFunctionAddress("faacEncSetConfiguration");
	ex_faacEncGetDecoderSpecificInfo	= (FAACENCGETDECODERSPECIFICINFO) faacdll->GetFunctionAddress("faacEncGetDecoderSpecificInfo");
	ex_faacEncEncode			= (FAACENCENCODE) faacdll->GetFunctionAddress("faacEncEncode");
	ex_faacEncClose				= (FAACENCCLOSE) faacdll->GetFunctionAddress("faacEncClose");

	if (ex_faacEncOpen			== NIL ||
	    ex_faacEncGetCurrentConfiguration	== NIL ||
	    ex_faacEncSetConfiguration		== NIL ||
	    ex_faacEncGetDecoderSpecificInfo	== NIL ||
	    ex_faacEncEncode			== NIL ||
	    ex_faacEncClose			== NIL) { FreeFAACDLL(); return False; }

	return True;
}

Void FreeFAACDLL()
{
	Object::DeleteObject(faacdll);

	faacdll = NIL;
}

Bool LoadMP4v2DLL()
{
	mp4v2dll = new DynamicLoader("codecs/MP4v2");

	ex_MP4CreateEx			= (MP4CREATEEX) mp4v2dll->GetFunctionAddress("MP4CreateEx");
	ex_MP4Close			= (MP4CLOSE) mp4v2dll->GetFunctionAddress("MP4Close");
	ex_MP4SetTrackESConfiguration	= (MP4SETTRACKESCONFIGURATION) mp4v2dll->GetFunctionAddress("MP4SetTrackESConfiguration");
	ex_MP4SetAudioProfileLevel	= (MP4SETAUDIOPROFILELEVEL) mp4v2dll->GetFunctionAddress("MP4SetAudioProfileLevel");
	ex_MP4AddAudioTrack		= (MP4ADDAUDIOTRACK) mp4v2dll->GetFunctionAddress("MP4AddAudioTrack");
	ex_MP4WriteSample		= (MP4WRITESAMPLE) mp4v2dll->GetFunctionAddress("MP4WriteSample");

	if (ex_MP4CreateEx			== NIL ||
	    ex_MP4Close				== NIL ||
	    ex_MP4SetTrackESConfiguration	== NIL ||
	    ex_MP4SetAudioProfileLevel		== NIL ||
	    ex_MP4AddAudioTrack			== NIL ||
	    ex_MP4WriteSample			== NIL) { FreeMP4v2DLL(); return False; }

	return True;
}

Void FreeMP4v2DLL()
{
	Object::DeleteObject(mp4v2dll);

	mp4v2dll = NIL;
}
