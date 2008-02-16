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

NEAACDECOPEN			 ex_NeAACDecOpen			= NIL;
NEAACDECINIT			 ex_NeAACDecInit			= NIL;
NEAACDECINIT2			 ex_NeAACDecInit2			= NIL;
NEAACDECGETCURRENTCONFIGURATION	 ex_NeAACDecGetCurrentConfiguration	= NIL;
NEAACDECSETCONFIGURATION	 ex_NeAACDecSetConfiguration		= NIL;
NEAACDECDECODE			 ex_NeAACDecDecode			= NIL;
NEAACDECCLOSE			 ex_NeAACDecClose			= NIL;
NEAACDECGETERRORMESSAGE		 ex_NeAACDecGetErrorMessage		= NIL;

MP4READ				 ex_MP4Read				= NIL;
MP4CLOSE			 ex_MP4Close				= NIL;
MP4FREE				 ex_MP4Free				= NIL;
MP4GETMETADATANAME		 ex_MP4GetMetadataName			= NIL;
MP4GETMETADATAARTIST		 ex_MP4GetMetadataArtist		= NIL;
MP4GETMETADATACOMMENT		 ex_MP4GetMetadataComment		= NIL;
MP4GETMETADATAYEAR		 ex_MP4GetMetadataYear			= NIL;
MP4GETMETADATAALBUM		 ex_MP4GetMetadataAlbum			= NIL;
MP4GETMETADATAGENRE		 ex_MP4GetMetadataGenre			= NIL;
MP4GETMETADATATRACK		 ex_MP4GetMetadataTrack			= NIL;
MP4GETNUMBEROFTRACKS		 ex_MP4GetNumberOfTracks		= NIL;
MP4FINDTRACKID			 ex_MP4FindTrackId			= NIL;
MP4GETTRACKTYPE			 ex_MP4GetTrackType			= NIL;
MP4GETTRACKESCONFIGURATION	 ex_MP4GetTrackESConfiguration		= NIL;
MP4GETTRACKDURATION		 ex_MP4GetTrackDuration			= NIL;
MP4GETTRACKTIMESCALE		 ex_MP4GetTrackTimeScale		= NIL;
MP4READSAMPLE			 ex_MP4ReadSample			= NIL;

DynamicLoader *faad2dll	= NIL;
DynamicLoader *mp4v2dll	= NIL;

Bool LoadFAAD2DLL()
{
	faad2dll = new DynamicLoader("encoders/FAAD2");

	ex_NeAACDecOpen				= (NEAACDECOPEN) faad2dll->GetFunctionAddress("NeAACDecOpen");
	ex_NeAACDecInit				= (NEAACDECINIT) faad2dll->GetFunctionAddress("NeAACDecInit");
	ex_NeAACDecInit2			= (NEAACDECINIT2) faad2dll->GetFunctionAddress("NeAACDecInit2");
	ex_NeAACDecGetCurrentConfiguration	= (NEAACDECGETCURRENTCONFIGURATION) faad2dll->GetFunctionAddress("NeAACDecGetCurrentConfiguration");
	ex_NeAACDecSetConfiguration		= (NEAACDECSETCONFIGURATION) faad2dll->GetFunctionAddress("NeAACDecSetConfiguration");
	ex_NeAACDecDecode			= (NEAACDECDECODE) faad2dll->GetFunctionAddress("NeAACDecDecode");
	ex_NeAACDecClose			= (NEAACDECCLOSE) faad2dll->GetFunctionAddress("NeAACDecClose");
	ex_NeAACDecGetErrorMessage		= (NEAACDECGETERRORMESSAGE) faad2dll->GetFunctionAddress("NeAACDecGetErrorMessage");

	if (ex_NeAACDecOpen			== NIL ||
	    ex_NeAACDecInit			== NIL ||
	    ex_NeAACDecInit2			== NIL ||
	    ex_NeAACDecGetCurrentConfiguration	== NIL ||
	    ex_NeAACDecSetConfiguration		== NIL ||
	    ex_NeAACDecDecode			== NIL ||
	    ex_NeAACDecClose			== NIL ||
	    ex_NeAACDecGetErrorMessage		== NIL) { FreeFAAD2DLL(); return False; }

	return True;
}

Void FreeFAAD2DLL()
{
	Object::DeleteObject(faad2dll);
}

Bool LoadMP4v2DLL()
{
	mp4v2dll = new DynamicLoader("encoders/MP4v2");

	ex_MP4Read			= (MP4READ) mp4v2dll->GetFunctionAddress("MP4Read");
	ex_MP4Close			= (MP4CLOSE) mp4v2dll->GetFunctionAddress("MP4Close");
	ex_MP4Free			= (MP4FREE) mp4v2dll->GetFunctionAddress("MP4Free");
	ex_MP4GetMetadataName		= (MP4GETMETADATANAME) mp4v2dll->GetFunctionAddress("MP4GetMetadataName");
	ex_MP4GetMetadataArtist		= (MP4GETMETADATAARTIST) mp4v2dll->GetFunctionAddress("MP4GetMetadataArtist");
	ex_MP4GetMetadataComment	= (MP4GETMETADATACOMMENT) mp4v2dll->GetFunctionAddress("MP4GetMetadataComment");
	ex_MP4GetMetadataYear		= (MP4GETMETADATAYEAR) mp4v2dll->GetFunctionAddress("MP4GetMetadataYear");
	ex_MP4GetMetadataAlbum		= (MP4GETMETADATAALBUM) mp4v2dll->GetFunctionAddress("MP4GetMetadataAlbum");
	ex_MP4GetMetadataGenre		= (MP4GETMETADATAGENRE) mp4v2dll->GetFunctionAddress("MP4GetMetadataGenre");
	ex_MP4GetMetadataTrack		= (MP4GETMETADATATRACK) mp4v2dll->GetFunctionAddress("MP4GetMetadataTrack");
	ex_MP4GetNumberOfTracks		= (MP4GETNUMBEROFTRACKS) mp4v2dll->GetFunctionAddress("MP4GetNumberOfTracks");
	ex_MP4FindTrackId		= (MP4FINDTRACKID) mp4v2dll->GetFunctionAddress("MP4FindTrackId");
	ex_MP4GetTrackType		= (MP4GETTRACKTYPE) mp4v2dll->GetFunctionAddress("MP4GetTrackType");
	ex_MP4GetTrackESConfiguration	= (MP4GETTRACKESCONFIGURATION) mp4v2dll->GetFunctionAddress("MP4GetTrackESConfiguration");
	ex_MP4GetTrackDuration		= (MP4GETTRACKDURATION) mp4v2dll->GetFunctionAddress("MP4GetTrackDuration");
	ex_MP4GetTrackTimeScale		= (MP4GETTRACKTIMESCALE) mp4v2dll->GetFunctionAddress("MP4GetTrackTimeScale");
	ex_MP4ReadSample		= (MP4READSAMPLE) mp4v2dll->GetFunctionAddress("MP4ReadSample");

	if (ex_MP4Read				== NIL ||
	    ex_MP4Close				== NIL ||
	    ex_MP4Free				== NIL ||
	    ex_MP4GetMetadataName		== NIL ||
	    ex_MP4GetMetadataArtist		== NIL ||
	    ex_MP4GetMetadataComment		== NIL ||
	    ex_MP4GetMetadataYear		== NIL ||
	    ex_MP4GetMetadataAlbum		== NIL ||
	    ex_MP4GetMetadataGenre		== NIL ||
	    ex_MP4GetMetadataTrack		== NIL ||
	    ex_MP4GetNumberOfTracks		== NIL ||
	    ex_MP4FindTrackId			== NIL ||
	    ex_MP4GetTrackType			== NIL ||
	    ex_MP4GetTrackESConfiguration	== NIL ||
	    ex_MP4GetTrackDuration		== NIL ||
	    ex_MP4GetTrackTimeScale		== NIL ||
	    ex_MP4ReadSample			== NIL) { FreeMP4v2DLL(); return False; }

	return True;
}

Void FreeMP4v2DLL()
{
	Object::DeleteObject(mp4v2dll);
}
