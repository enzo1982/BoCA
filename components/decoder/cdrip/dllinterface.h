 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "cdrip/cdrip.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*cdripdll;

Bool			 LoadCDRipDLL();
Void			 FreeCDRipDLL();

typedef CDEX_ERR			(_stdcall *CR_INIT)				(int);
typedef CDEX_ERR			(_stdcall *CR_DEINIT)				();
typedef BOOL				(_stdcall *CR_ISINITIALIZED)			();
typedef CDEX_ERR			(_stdcall *CR_READTOC)				();
typedef LONG				(_stdcall *CR_GETNUMTOCENTRIES)			();
typedef TOCENTRY			(_stdcall *CR_GETTOCENTRY)			(LONG);
typedef CDEX_ERR			(_stdcall *CR_OPENRIPPER)			(LONG *, LONG, LONG);
typedef CDEX_ERR			(_stdcall *CR_CLOSERIPPER)			();
typedef CDEX_ERR			(_stdcall *CR_RIPCHUNK)				(BYTE *, LONG *, BOOL &);
typedef LONG				(_stdcall *CR_GETNUMCDROM)			();
typedef void				(_stdcall *CR_SETACTIVECDROM)			(LONG);
typedef CDEX_ERR			(_stdcall *CR_GETCDROMPARAMETERS)		(CDROMPARAMS *);
typedef CDEX_ERR			(_stdcall *CR_SETCDROMPARAMETERS)		(CDROMPARAMS *);
typedef void				(_stdcall *CR_LOCKCD)				(BOOL);
typedef CDEX_ERR			(_stdcall *CR_READCDTEXT)			(BYTE *, int, LPINT);
typedef CDEX_ERR			(_stdcall *CR_READANDGETISRC)			(ISRC *, int);

extern CR_INIT				 ex_CR_Init;
extern CR_DEINIT			 ex_CR_DeInit;
extern CR_ISINITIALIZED			 ex_CR_IsInitialized;
extern CR_READTOC			 ex_CR_ReadToc;
extern CR_GETNUMTOCENTRIES		 ex_CR_GetNumTocEntries;
extern CR_GETTOCENTRY			 ex_CR_GetTocEntry;
extern CR_OPENRIPPER			 ex_CR_OpenRipper;
extern CR_CLOSERIPPER			 ex_CR_CloseRipper;
extern CR_RIPCHUNK			 ex_CR_RipChunk;
extern CR_GETNUMCDROM			 ex_CR_GetNumCDROM;
extern CR_SETACTIVECDROM		 ex_CR_SetActiveCDROM;
extern CR_GETCDROMPARAMETERS		 ex_CR_GetCDROMParameters;
extern CR_SETCDROMPARAMETERS		 ex_CR_SetCDROMParameters;
extern CR_LOCKCD			 ex_CR_LockCD;
extern CR_READCDTEXT			 ex_CR_ReadCDText;
extern CR_READANDGETISRC		 ex_CR_ReadAndGetISRC;
