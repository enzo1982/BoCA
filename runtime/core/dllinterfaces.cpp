 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/core/dllinterfaces.h>

ID3TAGNEW			 ex_ID3Tag_New				= NIL;
ID3TAGDELETE			 ex_ID3Tag_Delete			= NIL;
ID3TAGSETPADDING		 ex_ID3Tag_SetPadding			= NIL;
ID3TAGADDFRAME			 ex_ID3Tag_AddFrame			= NIL;
ID3TAGNUMFRAMES			 ex_ID3Tag_NumFrames			= NIL;
ID3TAGFINDFRAMEWITHID		 ex_ID3Tag_FindFrameWithID		= NIL;
ID3TAGPARSEBUFFER		 ex_ID3Tag_ParseBuffer			= NIL;
ID3TAGSIZE			 ex_ID3Tag_Size				= NIL;
ID3TAGRENDER			 ex_ID3Tag_Render			= NIL;
ID3TAGCREATEITERATOR		 ex_ID3Tag_CreateIterator		= NIL;
ID3TAGITERATORGETNEXT		 ex_ID3TagIterator_GetNext		= NIL;
ID3TAGITERATORDELETE		 ex_ID3TagIterator_Delete		= NIL;
ID3FRAMEGETID			 ex_ID3Frame_GetID			= NIL;
ID3FRAMENEWID			 ex_ID3Frame_NewID			= NIL;
ID3FRAMEDELETE			 ex_ID3Frame_Delete			= NIL;
ID3FRAMEGETFIELD		 ex_ID3Frame_GetField			= NIL;
ID3FIELDSIZE			 ex_ID3Field_Size			= NIL;
ID3FIELDSETBINARY		 ex_ID3Field_SetBINARY			= NIL;
ID3FIELDGETBINARY		 ex_ID3Field_GetBINARY			= NIL;
ID3FIELDSETINT			 ex_ID3Field_SetINT			= NIL;
ID3FIELDGETINT			 ex_ID3Field_GetINT			= NIL;
ID3FIELDSETENCODING		 ex_ID3Field_SetEncoding		= NIL;
ID3FIELDSETASCII		 ex_ID3Field_SetASCII			= NIL;
ID3FIELDGETASCII		 ex_ID3Field_GetASCII			= NIL;
ID3FIELDSETUNICODE		 ex_ID3Field_SetUNICODE			= NIL;
ID3FIELDGETUNICODE		 ex_ID3Field_GetUNICODE			= NIL;

MP4READ				 ex_MP4Read				= NIL;
MP4MODIFY			 ex_MP4Modify				= NIL;
MP4CLOSE			 ex_MP4Close				= NIL;
MP4FREE				 ex_MP4Free				= NIL;
MP4OPTIMIZE			 ex_MP4Optimize				= NIL;
MP4GETMETADATANAME		 ex_MP4GetMetadataName			= NIL;
MP4GETMETADATAARTIST		 ex_MP4GetMetadataArtist		= NIL;
MP4GETMETADATACOMMENT		 ex_MP4GetMetadataComment		= NIL;
MP4GETMETADATAYEAR		 ex_MP4GetMetadataYear			= NIL;
MP4GETMETADATAALBUM		 ex_MP4GetMetadataAlbum			= NIL;
MP4GETMETADATAGENRE		 ex_MP4GetMetadataGenre			= NIL;
MP4GETMETADATATRACK		 ex_MP4GetMetadataTrack			= NIL;
MP4GETMETADATACOVERART		 ex_MP4GetMetadataCoverArt		= NIL;
MP4GETMETADATACOVERARTCOUNT	 ex_MP4GetMetadataCoverArtCount		= NIL;
MP4SETMETADATANAME		 ex_MP4SetMetadataName			= NIL;
MP4SETMETADATAARTIST		 ex_MP4SetMetadataArtist		= NIL;
MP4SETMETADATACOMMENT		 ex_MP4SetMetadataComment		= NIL;
MP4SETMETADATAYEAR		 ex_MP4SetMetadataYear			= NIL;
MP4SETMETADATAALBUM		 ex_MP4SetMetadataAlbum			= NIL;
MP4SETMETADATAGENRE		 ex_MP4SetMetadataGenre			= NIL;
MP4SETMETADATATRACK		 ex_MP4SetMetadataTrack			= NIL;
MP4SETMETADATACOVERART		 ex_MP4SetMetadataCoverArt		= NIL;

OGGSTREAMINIT			 ex_ogg_stream_init			= NIL;
OGGSTREAMPACKETIN		 ex_ogg_stream_packetin			= NIL;
OGGSTREAMFLUSH			 ex_ogg_stream_flush			= NIL;
OGGSTREAMCLEAR			 ex_ogg_stream_clear			= NIL;
OGGSYNCINIT			 ex_ogg_sync_init			= NIL;
OGGSYNCBUFFER			 ex_ogg_sync_buffer			= NIL;
OGGSYNCWROTE			 ex_ogg_sync_wrote			= NIL;
OGGSYNCPAGEOUT			 ex_ogg_sync_pageout			= NIL;
OGGSYNCCLEAR			 ex_ogg_sync_clear			= NIL;

DynamicLoader *BoCA::DLLInterfaces::id3dll	= NIL;
DynamicLoader *BoCA::DLLInterfaces::mp4v2dll	= NIL;
DynamicLoader *BoCA::DLLInterfaces::oggdll	= NIL;

Bool BoCA::DLLInterfaces::LoadID3DLL()
{
	id3dll = new DynamicLoader("ID3lib");

	ex_ID3Tag_New			= (ID3TAGNEW) id3dll->GetFunctionAddress("ID3Tag_New");
	ex_ID3Tag_Delete		= (ID3TAGDELETE) id3dll->GetFunctionAddress("ID3Tag_Delete");
	ex_ID3Tag_SetPadding		= (ID3TAGSETPADDING) id3dll->GetFunctionAddress("ID3Tag_SetPadding");
	ex_ID3Tag_AddFrame		= (ID3TAGADDFRAME) id3dll->GetFunctionAddress("ID3Tag_AddFrame");
	ex_ID3Tag_NumFrames		= (ID3TAGNUMFRAMES) id3dll->GetFunctionAddress("ID3Tag_NumFrames");
	ex_ID3Tag_FindFrameWithID	= (ID3TAGFINDFRAMEWITHID) id3dll->GetFunctionAddress("ID3Tag_FindFrameWithID");
	ex_ID3Tag_ParseBuffer		= (ID3TAGPARSEBUFFER) id3dll->GetFunctionAddress("ID3Tag_ParseBuffer");
	ex_ID3Tag_Size			= (ID3TAGSIZE) id3dll->GetFunctionAddress("ID3Tag_Size");
	ex_ID3Tag_Render		= (ID3TAGRENDER) id3dll->GetFunctionAddress("ID3Tag_Render");
	ex_ID3Tag_CreateIterator	= (ID3TAGCREATEITERATOR) id3dll->GetFunctionAddress("ID3Tag_CreateIterator");
	ex_ID3TagIterator_GetNext	= (ID3TAGITERATORGETNEXT) id3dll->GetFunctionAddress("ID3TagIterator_GetNext");
	ex_ID3TagIterator_Delete	= (ID3TAGITERATORDELETE) id3dll->GetFunctionAddress("ID3TagIterator_Delete");
	ex_ID3Frame_GetID		= (ID3FRAMEGETID) id3dll->GetFunctionAddress("ID3Frame_GetID");
	ex_ID3Frame_NewID		= (ID3FRAMENEWID) id3dll->GetFunctionAddress("ID3Frame_NewID");
	ex_ID3Frame_Delete		= (ID3FRAMEDELETE) id3dll->GetFunctionAddress("ID3Frame_Delete");
	ex_ID3Frame_GetField		= (ID3FRAMEGETFIELD) id3dll->GetFunctionAddress("ID3Frame_GetField");
	ex_ID3Field_Size		= (ID3FIELDSIZE) id3dll->GetFunctionAddress("ID3Field_Size");
	ex_ID3Field_SetBINARY		= (ID3FIELDSETBINARY) id3dll->GetFunctionAddress("ID3Field_SetBINARY");
	ex_ID3Field_GetBINARY		= (ID3FIELDGETBINARY) id3dll->GetFunctionAddress("ID3Field_GetBINARY");
	ex_ID3Field_SetINT		= (ID3FIELDSETINT) id3dll->GetFunctionAddress("ID3Field_SetINT");
	ex_ID3Field_GetINT		= (ID3FIELDGETINT) id3dll->GetFunctionAddress("ID3Field_GetINT");
	ex_ID3Field_SetEncoding		= (ID3FIELDSETENCODING) id3dll->GetFunctionAddress("ID3Field_SetEncoding");
	ex_ID3Field_SetASCII		= (ID3FIELDSETASCII) id3dll->GetFunctionAddress("ID3Field_SetASCII");
	ex_ID3Field_GetASCII		= (ID3FIELDGETASCII) id3dll->GetFunctionAddress("ID3Field_GetASCII");
	ex_ID3Field_SetUNICODE		= (ID3FIELDSETUNICODE) id3dll->GetFunctionAddress("ID3Field_SetUNICODE");
	ex_ID3Field_GetUNICODE		= (ID3FIELDGETUNICODE) id3dll->GetFunctionAddress("ID3Field_GetUNICODE");

	if (ex_ID3Tag_New			== NIL ||
	    ex_ID3Tag_Delete			== NIL ||
	    ex_ID3Tag_SetPadding		== NIL ||
	    ex_ID3Tag_AddFrame			== NIL ||
	    ex_ID3Tag_NumFrames			== NIL ||
	    ex_ID3Tag_FindFrameWithID		== NIL ||
	    ex_ID3Tag_ParseBuffer		== NIL ||
	    ex_ID3Tag_Size			== NIL ||
	    ex_ID3Tag_Render			== NIL ||
	    ex_ID3Tag_CreateIterator		== NIL ||
	    ex_ID3TagIterator_GetNext		== NIL ||
	    ex_ID3TagIterator_Delete		== NIL ||
	    ex_ID3Frame_GetID			== NIL ||
	    ex_ID3Frame_NewID			== NIL ||
	    ex_ID3Frame_Delete			== NIL ||
	    ex_ID3Frame_GetField		== NIL ||
	    ex_ID3Field_Size			== NIL ||
	    ex_ID3Field_SetBINARY		== NIL ||
	    ex_ID3Field_GetBINARY		== NIL ||
	    ex_ID3Field_SetINT			== NIL ||
	    ex_ID3Field_GetINT			== NIL ||
	    ex_ID3Field_SetEncoding		== NIL ||
	    ex_ID3Field_SetASCII		== NIL ||
	    ex_ID3Field_GetASCII		== NIL ||
	    ex_ID3Field_SetUNICODE		== NIL ||
	    ex_ID3Field_GetUNICODE		== NIL) { FreeID3DLL(); return False; }

	return True;
}

Void BoCA::DLLInterfaces::FreeID3DLL()
{
	Object::DeleteObject(id3dll);

	id3dll = NIL;
}

Bool BoCA::DLLInterfaces::LoadMP4v2DLL()
{
	mp4v2dll = new DynamicLoader("codecs/MP4v2");

	ex_MP4Read			= (MP4READ) mp4v2dll->GetFunctionAddress("MP4Read");
	ex_MP4Modify			= (MP4MODIFY) mp4v2dll->GetFunctionAddress("MP4Modify");
	ex_MP4Close			= (MP4CLOSE) mp4v2dll->GetFunctionAddress("MP4Close");
	ex_MP4Free			= (MP4FREE) mp4v2dll->GetFunctionAddress("MP4Free");
	ex_MP4Optimize			= (MP4OPTIMIZE) mp4v2dll->GetFunctionAddress("MP4Optimize");
	ex_MP4GetMetadataName		= (MP4GETMETADATANAME) mp4v2dll->GetFunctionAddress("MP4GetMetadataName");
	ex_MP4GetMetadataArtist		= (MP4GETMETADATAARTIST) mp4v2dll->GetFunctionAddress("MP4GetMetadataArtist");
	ex_MP4GetMetadataComment	= (MP4GETMETADATACOMMENT) mp4v2dll->GetFunctionAddress("MP4GetMetadataComment");
	ex_MP4GetMetadataYear		= (MP4GETMETADATAYEAR) mp4v2dll->GetFunctionAddress("MP4GetMetadataYear");
	ex_MP4GetMetadataAlbum		= (MP4GETMETADATAALBUM) mp4v2dll->GetFunctionAddress("MP4GetMetadataAlbum");
	ex_MP4GetMetadataGenre		= (MP4GETMETADATAGENRE) mp4v2dll->GetFunctionAddress("MP4GetMetadataGenre");
	ex_MP4GetMetadataTrack		= (MP4GETMETADATATRACK) mp4v2dll->GetFunctionAddress("MP4GetMetadataTrack");
	ex_MP4GetMetadataCoverArt	= (MP4GETMETADATACOVERART) mp4v2dll->GetFunctionAddress("MP4GetMetadataCoverArt");
	ex_MP4GetMetadataCoverArtCount	= (MP4GETMETADATACOVERARTCOUNT) mp4v2dll->GetFunctionAddress("MP4GetMetadataCoverArtCount");
	ex_MP4SetMetadataName		= (MP4SETMETADATANAME) mp4v2dll->GetFunctionAddress("MP4SetMetadataName");
	ex_MP4SetMetadataArtist		= (MP4SETMETADATAARTIST) mp4v2dll->GetFunctionAddress("MP4SetMetadataArtist");
	ex_MP4SetMetadataComment	= (MP4SETMETADATACOMMENT) mp4v2dll->GetFunctionAddress("MP4SetMetadataComment");
	ex_MP4SetMetadataYear		= (MP4SETMETADATAYEAR) mp4v2dll->GetFunctionAddress("MP4SetMetadataYear");
	ex_MP4SetMetadataAlbum		= (MP4SETMETADATAALBUM) mp4v2dll->GetFunctionAddress("MP4SetMetadataAlbum");
	ex_MP4SetMetadataGenre		= (MP4SETMETADATAGENRE) mp4v2dll->GetFunctionAddress("MP4SetMetadataGenre");
	ex_MP4SetMetadataTrack		= (MP4SETMETADATATRACK) mp4v2dll->GetFunctionAddress("MP4SetMetadataTrack");
	ex_MP4SetMetadataCoverArt	= (MP4SETMETADATACOVERART) mp4v2dll->GetFunctionAddress("MP4SetMetadataCoverArt");

	if (ex_MP4Read				== NIL ||
	    ex_MP4Modify			== NIL ||
	    ex_MP4Close				== NIL ||
	    ex_MP4Free				== NIL ||
	    ex_MP4Optimize			== NIL ||
	    ex_MP4GetMetadataName		== NIL ||
	    ex_MP4GetMetadataArtist		== NIL ||
	    ex_MP4GetMetadataComment		== NIL ||
	    ex_MP4GetMetadataYear		== NIL ||
	    ex_MP4GetMetadataAlbum		== NIL ||
	    ex_MP4GetMetadataGenre		== NIL ||
	    ex_MP4GetMetadataTrack		== NIL ||
	    ex_MP4GetMetadataCoverArt		== NIL ||
	    ex_MP4GetMetadataCoverArtCount	== NIL ||
	    ex_MP4SetMetadataName		== NIL ||
	    ex_MP4SetMetadataArtist		== NIL ||
	    ex_MP4SetMetadataComment		== NIL ||
	    ex_MP4SetMetadataYear		== NIL ||
	    ex_MP4SetMetadataAlbum		== NIL ||
	    ex_MP4SetMetadataGenre		== NIL ||
	    ex_MP4SetMetadataTrack		== NIL ||
	    ex_MP4SetMetadataCoverArt		== NIL) { FreeMP4v2DLL(); return False; }

	return True;
}

Void BoCA::DLLInterfaces::FreeMP4v2DLL()
{
	Object::DeleteObject(mp4v2dll);

	mp4v2dll = NIL;
}

Bool BoCA::DLLInterfaces::LoadOggDLL()
{
	oggdll = new DynamicLoader("codecs/Ogg");

	ex_ogg_stream_init		= (OGGSTREAMINIT) oggdll->GetFunctionAddress("ogg_stream_init");
	ex_ogg_stream_packetin		= (OGGSTREAMPACKETIN) oggdll->GetFunctionAddress("ogg_stream_packetin");
	ex_ogg_stream_flush		= (OGGSTREAMFLUSH) oggdll->GetFunctionAddress("ogg_stream_flush");
	ex_ogg_stream_clear		= (OGGSTREAMCLEAR) oggdll->GetFunctionAddress("ogg_stream_clear");
	ex_ogg_sync_init		= (OGGSYNCINIT) oggdll->GetFunctionAddress("ogg_sync_init");
	ex_ogg_sync_buffer		= (OGGSYNCBUFFER) oggdll->GetFunctionAddress("ogg_sync_buffer");
	ex_ogg_sync_wrote		= (OGGSYNCWROTE) oggdll->GetFunctionAddress("ogg_sync_wrote");
	ex_ogg_sync_pageout		= (OGGSYNCPAGEOUT) oggdll->GetFunctionAddress("ogg_sync_pageout");
	ex_ogg_sync_clear		= (OGGSYNCCLEAR) oggdll->GetFunctionAddress("ogg_sync_clear");

	if (ex_ogg_stream_init			== NIL ||
	    ex_ogg_stream_packetin		== NIL ||
	    ex_ogg_stream_flush			== NIL ||
	    ex_ogg_stream_clear			== NIL ||
	    ex_ogg_sync_init			== NIL ||
	    ex_ogg_sync_buffer			== NIL ||
	    ex_ogg_sync_wrote			== NIL ||
	    ex_ogg_sync_pageout			== NIL ||
	    ex_ogg_sync_clear			== NIL) { FreeOggDLL(); return False; }

	return True;
}

Void BoCA::DLLInterfaces::FreeOggDLL()
{
	Object::DeleteObject(oggdll);

	oggdll = NIL;
}
