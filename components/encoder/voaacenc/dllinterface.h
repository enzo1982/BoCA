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

#include <smooth.h>

#include <vo-aacenc/voAAC.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*voaacencdll;

Bool			 LoadVOAACEncDLL();
Void			 FreeVOAACEncDLL();

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
