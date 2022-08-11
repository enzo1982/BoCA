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

MP4READ				 ex_MP4Read			= NIL;
MP4READCALLBACKS		 ex_MP4ReadCallbacks		= NIL;
MP4CLOSE			 ex_MP4Close			= NIL;
MP4FREE				 ex_MP4Free			= NIL;
MP4FINDTRACKID			 ex_MP4FindTrackId		= NIL;
MP4GETTRACKDURATION		 ex_MP4GetTrackDuration		= NIL;
MP4HAVETRACKATOM		 ex_MP4HaveTrackAtom		= NIL;
MP4GETTRACKBYTESPROPERTY	 ex_MP4GetTrackBytesProperty	= NIL;
MP4GETTRACKTIMESCALE		 ex_MP4GetTrackTimeScale	= NIL;
MP4GETSAMPLETIME		 ex_MP4GetSampleTime		= NIL;
MP4GETSAMPLEIDFROMTIME		 ex_MP4GetSampleIdFromTime	= NIL;
MP4GETSAMPLESIZE		 ex_MP4GetSampleSize		= NIL;
MP4READSAMPLE			 ex_MP4ReadSample		= NIL;

DynamicLoader *mp4v2dll		= NIL;

Bool LoadMP4v2DLL()
{
	mp4v2dll = BoCA::Utilities::LoadCodecDLL("mp4v2");

	if (mp4v2dll == NIL) return False;

	ex_MP4Read			= (MP4READ) mp4v2dll->GetFunctionAddress("MP4Read");
	ex_MP4ReadCallbacks		= (MP4READCALLBACKS) mp4v2dll->GetFunctionAddress("MP4ReadCallbacks");
	ex_MP4Close			= (MP4CLOSE) mp4v2dll->GetFunctionAddress("MP4Close");
	ex_MP4Free			= (MP4FREE) mp4v2dll->GetFunctionAddress("MP4Free");
	ex_MP4FindTrackId		= (MP4FINDTRACKID) mp4v2dll->GetFunctionAddress("MP4FindTrackId");
	ex_MP4GetTrackDuration		= (MP4GETTRACKDURATION) mp4v2dll->GetFunctionAddress("MP4GetTrackDuration");
	ex_MP4HaveTrackAtom		= (MP4HAVETRACKATOM) mp4v2dll->GetFunctionAddress("MP4HaveTrackAtom");
	ex_MP4GetTrackBytesProperty	= (MP4GETTRACKBYTESPROPERTY) mp4v2dll->GetFunctionAddress("MP4GetTrackBytesProperty");
	ex_MP4GetTrackTimeScale		= (MP4GETTRACKTIMESCALE) mp4v2dll->GetFunctionAddress("MP4GetTrackTimeScale");
	ex_MP4GetSampleTime		= (MP4GETSAMPLETIME) mp4v2dll->GetFunctionAddress("MP4GetSampleTime");
	ex_MP4GetSampleIdFromTime	= (MP4GETSAMPLEIDFROMTIME) mp4v2dll->GetFunctionAddress("MP4GetSampleIdFromTime");
	ex_MP4GetSampleSize		= (MP4GETSAMPLESIZE) mp4v2dll->GetFunctionAddress("MP4GetSampleSize");
	ex_MP4ReadSample		= (MP4READSAMPLE) mp4v2dll->GetFunctionAddress("MP4ReadSample");

	if (ex_MP4Read			== NIL ||
	    ex_MP4ReadCallbacks		== NIL ||
	    ex_MP4Close			== NIL ||
	    ex_MP4Free			== NIL ||
	    ex_MP4FindTrackId		== NIL ||
	    ex_MP4GetTrackDuration	== NIL ||
	    ex_MP4HaveTrackAtom		== NIL ||
	    ex_MP4GetTrackBytesProperty	== NIL ||
	    ex_MP4GetTrackTimeScale	== NIL ||
	    ex_MP4GetSampleTime		== NIL ||
	    ex_MP4GetSampleIdFromTime	== NIL ||
	    ex_MP4GetSampleSize		== NIL ||
	    ex_MP4ReadSample		== NIL) { FreeMP4v2DLL(); return False; }

	return True;
}

Void FreeMP4v2DLL()
{
	BoCA::Utilities::FreeCodecDLL(mp4v2dll);

	mp4v2dll = NIL;
}
