 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

AACDECODER_OPEN			 ex_aacDecoder_Open			= NIL;
AACDECODER_CONFIGRAW		 ex_aacDecoder_ConfigRaw		= NIL;
AACDECODER_FILL			 ex_aacDecoder_Fill			= NIL;
AACDECODER_GETSTREAMINFO	 ex_aacDecoder_GetStreamInfo		= NIL;
AACDECODER_DECODEFRAME		 ex_aacDecoder_DecodeFrame		= NIL;
AACDECODER_CLOSE		 ex_aacDecoder_Close			= NIL;

MP4READ				 ex_MP4Read				= NIL;
MP4CLOSE			 ex_MP4Close				= NIL;
MP4FREE				 ex_MP4Free				= NIL;
MP4GETNUMBEROFTRACKS		 ex_MP4GetNumberOfTracks		= NIL;
MP4FINDTRACKID			 ex_MP4FindTrackId			= NIL;
MP4GETTRACKTYPE			 ex_MP4GetTrackType			= NIL;
MP4GETTRACKAUDIOMPEG4TYPE	 ex_MP4GetTrackAudioMpeg4Type		= NIL;
MP4GETTRACKESCONFIGURATION	 ex_MP4GetTrackESConfiguration		= NIL;
MP4GETTRACKDURATION		 ex_MP4GetTrackDuration			= NIL;
MP4GETTRACKTIMESCALE		 ex_MP4GetTrackTimeScale		= NIL;
MP4GETSAMPLEIDFROMTIME		 ex_MP4GetSampleIdFromTime		= NIL;
MP4READSAMPLE			 ex_MP4ReadSample			= NIL;

DynamicLoader *fdkaacdll	= NIL;
DynamicLoader *mp4v2dll		= NIL;

Bool LoadFDKAACDLL()
{
	fdkaacdll = BoCA::Utilities::LoadCodecDLL("fdk-aac");

	if (fdkaacdll == NIL) return False;

	ex_aacDecoder_Open		= (AACDECODER_OPEN) fdkaacdll->GetFunctionAddress("aacDecoder_Open");
	ex_aacDecoder_ConfigRaw		= (AACDECODER_CONFIGRAW) fdkaacdll->GetFunctionAddress("aacDecoder_ConfigRaw");
	ex_aacDecoder_Fill		= (AACDECODER_FILL) fdkaacdll->GetFunctionAddress("aacDecoder_Fill");
	ex_aacDecoder_GetStreamInfo	= (AACDECODER_GETSTREAMINFO) fdkaacdll->GetFunctionAddress("aacDecoder_GetStreamInfo");
	ex_aacDecoder_DecodeFrame	= (AACDECODER_DECODEFRAME) fdkaacdll->GetFunctionAddress("aacDecoder_DecodeFrame");
	ex_aacDecoder_Close		= (AACDECODER_CLOSE) fdkaacdll->GetFunctionAddress("aacDecoder_Close");

	if (ex_aacDecoder_Open		== NIL ||
	    ex_aacDecoder_ConfigRaw	== NIL ||
	    ex_aacDecoder_Fill		== NIL ||
	    ex_aacDecoder_GetStreamInfo	== NIL ||
	    ex_aacDecoder_DecodeFrame	== NIL ||
	    ex_aacDecoder_Close		== NIL) { FreeFDKAACDLL(); return False; }

	return True;
}

Void FreeFDKAACDLL()
{
	BoCA::Utilities::FreeCodecDLL(fdkaacdll);

	fdkaacdll = NIL;
}

Bool LoadMP4v2DLL()
{
	mp4v2dll = BoCA::Utilities::LoadCodecDLL("mp4v2");

	if (mp4v2dll == NIL) return False;

	ex_MP4Read			= (MP4READ) mp4v2dll->GetFunctionAddress("MP4Read");
	ex_MP4Close			= (MP4CLOSE) mp4v2dll->GetFunctionAddress("MP4Close");
	ex_MP4Free			= (MP4FREE) mp4v2dll->GetFunctionAddress("MP4Free");
	ex_MP4GetNumberOfTracks		= (MP4GETNUMBEROFTRACKS) mp4v2dll->GetFunctionAddress("MP4GetNumberOfTracks");
	ex_MP4FindTrackId		= (MP4FINDTRACKID) mp4v2dll->GetFunctionAddress("MP4FindTrackId");
	ex_MP4GetTrackType		= (MP4GETTRACKTYPE) mp4v2dll->GetFunctionAddress("MP4GetTrackType");
	ex_MP4GetTrackAudioMpeg4Type	= (MP4GETTRACKAUDIOMPEG4TYPE) mp4v2dll->GetFunctionAddress("MP4GetTrackAudioMpeg4Type");
	ex_MP4GetTrackESConfiguration	= (MP4GETTRACKESCONFIGURATION) mp4v2dll->GetFunctionAddress("MP4GetTrackESConfiguration");
	ex_MP4GetTrackDuration		= (MP4GETTRACKDURATION) mp4v2dll->GetFunctionAddress("MP4GetTrackDuration");
	ex_MP4GetTrackTimeScale		= (MP4GETTRACKTIMESCALE) mp4v2dll->GetFunctionAddress("MP4GetTrackTimeScale");
	ex_MP4GetSampleIdFromTime	= (MP4GETSAMPLEIDFROMTIME) mp4v2dll->GetFunctionAddress("MP4GetSampleIdFromTime");
	ex_MP4ReadSample		= (MP4READSAMPLE) mp4v2dll->GetFunctionAddress("MP4ReadSample");

	if (ex_MP4Read				== NIL ||
	    ex_MP4Close				== NIL ||
	    ex_MP4Free				== NIL ||
	    ex_MP4GetNumberOfTracks		== NIL ||
	    ex_MP4FindTrackId			== NIL ||
	    ex_MP4GetTrackType			== NIL ||
	    ex_MP4GetTrackAudioMpeg4Type	== NIL ||
	    ex_MP4GetTrackESConfiguration	== NIL ||
	    ex_MP4GetTrackDuration		== NIL ||
	    ex_MP4GetTrackTimeScale		== NIL ||
	    ex_MP4GetSampleIdFromTime		== NIL ||
	    ex_MP4ReadSample			== NIL) { FreeMP4v2DLL(); return False; }

	return True;
}

Void FreeMP4v2DLL()
{
	BoCA::Utilities::FreeCodecDLL(mp4v2dll);

	mp4v2dll = NIL;
}