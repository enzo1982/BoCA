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

#include <smooth.h>

#ifdef callbacks
#	undef callbacks
#endif

#include <vo-aacenc/voAAC.h>
#include <mp4v2/mp4v2.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*voaacencdll;
extern DynamicLoader	*mp4v2dll;

Bool			 LoadVOAACEncDLL();
Void			 FreeVOAACEncDLL();

Bool			 LoadMP4v2DLL();
Void			 FreeMP4v2DLL();

typedef VO_S32				(VO_API *VOGETAACENCAPI)	(VO_AUDIO_CODECAPI *);

typedef VO_U32				(*CMNMEMALLOC)			(VO_S32, VO_MEM_INFO *);
typedef VO_U32				(*CMNMEMFREE)			(VO_S32, VO_PTR);
typedef VO_U32				(*CMNMEMSET)			(VO_S32, VO_PTR, VO_U8, VO_U32);
typedef VO_U32				(*CMNMEMCOPY)			(VO_S32, VO_PTR, VO_PTR, VO_U32);
typedef VO_U32				(*CMNMEMCHECK)			(VO_S32, VO_PTR, VO_U32);
typedef VO_S32				(*CMNMEMCOMPARE)		(VO_S32, VO_PTR, VO_PTR, VO_U32);
typedef VO_U32				(*CMNMEMMOVE)			(VO_S32, VO_PTR, VO_PTR, VO_U32);

extern VOGETAACENCAPI			 ex_voGetAACEncAPI;

extern CMNMEMALLOC			 ex_cmnMemAlloc;
extern CMNMEMFREE			 ex_cmnMemFree;
extern CMNMEMSET			 ex_cmnMemSet;
extern CMNMEMCOPY			 ex_cmnMemCopy;
extern CMNMEMCHECK			 ex_cmnMemCheck;
extern CMNMEMCOMPARE			 ex_cmnMemCompare;
extern CMNMEMMOVE			 ex_cmnMemMove;

typedef MP4FileHandle			(*MP4CREATECALLBACKS)		(const MP4IOCallbacks *, void *, uint32_t);
typedef void				(*MP4CLOSE)			(MP4FileHandle, uint32_t);
typedef bool				(*MP4OPTIMIZE)			(const char *, const char *);

typedef bool				(*MP4SETTRACKESCONFIGURATION)	(MP4FileHandle, MP4TrackId, const uint8_t *, uint32_t);
typedef void				(*MP4SETAUDIOPROFILELEVEL)	(MP4FileHandle, uint8_t);
typedef MP4TrackId			(*MP4ADDAUDIOTRACK)		(MP4FileHandle, uint32_t, MP4Duration, uint8_t);
typedef bool				(*MP4WRITESAMPLE)		(MP4FileHandle, MP4TrackId, const uint8_t *, uint32_t, MP4Duration, MP4Duration, bool);

typedef MP4ItmfItem *			(*MP4ITMFITEMALLOC)		(const char *, uint32_t);
typedef void				(*MP4ITMFITEMFREE)		(MP4ItmfItem *);
typedef bool				(*MP4ITMFADDITEM)		(MP4FileHandle, const MP4ItmfItem *);

extern MP4CREATECALLBACKS		 ex_MP4CreateCallbacks;
extern MP4CLOSE				 ex_MP4Close;
extern MP4OPTIMIZE			 ex_MP4Optimize;

extern MP4SETTRACKESCONFIGURATION	 ex_MP4SetTrackESConfiguration;
extern MP4SETAUDIOPROFILELEVEL		 ex_MP4SetAudioProfileLevel;
extern MP4ADDAUDIOTRACK			 ex_MP4AddAudioTrack;
extern MP4WRITESAMPLE			 ex_MP4WriteSample;

extern MP4ITMFITEMALLOC			 ex_MP4ItmfItemAlloc;
extern MP4ITMFITEMFREE			 ex_MP4ItmfItemFree;
extern MP4ITMFADDITEM			 ex_MP4ItmfAddItem;
