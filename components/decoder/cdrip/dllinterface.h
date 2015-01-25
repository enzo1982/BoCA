 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
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

typedef LONG				(CRCCONV *CR_GETNUMCDROM)			();
typedef CDROMDRIVE *			(CRCCONV *CR_OPENCDROM)				(LONG);
typedef CDEX_ERR			(CRCCONV *CR_CLOSECDROM)			(CDROMDRIVE *);
typedef CDEX_ERR			(CRCCONV *CR_READTOC)				(CDROMDRIVE *);
typedef LONG				(CRCCONV *CR_GETNUMTOCENTRIES)			(CDROMDRIVE *);
typedef TOCENTRY			(CRCCONV *CR_GETTOCENTRY)			(CDROMDRIVE *, LONG);
typedef CDEX_ERR			(CRCCONV *CR_OPENRIPPER)			(CDROMDRIVE *, LONG *, LONG, LONG);
typedef CDEX_ERR			(CRCCONV *CR_CLOSERIPPER)			(CDROMDRIVE *);
typedef CDEX_ERR			(CRCCONV *CR_RIPCHUNK)				(CDROMDRIVE *, BYTE *, LONG *, BOOL &);
typedef CDEX_ERR			(CRCCONV *CR_GETCDROMPARAMETERS)		(CDROMDRIVE *, CDROMPARAMS *);
typedef CDEX_ERR			(CRCCONV *CR_SETCDROMPARAMETERS)		(CDROMDRIVE *, CDROMPARAMS *);
typedef void				(CRCCONV *CR_LOCKCD)				(CDROMDRIVE *, BOOL);
typedef CDEX_ERR			(CRCCONV *CR_READCDTEXT)			(CDROMDRIVE *, BYTE *, int, LPINT);
typedef CDEX_ERR			(CRCCONV *CR_READANDGETISRC)			(CDROMDRIVE *, ISRC *, int);
typedef LONG				(CRCCONV *CR_GETNUMBEROFCACHEERRORS)		(CDROMDRIVE *);

extern CR_GETNUMCDROM			 ex_CR_GetNumCDROM;
extern CR_OPENCDROM			 ex_CR_OpenCDROM;
extern CR_CLOSECDROM			 ex_CR_CloseCDROM;
extern CR_READTOC			 ex_CR_ReadToc;
extern CR_GETNUMTOCENTRIES		 ex_CR_GetNumTocEntries;
extern CR_GETTOCENTRY			 ex_CR_GetTocEntry;
extern CR_OPENRIPPER			 ex_CR_OpenRipper;
extern CR_CLOSERIPPER			 ex_CR_CloseRipper;
extern CR_RIPCHUNK			 ex_CR_RipChunk;
extern CR_GETCDROMPARAMETERS		 ex_CR_GetCDROMParameters;
extern CR_SETCDROMPARAMETERS		 ex_CR_SetCDROMParameters;
extern CR_LOCKCD			 ex_CR_LockCD;
extern CR_READCDTEXT			 ex_CR_ReadCDText;
extern CR_READANDGETISRC		 ex_CR_ReadAndGetISRC;
extern CR_GETNUMBEROFCACHEERRORS	 ex_CR_GetNumberOfCacheErrors;
