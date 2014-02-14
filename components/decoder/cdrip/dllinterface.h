 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
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

typedef CDEX_ERR			(CRCCONV *CR_READTOC)				();
typedef LONG				(CRCCONV *CR_GETNUMTOCENTRIES)			();
typedef TOCENTRY			(CRCCONV *CR_GETTOCENTRY)			(LONG);
typedef CDEX_ERR			(CRCCONV *CR_OPENRIPPER)			(LONG *, LONG, LONG);
typedef CDEX_ERR			(CRCCONV *CR_CLOSERIPPER)			();
typedef CDEX_ERR			(CRCCONV *CR_RIPCHUNK)				(BYTE *, LONG *, BOOL &);
typedef void				(CRCCONV *CR_SETACTIVECDROM)			(LONG);
typedef CDEX_ERR			(CRCCONV *CR_GETCDROMPARAMETERS)		(CDROMPARAMS *);
typedef CDEX_ERR			(CRCCONV *CR_SETCDROMPARAMETERS)		(CDROMPARAMS *);
typedef void				(CRCCONV *CR_LOCKCD)				(BOOL);
typedef CDEX_ERR			(CRCCONV *CR_READCDTEXT)			(BYTE *, int, LPINT);
typedef CDEX_ERR			(CRCCONV *CR_READANDGETISRC)			(ISRC *, int);
typedef LONG				(CRCCONV *CR_GETNUMBEROFCACHEERRORS)		();

extern CR_READTOC			 ex_CR_ReadToc;
extern CR_GETNUMTOCENTRIES		 ex_CR_GetNumTocEntries;
extern CR_GETTOCENTRY			 ex_CR_GetTocEntry;
extern CR_OPENRIPPER			 ex_CR_OpenRipper;
extern CR_CLOSERIPPER			 ex_CR_CloseRipper;
extern CR_RIPCHUNK			 ex_CR_RipChunk;
extern CR_SETACTIVECDROM		 ex_CR_SetActiveCDROM;
extern CR_GETCDROMPARAMETERS		 ex_CR_GetCDROMParameters;
extern CR_SETCDROMPARAMETERS		 ex_CR_SetCDROMParameters;
extern CR_LOCKCD			 ex_CR_LockCD;
extern CR_READCDTEXT			 ex_CR_ReadCDText;
extern CR_READANDGETISRC		 ex_CR_ReadAndGetISRC;
extern CR_GETNUMBEROFCACHEERRORS	 ex_CR_GetNumberOfCacheErrors;
