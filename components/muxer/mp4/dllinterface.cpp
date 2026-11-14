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

MP4READCALLBACKS		 ex_MP4ReadCallbacks		= NIL;
MP4CREATECALLBACKS		 ex_MP4CreateCallbacks		= NIL;
MP4MODIFYCALLBACKS		 ex_MP4ModifyCallbacks		= NIL;
MP4CLOSE			 ex_MP4Close			= NIL;
MP4FREE				 ex_MP4Free			= NIL;
MP4OPTIMIZE			 ex_MP4Optimize			= NIL;

MP4FINDTRACKID			 ex_MP4FindTrackId		= NIL;
MP4ADDAUDIOTRACK		 ex_MP4AddAudioTrack		= NIL;
MP4SETAUDIOPROFILELEVEL		 ex_MP4SetAudioProfileLevel	= NIL;

MP4HAVETRACKATOM		 ex_MP4HaveTrackAtom		= NIL;
MP4GETTRACKBYTESPROPERTY	 ex_MP4GetTrackBytesProperty	= NIL;
MP4SETTRACKINTEGERPROPERTY	 ex_MP4SetTrackIntegerProperty	= NIL;

MP4GETTRACKAUDIOMPEG4TYPE	 ex_MP4GetTrackAudioMpeg4Type	= NIL;
MP4GETTRACKAUDIOCHANNELS	 ex_MP4GetTrackAudioChannels	= NIL;
MP4GETTRACKESCONFIGURATION	 ex_MP4GetTrackESConfiguration	= NIL;
MP4SETTRACKESCONFIGURATION	 ex_MP4SetTrackESConfiguration	= NIL;
MP4GETTRACKDURATION		 ex_MP4GetTrackDuration		= NIL;
MP4GETTRACKTIMESCALE		 ex_MP4GetTrackTimeScale	= NIL;
MP4GETTRACKNUMBEROFSAMPLES	 ex_MP4GetTrackNumberOfSamples	= NIL;

MP4GETSAMPLETIME		 ex_MP4GetSampleTime		= NIL;
MP4GETSAMPLEIDFROMTIME		 ex_MP4GetSampleIdFromTime	= NIL;
MP4GETSAMPLESIZE		 ex_MP4GetSampleSize		= NIL;
MP4READSAMPLE			 ex_MP4ReadSample		= NIL;
MP4WRITESAMPLE			 ex_MP4WriteSample		= NIL;

MP4ITMFITEMALLOC		 ex_MP4ItmfItemAlloc		= NIL;
MP4ITMFITEMFREE			 ex_MP4ItmfItemFree		= NIL;
MP4ITMFADDITEM			 ex_MP4ItmfAddItem		= NIL;
MP4ITMFGETITEMSBYMEANING	 ex_MP4ItmfGetItemsByMeaning	= NIL;
MP4ITMFITEMLISTFREE		 ex_MP4ItmfItemListFree		= NIL;

DynamicLoader *mp4v2dll		= NIL;

Bool LoadMP4v2DLL()
{
	mp4v2dll = BoCA::Utilities::LoadCodecDLL("mp4v2");

	if (mp4v2dll == NIL) return False;

	ex_MP4ReadCallbacks		= (MP4READCALLBACKS) mp4v2dll->GetFunctionAddress("MP4ReadCallbacks");
	ex_MP4CreateCallbacks		= (MP4CREATECALLBACKS) mp4v2dll->GetFunctionAddress("MP4CreateCallbacks");
	ex_MP4ModifyCallbacks		= (MP4MODIFYCALLBACKS) mp4v2dll->GetFunctionAddress("MP4ModifyCallbacks");
	ex_MP4Close			= (MP4CLOSE) mp4v2dll->GetFunctionAddress("MP4Close");
	ex_MP4Free			= (MP4FREE) mp4v2dll->GetFunctionAddress("MP4Free");
	ex_MP4Optimize			= (MP4OPTIMIZE) mp4v2dll->GetFunctionAddress("MP4Optimize");

	ex_MP4FindTrackId		= (MP4FINDTRACKID) mp4v2dll->GetFunctionAddress("MP4FindTrackId");
	ex_MP4AddAudioTrack		= (MP4ADDAUDIOTRACK) mp4v2dll->GetFunctionAddress("MP4AddAudioTrack");
	ex_MP4SetAudioProfileLevel	= (MP4SETAUDIOPROFILELEVEL) mp4v2dll->GetFunctionAddress("MP4SetAudioProfileLevel");

	ex_MP4HaveTrackAtom		= (MP4HAVETRACKATOM) mp4v2dll->GetFunctionAddress("MP4HaveTrackAtom");
	ex_MP4GetTrackBytesProperty	= (MP4GETTRACKBYTESPROPERTY) mp4v2dll->GetFunctionAddress("MP4GetTrackBytesProperty");
	ex_MP4SetTrackIntegerProperty	= (MP4SETTRACKINTEGERPROPERTY) mp4v2dll->GetFunctionAddress("MP4SetTrackIntegerProperty");

	ex_MP4GetTrackAudioMpeg4Type	= (MP4GETTRACKAUDIOMPEG4TYPE) mp4v2dll->GetFunctionAddress("MP4GetTrackAudioMpeg4Type");
	ex_MP4GetTrackAudioChannels	= (MP4GETTRACKAUDIOCHANNELS) mp4v2dll->GetFunctionAddress("MP4GetTrackAudioChannels");
	ex_MP4GetTrackESConfiguration	= (MP4GETTRACKESCONFIGURATION) mp4v2dll->GetFunctionAddress("MP4GetTrackESConfiguration");
	ex_MP4SetTrackESConfiguration	= (MP4SETTRACKESCONFIGURATION) mp4v2dll->GetFunctionAddress("MP4SetTrackESConfiguration");
	ex_MP4GetTrackDuration		= (MP4GETTRACKDURATION) mp4v2dll->GetFunctionAddress("MP4GetTrackDuration");
	ex_MP4GetTrackTimeScale		= (MP4GETTRACKTIMESCALE) mp4v2dll->GetFunctionAddress("MP4GetTrackTimeScale");
	ex_MP4GetTrackNumberOfSamples	= (MP4GETTRACKNUMBEROFSAMPLES) mp4v2dll->GetFunctionAddress("MP4GetTrackNumberOfSamples");

	ex_MP4GetSampleTime		= (MP4GETSAMPLETIME) mp4v2dll->GetFunctionAddress("MP4GetSampleTime");
	ex_MP4GetSampleIdFromTime	= (MP4GETSAMPLEIDFROMTIME) mp4v2dll->GetFunctionAddress("MP4GetSampleIdFromTime");
	ex_MP4GetSampleSize		= (MP4GETSAMPLESIZE) mp4v2dll->GetFunctionAddress("MP4GetSampleSize");
	ex_MP4ReadSample		= (MP4READSAMPLE) mp4v2dll->GetFunctionAddress("MP4ReadSample");
	ex_MP4WriteSample		= (MP4WRITESAMPLE) mp4v2dll->GetFunctionAddress("MP4WriteSample");

	ex_MP4ItmfItemAlloc		= (MP4ITMFITEMALLOC) mp4v2dll->GetFunctionAddress("MP4ItmfItemAlloc");
	ex_MP4ItmfItemFree		= (MP4ITMFITEMFREE) mp4v2dll->GetFunctionAddress("MP4ItmfItemFree");
	ex_MP4ItmfAddItem		= (MP4ITMFADDITEM) mp4v2dll->GetFunctionAddress("MP4ItmfAddItem");
	ex_MP4ItmfGetItemsByMeaning	= (MP4ITMFGETITEMSBYMEANING) mp4v2dll->GetFunctionAddress("MP4ItmfGetItemsByMeaning");
	ex_MP4ItmfItemListFree		= (MP4ITMFITEMLISTFREE) mp4v2dll->GetFunctionAddress("MP4ItmfItemListFree");

	if (ex_MP4ReadCallbacks			== NIL ||
	    ex_MP4CreateCallbacks		== NIL ||
	    ex_MP4ModifyCallbacks		== NIL ||
	    ex_MP4Close				== NIL ||
	    ex_MP4Free				== NIL ||
	    ex_MP4Optimize			== NIL ||

	    ex_MP4FindTrackId			== NIL ||
	    ex_MP4AddAudioTrack			== NIL ||
	    ex_MP4SetAudioProfileLevel		== NIL ||

	    ex_MP4HaveTrackAtom			== NIL ||
	    ex_MP4GetTrackBytesProperty		== NIL ||
	    ex_MP4SetTrackIntegerProperty	== NIL ||

	    ex_MP4GetTrackAudioMpeg4Type	== NIL ||
	    ex_MP4GetTrackAudioChannels		== NIL ||
	    ex_MP4GetTrackESConfiguration	== NIL ||
	    ex_MP4SetTrackESConfiguration	== NIL ||
	    ex_MP4GetTrackDuration		== NIL ||
	    ex_MP4GetTrackTimeScale		== NIL ||
	    ex_MP4GetTrackNumberOfSamples	== NIL ||

	    ex_MP4GetSampleTime			== NIL ||
	    ex_MP4GetSampleIdFromTime		== NIL ||
	    ex_MP4GetSampleSize			== NIL ||
	    ex_MP4ReadSample			== NIL ||
	    ex_MP4WriteSample			== NIL ||

	    ex_MP4ItmfItemAlloc			== NIL ||
	    ex_MP4ItmfItemFree			== NIL ||
	    ex_MP4ItmfAddItem			== NIL ||
	    ex_MP4ItmfGetItemsByMeaning		== NIL ||
	    ex_MP4ItmfItemListFree		== NIL) { FreeMP4v2DLL(); return False; }

	return True;
}

Void FreeMP4v2DLL()
{
	BoCA::Utilities::FreeCodecDLL(mp4v2dll);

	mp4v2dll = NIL;
}
