 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

MP4READ			 ex_MP4Read			= NIL;
MP4MODIFY		 ex_MP4Modify			= NIL;
MP4CLOSE		 ex_MP4Close			= NIL;
MP4OPTIMIZE		 ex_MP4Optimize			= NIL;

MP4TAGSALLOC		 ex_MP4TagsAlloc		= NIL;
MP4TAGSFETCH		 ex_MP4TagsFetch		= NIL;
MP4TAGSSTORE		 ex_MP4TagsStore		= NIL;
MP4TAGSFREE		 ex_MP4TagsFree			= NIL;

MP4TAGSSETNAME		 ex_MP4TagsSetName		= NIL;
MP4TAGSSETARTIST	 ex_MP4TagsSetArtist		= NIL;
MP4TAGSSETALBUMARTIST	 ex_MP4TagsSetAlbumArtist	= NIL;
MP4TAGSSETALBUM		 ex_MP4TagsSetAlbum		= NIL;
MP4TAGSSETGROUPING	 ex_MP4TagsSetGrouping		= NIL;
MP4TAGSSETCOMPOSER	 ex_MP4TagsSetComposer		= NIL;
MP4TAGSSETCOMMENTS	 ex_MP4TagsSetComments		= NIL;
MP4TAGSSETGENRE		 ex_MP4TagsSetGenre		= NIL;
MP4TAGSSETGENRETYPE	 ex_MP4TagsSetGenreType		= NIL;
MP4TAGSSETRELEASEDATE	 ex_MP4TagsSetReleaseDate	= NIL;
MP4TAGSSETTRACK		 ex_MP4TagsSetTrack		= NIL;
MP4TAGSSETDISK		 ex_MP4TagsSetDisk		= NIL;
MP4TAGSSETCOMPILATION	 ex_MP4TagsSetCompilation	= NIL;
MP4TAGSSETCOPYRIGHT	 ex_MP4TagsSetCopyright		= NIL;

MP4TAGSADDARTWORK	 ex_MP4TagsAddArtwork		= NIL;
MP4TAGSREMOVEARTWORK	 ex_MP4TagsRemoveArtwork	= NIL;

DynamicLoader *mp4v2dll	= NIL;

Bool LoadMP4v2DLL()
{
	mp4v2dll = new DynamicLoader("codecs/MP4v2");

	ex_MP4Read			= (MP4READ) mp4v2dll->GetFunctionAddress("MP4Read");
	ex_MP4Modify			= (MP4MODIFY) mp4v2dll->GetFunctionAddress("MP4Modify");
	ex_MP4Close			= (MP4CLOSE) mp4v2dll->GetFunctionAddress("MP4Close");
	ex_MP4Optimize			= (MP4OPTIMIZE) mp4v2dll->GetFunctionAddress("MP4Optimize");

	ex_MP4TagsAlloc			= (MP4TAGSALLOC) mp4v2dll->GetFunctionAddress("MP4TagsAlloc");
	ex_MP4TagsFetch			= (MP4TAGSFETCH) mp4v2dll->GetFunctionAddress("MP4TagsFetch");
	ex_MP4TagsStore			= (MP4TAGSSTORE) mp4v2dll->GetFunctionAddress("MP4TagsStore");
	ex_MP4TagsFree			= (MP4TAGSFREE) mp4v2dll->GetFunctionAddress("MP4TagsFree");

	ex_MP4TagsSetName		= (MP4TAGSSETNAME) mp4v2dll->GetFunctionAddress("MP4TagsSetName");
	ex_MP4TagsSetArtist		= (MP4TAGSSETARTIST) mp4v2dll->GetFunctionAddress("MP4TagsSetArtist");
	ex_MP4TagsSetAlbumArtist	= (MP4TAGSSETALBUMARTIST) mp4v2dll->GetFunctionAddress("MP4TagsSetAlbumArtist");
	ex_MP4TagsSetAlbum		= (MP4TAGSSETALBUM) mp4v2dll->GetFunctionAddress("MP4TagsSetAlbum");
	ex_MP4TagsSetGrouping		= (MP4TAGSSETGROUPING) mp4v2dll->GetFunctionAddress("MP4TagsSetGrouping");
	ex_MP4TagsSetComposer		= (MP4TAGSSETCOMPOSER) mp4v2dll->GetFunctionAddress("MP4TagsSetComposer");
	ex_MP4TagsSetComments		= (MP4TAGSSETCOMMENTS) mp4v2dll->GetFunctionAddress("MP4TagsSetComments");
	ex_MP4TagsSetGenre		= (MP4TAGSSETGENRE) mp4v2dll->GetFunctionAddress("MP4TagsSetGenre");
	ex_MP4TagsSetGenreType		= (MP4TAGSSETGENRETYPE) mp4v2dll->GetFunctionAddress("MP4TagsSetGenreType");
	ex_MP4TagsSetReleaseDate	= (MP4TAGSSETRELEASEDATE) mp4v2dll->GetFunctionAddress("MP4TagsSetReleaseDate");
	ex_MP4TagsSetTrack		= (MP4TAGSSETTRACK) mp4v2dll->GetFunctionAddress("MP4TagsSetTrack");
	ex_MP4TagsSetDisk		= (MP4TAGSSETDISK) mp4v2dll->GetFunctionAddress("MP4TagsSetDisk");
	ex_MP4TagsSetCompilation	= (MP4TAGSSETCOMPILATION) mp4v2dll->GetFunctionAddress("MP4TagsSetCompilation");
	ex_MP4TagsSetCopyright		= (MP4TAGSSETCOPYRIGHT) mp4v2dll->GetFunctionAddress("MP4TagsSetCopyright");

	ex_MP4TagsAddArtwork		= (MP4TAGSADDARTWORK) mp4v2dll->GetFunctionAddress("MP4TagsAddArtwork");
	ex_MP4TagsRemoveArtwork		= (MP4TAGSREMOVEARTWORK) mp4v2dll->GetFunctionAddress("MP4TagsRemoveArtwork");

	if (ex_MP4Read			== NIL ||
	    ex_MP4Modify		== NIL ||
	    ex_MP4Close			== NIL ||
	    ex_MP4Optimize		== NIL ||

	    ex_MP4TagsAlloc		== NIL ||
	    ex_MP4TagsFetch		== NIL ||
	    ex_MP4TagsStore		== NIL ||
	    ex_MP4TagsFree		== NIL ||

	    ex_MP4TagsSetName		== NIL ||
	    ex_MP4TagsSetArtist		== NIL ||
	    ex_MP4TagsSetAlbumArtist	== NIL ||
	    ex_MP4TagsSetAlbum		== NIL ||
	    ex_MP4TagsSetGrouping	== NIL ||
	    ex_MP4TagsSetComposer	== NIL ||
	    ex_MP4TagsSetComments	== NIL ||
	    ex_MP4TagsSetGenre		== NIL ||
	    ex_MP4TagsSetGenreType	== NIL ||
	    ex_MP4TagsSetReleaseDate	== NIL ||
	    ex_MP4TagsSetTrack		== NIL ||
	    ex_MP4TagsSetDisk		== NIL ||
	    ex_MP4TagsSetCompilation	== NIL ||
	    ex_MP4TagsSetCopyright	== NIL ||

	    ex_MP4TagsAddArtwork	== NIL ||
	    ex_MP4TagsRemoveArtwork	== NIL) { FreeMP4v2DLL(); return False; }

	return True;
}

Void FreeMP4v2DLL()
{
	Object::DeleteObject(mp4v2dll);

	mp4v2dll = NIL;
}
