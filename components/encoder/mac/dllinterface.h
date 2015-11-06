 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*macdll;

Bool			 LoadMACDLL();
Void			 FreeMACDLL();

typedef APE_COMPRESS_HANDLE	(__stdcall *APECOMPRESS_CREATE)		(int *);
typedef void			(__stdcall *APECOMPRESS_DESTROY)	(APE_COMPRESS_HANDLE);
typedef int			(__stdcall *APECOMPRESS_START)		(APE_COMPRESS_HANDLE, const char *, const WAVEFORMATEX *, int, int, const unsigned char, int);
typedef int			(__stdcall *APECOMPRESS_ADDDATA)	(APE_COMPRESS_HANDLE, unsigned char *, int);
typedef int			(__stdcall *APECOMPRESS_FINISH)		(APE_COMPRESS_HANDLE, unsigned char *, int, int);
typedef int			(__stdcall *APEGETVERSIONNUMBER)	();

extern APECOMPRESS_CREATE	 ex_APECompress_Create;
extern APECOMPRESS_DESTROY	 ex_APECompress_Destroy;
extern APECOMPRESS_START	 ex_APECompress_Start;
extern APECOMPRESS_ADDDATA	 ex_APECompress_AddData;
extern APECOMPRESS_FINISH	 ex_APECompress_Finish;
extern APEGETVERSIONNUMBER	 ex_APEGetVersionNumber;
