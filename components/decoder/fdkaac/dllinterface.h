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

#include <fdk-aac/aacdecoder_lib.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*fdkaacdll;

Bool			 LoadFDKAACDLL();
Void			 FreeFDKAACDLL();

typedef HANDLE_AACDECODER		(LINKSPEC_H *AACDECODER_OPEN)		(TRANSPORT_TYPE, UINT);
typedef AAC_DECODER_ERROR		(LINKSPEC_H *AACDECODER_CONFIGRAW)	(HANDLE_AACDECODER, UCHAR *[], const UINT []);
typedef AAC_DECODER_ERROR		(LINKSPEC_H *AACDECODER_FILL)		(HANDLE_AACDECODER, UCHAR *[], const UINT [], UINT *);
typedef CStreamInfo *			(LINKSPEC_H *AACDECODER_GETSTREAMINFO)	(HANDLE_AACDECODER);
typedef AAC_DECODER_ERROR		(LINKSPEC_H *AACDECODER_DECODEFRAME)	(HANDLE_AACDECODER, INT_PCM *, const INT, const UINT);
typedef void				(LINKSPEC_H *AACDECODER_CLOSE)		(HANDLE_AACDECODER);
typedef INT				(LINKSPEC_H *AACDECODER_GETLIBINFO)	(LIB_INFO *);

extern AACDECODER_OPEN			 ex_aacDecoder_Open;
extern AACDECODER_CONFIGRAW		 ex_aacDecoder_ConfigRaw;
extern AACDECODER_FILL			 ex_aacDecoder_Fill;
extern AACDECODER_GETSTREAMINFO		 ex_aacDecoder_GetStreamInfo;
extern AACDECODER_DECODEFRAME		 ex_aacDecoder_DecodeFrame;
extern AACDECODER_CLOSE			 ex_aacDecoder_Close;
extern AACDECODER_GETLIBINFO		 ex_aacDecoder_GetLibInfo;
