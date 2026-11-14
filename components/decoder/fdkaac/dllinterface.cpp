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

#include <boca.h>
#include "dllinterface.h"

AACDECODER_OPEN			 ex_aacDecoder_Open			= NIL;
AACDECODER_CONFIGRAW		 ex_aacDecoder_ConfigRaw		= NIL;
AACDECODER_FILL			 ex_aacDecoder_Fill			= NIL;
AACDECODER_GETSTREAMINFO	 ex_aacDecoder_GetStreamInfo		= NIL;
AACDECODER_DECODEFRAME		 ex_aacDecoder_DecodeFrame		= NIL;
AACDECODER_CLOSE		 ex_aacDecoder_Close			= NIL;
AACDECODER_GETLIBINFO		 ex_aacDecoder_GetLibInfo		= NIL;

DynamicLoader *fdkaacdll	= NIL;

Bool LoadFDKAACDLL()
{
	fdkaacdll = BoCA::Utilities::LoadCodecDLL("fdk-aac");

	if (fdkaacdll == NIL) return False;

	ex_aacDecoder_Open		= (AACDECODER_OPEN) fdkaacdll->GetFunctionAddress("aacDecoder_Open");
	ex_aacDecoder_ConfigRaw		= (AACDECODER_CONFIGRAW) fdkaacdll->GetFunctionAddress("aacDecoder_ConfigRaw");
	ex_aacDecoder_Fill		= (AACDECODER_FILL) fdkaacdll->GetFunctionAddress("aacDecoder_Fill");
	ex_aacDecoder_GetStreamInfo	= (AACDECODER_GETSTREAMINFO) fdkaacdll->GetFunctionAddress("aacDecoder_GetStreamInfo");
	ex_aacDecoder_DecodeFrame	= (AACDECODER_DECODEFRAME) fdkaacdll->GetFunctionAddress("aacDecoder_DecodeFrame");
	ex_aacDecoder_Close		= (AACDECODER_CLOSE) fdkaacdll->GetFunctionAddress("aacDecoder_Close");
	ex_aacDecoder_GetLibInfo	= (AACDECODER_GETLIBINFO) fdkaacdll->GetFunctionAddress("aacDecoder_GetLibInfo");

	if (ex_aacDecoder_Open		== NIL ||
	    ex_aacDecoder_ConfigRaw	== NIL ||
	    ex_aacDecoder_Fill		== NIL ||
	    ex_aacDecoder_GetStreamInfo	== NIL ||
	    ex_aacDecoder_DecodeFrame	== NIL ||
	    ex_aacDecoder_Close		== NIL ||
	    ex_aacDecoder_GetLibInfo	== NIL) { FreeFDKAACDLL(); return False; }

	return True;
}

Void FreeFDKAACDLL()
{
	BoCA::Utilities::FreeCodecDLL(fdkaacdll);

	fdkaacdll = NIL;
}
