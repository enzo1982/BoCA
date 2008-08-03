 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_BOCA_DLLINTERFACES
#define H_BOCA_DLLINTERFACES

#include <smooth.h>

#include "3rdparty/id3.h"
#include "3rdparty/mp4/mp4.h"

using namespace smooth;
using namespace smooth::System;

namespace BoCA
{
	class DLLInterfaces
	{
		private:
			static DynamicLoader	*id3dll;
			static DynamicLoader	*mp4v2dll;
		public:
			static Bool		 LoadID3DLL();
			static Void		 FreeID3DLL();

			static Bool		 LoadMP4v2DLL();
			static Void		 FreeMP4v2DLL();
	};
};

// ID3Lib DLL API

	typedef ID3Tag *			(*ID3TAGNEW)			();
	typedef void				(*ID3TAGDELETE)			(ID3Tag *);
	typedef void				(*ID3TAGSETPADDING)		(ID3Tag *, bool);
	typedef void				(*ID3TAGADDFRAME)		(ID3Tag *, const ID3Frame *);
	typedef size_t				(*ID3TAGLINK)			(ID3Tag *, const char *);
	typedef size_t				(*ID3TAGNUMFRAMES)		(ID3Tag *);
	typedef ID3Frame *			(*ID3TAGFINDFRAMEWITHID)	(const ID3Tag *, ID3_FrameID);
	typedef size_t				(*ID3TAGPARSEBUFFER)		(ID3Tag *tag, const uchar *buffer, size_t bytes);
	typedef size_t				(*ID3TAGSIZE)			(const ID3Tag *);
	typedef size_t				(*ID3TAGRENDER)			(const ID3Tag *, uchar *, ID3_TagType);
	typedef ID3TagIterator *		(*ID3TAGCREATEITERATOR)		(ID3Tag *);
	typedef ID3Frame *			(*ID3TAGITERATORGETNEXT)	(ID3TagIterator *);
	typedef void				(*ID3TAGITERATORDELETE)		(ID3TagIterator *);
	typedef ID3_FrameID			(*ID3FRAMEGETID)		(const ID3Frame *);
	typedef ID3Frame *			(*ID3FRAMENEWID)		(ID3_FrameID);
	typedef void				(*ID3FRAMEDELETE)		(ID3Frame *);
	typedef ID3Field *			(*ID3FRAMEGETFIELD)		(const ID3Frame *, ID3_FieldID);
	typedef size_t				(*ID3FIELDSIZE)			(const ID3Field *);
	typedef void				(*ID3FIELDSETBINARY)		(ID3Field *, const uchar *, size_t);
	typedef void				(*ID3FIELDGETBINARY)		(const ID3Field *, uchar *, size_t);
	typedef void				(*ID3FIELDSETINT)		(const ID3Field *, uint32);
	typedef uint32				(*ID3FIELDGETINT)		(const ID3Field *);
	typedef bool				(*ID3FIELDSETENCODING)		(ID3Field *, ID3_TextEnc);
	typedef void				(*ID3FIELDSETASCII)		(ID3Field *, const char *);
	typedef size_t				(*ID3FIELDGETASCII)		(const ID3Field *, char *, size_t);
	typedef void				(*ID3FIELDSETUNICODE)		(ID3Field *, unicode_t *);
	typedef size_t				(*ID3FIELDGETUNICODE)		(const ID3Field *, unicode_t *, size_t);

	extern ID3TAGNEW			 ex_ID3Tag_New;
	extern ID3TAGDELETE			 ex_ID3Tag_Delete;
	extern ID3TAGSETPADDING			 ex_ID3Tag_SetPadding;
	extern ID3TAGADDFRAME			 ex_ID3Tag_AddFrame;
	extern ID3TAGLINK			 ex_ID3Tag_Link;
	extern ID3TAGNUMFRAMES			 ex_ID3Tag_NumFrames;
	extern ID3TAGFINDFRAMEWITHID		 ex_ID3Tag_FindFrameWithID;
	extern ID3TAGPARSEBUFFER		 ex_ID3Tag_ParseBuffer;
	extern ID3TAGSIZE			 ex_ID3Tag_Size;
	extern ID3TAGRENDER			 ex_ID3Tag_Render;
	extern ID3TAGCREATEITERATOR		 ex_ID3Tag_CreateIterator;
	extern ID3TAGITERATORGETNEXT		 ex_ID3TagIterator_GetNext;
	extern ID3TAGITERATORDELETE		 ex_ID3TagIterator_Delete;
	extern ID3FRAMEGETID			 ex_ID3Frame_GetID;
	extern ID3FRAMENEWID			 ex_ID3Frame_NewID;
	extern ID3FRAMEDELETE			 ex_ID3Frame_Delete;
	extern ID3FRAMEGETFIELD			 ex_ID3Frame_GetField;
	extern ID3FIELDSIZE			 ex_ID3Field_Size;
	extern ID3FIELDSETBINARY		 ex_ID3Field_SetBINARY;
	extern ID3FIELDGETBINARY		 ex_ID3Field_GetBINARY;
	extern ID3FIELDSETINT			 ex_ID3Field_SetINT;
	extern ID3FIELDGETINT			 ex_ID3Field_GetINT;
	extern ID3FIELDSETENCODING		 ex_ID3Field_SetEncoding;
	extern ID3FIELDSETASCII			 ex_ID3Field_SetASCII;
	extern ID3FIELDGETASCII			 ex_ID3Field_GetASCII;
	extern ID3FIELDSETUNICODE		 ex_ID3Field_SetUNICODE;
	extern ID3FIELDGETUNICODE		 ex_ID3Field_GetUNICODE;

// MP4v2 DLL API

	typedef MP4FileHandle			(*MP4READ)			(const char *, u_int32_t);
	typedef MP4FileHandle			(*MP4MODIFY)			(const char *, u_int32_t, u_int32_t);
	typedef bool				(*MP4CLOSE)			(MP4FileHandle);
	typedef void				(*MP4FREE)			(void *);
	typedef bool				(*MP4OPTIMIZE)			(const char *, const char *, u_int32_t);
	typedef bool				(*MP4GETMETADATANAME)		(MP4FileHandle, char **);
	typedef bool				(*MP4GETMETADATAARTIST)		(MP4FileHandle, char **);
	typedef bool				(*MP4GETMETADATACOMMENT)	(MP4FileHandle, char **);
	typedef bool				(*MP4GETMETADATAYEAR)		(MP4FileHandle, char **);
	typedef bool				(*MP4GETMETADATAALBUM)		(MP4FileHandle, char **);
	typedef bool				(*MP4GETMETADATAGENRE)		(MP4FileHandle, char **);
	typedef bool				(*MP4GETMETADATATRACK)		(MP4FileHandle, u_int16_t *, u_int16_t *);
	typedef bool				(*MP4GETMETADATACOVERART)	(MP4FileHandle, u_int8_t **, u_int32_t *, uint32_t);
	typedef u_int32_t			(*MP4GETMETADATACOVERARTCOUNT)	(MP4FileHandle);
	typedef bool				(*MP4SETMETADATANAME)		(MP4FileHandle, const char *);
	typedef bool				(*MP4SETMETADATAARTIST)		(MP4FileHandle, const char *);
	typedef bool				(*MP4SETMETADATACOMMENT)	(MP4FileHandle, const char *);
	typedef bool				(*MP4SETMETADATAYEAR)		(MP4FileHandle, const char *);
	typedef bool				(*MP4SETMETADATAALBUM)		(MP4FileHandle, const char *);
	typedef bool				(*MP4SETMETADATAGENRE)		(MP4FileHandle, const char *);
	typedef bool				(*MP4SETMETADATATRACK)		(MP4FileHandle, u_int16_t, u_int16_t);
	typedef bool				(*MP4SETMETADATACOVERART)	(MP4FileHandle, u_int8_t *, u_int32_t);

	extern MP4READ				 ex_MP4Read;
	extern MP4MODIFY			 ex_MP4Modify;
	extern MP4CLOSE				 ex_MP4Close;
	extern MP4FREE				 ex_MP4Free;
	extern MP4OPTIMIZE			 ex_MP4Optimize;
	extern MP4GETMETADATANAME		 ex_MP4GetMetadataName;
	extern MP4GETMETADATAARTIST		 ex_MP4GetMetadataArtist;
	extern MP4GETMETADATACOMMENT		 ex_MP4GetMetadataComment;
	extern MP4GETMETADATAYEAR		 ex_MP4GetMetadataYear;
	extern MP4GETMETADATAALBUM		 ex_MP4GetMetadataAlbum;
	extern MP4GETMETADATAGENRE		 ex_MP4GetMetadataGenre;
	extern MP4GETMETADATATRACK		 ex_MP4GetMetadataTrack;
	extern MP4GETMETADATACOVERART		 ex_MP4GetMetadataCoverArt;
	extern MP4GETMETADATACOVERARTCOUNT	 ex_MP4GetMetadataCoverArtCount;
	extern MP4SETMETADATANAME		 ex_MP4SetMetadataName;
	extern MP4SETMETADATAARTIST		 ex_MP4SetMetadataArtist;
	extern MP4SETMETADATACOMMENT		 ex_MP4SetMetadataComment;
	extern MP4SETMETADATAYEAR		 ex_MP4SetMetadataYear;
	extern MP4SETMETADATAALBUM		 ex_MP4SetMetadataAlbum;
	extern MP4SETMETADATAGENRE		 ex_MP4SetMetadataGenre;
	extern MP4SETMETADATATRACK		 ex_MP4SetMetadataTrack;
	extern MP4SETMETADATACOVERART		 ex_MP4SetMetadataCoverArt;

#endif
