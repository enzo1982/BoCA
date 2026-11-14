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

#include <fdk-aac/aacenc_lib.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*fdkaacdll;

Bool			 LoadFDKAACDLL();
Void			 FreeFDKAACDLL();

typedef struct
{
	UINT	 maxOutBufBytes;
	UINT	 maxAncBytes;
	UINT	 inBufFillLevel;
	UINT	 inputChannels;
	UINT	 frameLength;
	UINT	 encoderDelay;
	UCHAR	 confBuf[64];
	UINT	 confSize;
}
AACENC_InfoStruct_Old;

typedef AACENC_ERROR			(*AACENCOPEN)			(HANDLE_AACENCODER *, const UINT, const UINT);
typedef AACENC_ERROR			(*AACENCCLOSE)			(HANDLE_AACENCODER *);
typedef AACENC_ERROR			(*AACENCENCODE)			(const HANDLE_AACENCODER, const AACENC_BufDesc *, const AACENC_BufDesc *, const AACENC_InArgs *, AACENC_OutArgs *);
typedef AACENC_ERROR			(*AACENCINFO)			(const HANDLE_AACENCODER, AACENC_InfoStruct *);
typedef AACENC_ERROR			(*AACENCGETLIBINFO)		(LIB_INFO *);
typedef UINT				(*AACENCODER_GETPARAM)		(const HANDLE_AACENCODER, const AACENC_PARAM);
typedef AACENC_ERROR			(*AACENCODER_SETPARAM)		(const HANDLE_AACENCODER, const AACENC_PARAM, const UINT);

extern AACENCOPEN			 ex_aacEncOpen;
extern AACENCCLOSE			 ex_aacEncClose;
extern AACENCENCODE			 ex_aacEncEncode;
extern AACENCINFO			 ex_aacEncInfo;
extern AACENCGETLIBINFO			 ex_aacEncGetLibInfo;
extern AACENCODER_GETPARAM		 ex_aacEncoder_GetParam;
extern AACENCODER_SETPARAM		 ex_aacEncoder_SetParam;
