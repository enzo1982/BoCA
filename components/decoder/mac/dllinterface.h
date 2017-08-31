 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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

#include "mac/MACDll.h"

using namespace APE;

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*macdll;

Bool			 LoadMACDLL();
Void			 FreeMACDLL();

typedef APE_DECOMPRESS_HANDLE	(__stdcall *APEDECOMPRESS_CREATE)	(const char *, int *);
typedef void			(__stdcall *APEDECOMPRESS_DESTROY)	(APE_DECOMPRESS_HANDLE);
typedef int			(__stdcall *APEDECOMPRESS_SEEK)		(APE_DECOMPRESS_HANDLE, int);
typedef int			(__stdcall *APEDECOMPRESS_GETDATA)	(APE_DECOMPRESS_HANDLE, char *, int, int *);
typedef int			(__stdcall *APEDECOMPRESS_GETINFO)	(APE_DECOMPRESS_HANDLE, APE_DECOMPRESS_FIELDS, int, int);
typedef int			(__stdcall *APEGETVERSIONNUMBER)	();

extern APEDECOMPRESS_CREATE	 ex_APEDecompress_Create;
extern APEDECOMPRESS_DESTROY	 ex_APEDecompress_Destroy;
extern APEDECOMPRESS_SEEK	 ex_APEDecompress_Seek;
extern APEDECOMPRESS_GETDATA	 ex_APEDecompress_GetData;
extern APEDECOMPRESS_GETINFO	 ex_APEDecompress_GetInfo;
extern APEGETVERSIONNUMBER	 ex_APEGetVersionNumber;
