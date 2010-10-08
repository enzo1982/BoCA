 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

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

DynamicLoader *id3dll	= NIL;

Bool LoadID3DLL()
{
#ifdef __WIN32__
	if (!File(String(GUI::Application::GetApplicationDirectory()).Append("ID3Lib.dll")).Exists()) return False;
#endif

	id3dll = new DynamicLoader("ID3Lib");

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

Void FreeID3DLL()
{
	Object::DeleteObject(id3dll);

	id3dll = NIL;
}
