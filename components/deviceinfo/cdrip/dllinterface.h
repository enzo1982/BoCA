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

typedef CDEX_ERR			(CRCCONV *CR_INIT)				(int);
typedef CDEX_ERR			(CRCCONV *CR_DEINIT)				();
typedef BOOL				(CRCCONV *CR_ISINITIALIZED)			();
typedef LONG				(CRCCONV *CR_GETNUMCDROM)			();
typedef CDROMDRIVE *			(CRCCONV *CR_OPENCDROM)				(LONG);
typedef CDEX_ERR			(CRCCONV *CR_CLOSECDROM)			(CDROMDRIVE *);
typedef CDEX_ERR			(CRCCONV *CR_READTOC)				(CDROMDRIVE *);
typedef LONG				(CRCCONV *CR_GETNUMTOCENTRIES)			(CDROMDRIVE *);
typedef TOCENTRY			(CRCCONV *CR_GETTOCENTRY)			(CDROMDRIVE *, LONG);
typedef CDEX_ERR			(CRCCONV *CR_GETCDROMPARAMETERS)		(CDROMDRIVE *, CDROMPARAMS *);
typedef BOOL				(CRCCONV *CR_EJECTCD)				(CDROMDRIVE *, BOOL);

extern CR_INIT				 ex_CR_Init;
extern CR_DEINIT			 ex_CR_DeInit;
extern CR_ISINITIALIZED			 ex_CR_IsInitialized;
extern CR_GETNUMCDROM			 ex_CR_GetNumCDROM;
extern CR_OPENCDROM			 ex_CR_OpenCDROM;
extern CR_CLOSECDROM			 ex_CR_CloseCDROM;
extern CR_READTOC			 ex_CR_ReadToc;
extern CR_GETNUMTOCENTRIES		 ex_CR_GetNumTocEntries;
extern CR_GETTOCENTRY			 ex_CR_GetTocEntry;
extern CR_GETCDROMPARAMETERS		 ex_CR_GetCDROMParameters;
extern CR_EJECTCD			 ex_CR_EjectCD;
