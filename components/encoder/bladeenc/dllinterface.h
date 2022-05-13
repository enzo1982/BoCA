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
#include <windows.h>

#include <bladedll.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*bladedll;

Bool			 LoadBladeDLL();
Void			 FreeBladeDLL();

typedef BE_ERR		(*BEINITSTREAM)		(PBE_CONFIG, PDWORD, PDWORD, PHBE_STREAM);
typedef BE_ERR		(*BEENCODECHUNK)	(HBE_STREAM, DWORD, PSHORT, PBYTE, PDWORD);
typedef BE_ERR		(*BEDEINITSTREAM)	(HBE_STREAM, PBYTE, PDWORD);
typedef BE_ERR		(*BECLOSESTREAM)	(HBE_STREAM);
typedef VOID		(*BEVERSION)		(PBE_VERSION);

extern BEINITSTREAM	 ex_beInitStream;
extern BEENCODECHUNK	 ex_beEncodeChunk;
extern BEDEINITSTREAM	 ex_beDeinitStream;
extern BECLOSESTREAM	 ex_beCloseStream;
extern BEVERSION	 ex_beVersion;
