 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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
typedef CDEX_ERR			(_stdcall *CR_READTOC)				();
typedef LONG				(_stdcall *CR_GETNUMTOCENTRIES)			();
typedef TOCENTRY			(_stdcall *CR_GETTOCENTRY)			(LONG);
typedef CDEX_ERR			(_stdcall *CR_OPENRIPPER)			(LONG *, LONG, LONG);
typedef CDEX_ERR			(_stdcall *CR_CLOSERIPPER)			();
typedef CDEX_ERR			(_stdcall *CR_RIPCHUNK)				(BYTE *, LONG *, BOOL &);
typedef LONG				(_stdcall *CR_GETNUMCDROM)			();
typedef LONG				(_stdcall *CR_GETACTIVECDROM)			();
typedef void				(_stdcall *CR_SETACTIVECDROM)			(LONG);
typedef CDEX_ERR			(_stdcall *CR_GETCDROMPARAMETERS)		(CDROMPARAMS *);
typedef CDEX_ERR			(_stdcall *CR_SETCDROMPARAMETERS)		(CDROMPARAMS *);
typedef LONG				(_stdcall *CR_GETNUMBEROFJITTERERRORS)		();
typedef LONG				(_stdcall *CR_GETNUMBEROFC2ERRORS)		();
typedef void				(_stdcall *CR_LOCKCD)				(BOOL);
typedef BOOL				(_stdcall *CR_EJECTCD)				(BOOL);
typedef CDEX_ERR			(_stdcall *CR_READCDTEXT)			(BYTE *, int, LPINT);

extern CR_INIT				 ex_CR_Init;
extern CR_DEINIT			 ex_CR_DeInit;
extern CR_READTOC			 ex_CR_ReadToc;
extern CR_GETNUMTOCENTRIES		 ex_CR_GetNumTocEntries;
extern CR_GETTOCENTRY			 ex_CR_GetTocEntry;
extern CR_OPENRIPPER			 ex_CR_OpenRipper;
extern CR_CLOSERIPPER			 ex_CR_CloseRipper;
extern CR_RIPCHUNK			 ex_CR_RipChunk;
extern CR_GETNUMCDROM			 ex_CR_GetNumCDROM;
extern CR_GETACTIVECDROM		 ex_CR_GetActiveCDROM;
extern CR_SETACTIVECDROM		 ex_CR_SetActiveCDROM;
extern CR_GETCDROMPARAMETERS		 ex_CR_GetCDROMParameters;
extern CR_SETCDROMPARAMETERS		 ex_CR_SetCDROMParameters;
extern CR_GETNUMBEROFJITTERERRORS	 ex_CR_GetNumberOfJitterErrors;
extern CR_GETNUMBEROFC2ERRORS		 ex_CR_GetNumberOfC2Errors;
extern CR_LOCKCD			 ex_CR_LockCD;
extern CR_EJECTCD			 ex_CR_EjectCD;
extern CR_READCDTEXT			 ex_CR_ReadCDText;
