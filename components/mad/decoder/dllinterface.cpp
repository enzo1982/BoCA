 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

MAD_DECODER_INIT	 ex_mad_decoder_init	= NIL;
MAD_DECODER_RUN		 ex_mad_decoder_run	= NIL;
MAD_DECODER_FINISH	 ex_mad_decoder_finish	= NIL;
MAD_STREAM_BUFFER	 ex_mad_stream_buffer	= NIL;

DynamicLoader *maddll	= NIL;

Bool LoadMADDLL()
{
	maddll = new DynamicLoader("codecs/MAD");

	ex_mad_decoder_init	= (MAD_DECODER_INIT) maddll->GetFunctionAddress("mad_decoder_init");
	ex_mad_decoder_run	= (MAD_DECODER_RUN) maddll->GetFunctionAddress("mad_decoder_run");
	ex_mad_decoder_finish	= (MAD_DECODER_FINISH) maddll->GetFunctionAddress("mad_decoder_finish");
	ex_mad_stream_buffer	= (MAD_STREAM_BUFFER) maddll->GetFunctionAddress("mad_stream_buffer");

	if (ex_mad_decoder_init		== NIL ||
	    ex_mad_decoder_run		== NIL ||
	    ex_mad_decoder_finish	== NIL ||
	    ex_mad_stream_buffer	== NIL) { FreeMADDLL(); return False; }

	return True;
}

Void FreeMADDLL()
{
	Object::DeleteObject(maddll);

	maddll = NIL;
}
