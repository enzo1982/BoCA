 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

using namespace BoCA;

OGGSTREAMINIT		 ex_ogg_stream_init		= NIL;
OGGSTREAMPACKETIN	 ex_ogg_stream_packetin		= NIL;
OGGSTREAMFLUSH		 ex_ogg_stream_flush		= NIL;
OGGSTREAMPAGEOUT	 ex_ogg_stream_pageout		= NIL;
OGGPAGEEOS		 ex_ogg_page_eos		= NIL;
OGGSTREAMCLEAR		 ex_ogg_stream_clear		= NIL;

OPUSENCODERCREATE	 ex_opus_encoder_create		= NIL;
OPUSENCODE		 ex_opus_encode			= NIL;
OPUSENCODERCTL		 ex_opus_encoder_ctl		= NIL;
OPUSENCODERDESTROY	 ex_opus_encoder_destroy	= NIL;
OPUSGETVERSIONSTRING	 ex_opus_get_version_string	= NIL;

DynamicLoader *oggdll	= NIL;
DynamicLoader *opusdll	= NIL;

Bool LoadOggDLL()
{
	oggdll = BoCA::Utilities::LoadCodecDLL("ogg");

	if (oggdll == NIL) return False;

	ex_ogg_stream_init	= (OGGSTREAMINIT) oggdll->GetFunctionAddress("ogg_stream_init");
	ex_ogg_stream_packetin	= (OGGSTREAMPACKETIN) oggdll->GetFunctionAddress("ogg_stream_packetin");
	ex_ogg_stream_flush	= (OGGSTREAMFLUSH) oggdll->GetFunctionAddress("ogg_stream_flush");
	ex_ogg_stream_pageout	= (OGGSTREAMPAGEOUT) oggdll->GetFunctionAddress("ogg_stream_pageout");
	ex_ogg_page_eos		= (OGGPAGEEOS) oggdll->GetFunctionAddress("ogg_page_eos");
	ex_ogg_stream_clear	= (OGGSTREAMCLEAR) oggdll->GetFunctionAddress("ogg_stream_clear");

	if (ex_ogg_stream_init		== NIL ||
	    ex_ogg_stream_packetin	== NIL ||
	    ex_ogg_stream_flush		== NIL ||
	    ex_ogg_stream_pageout	== NIL ||
	    ex_ogg_page_eos		== NIL ||
	    ex_ogg_stream_clear		== NIL) { FreeOggDLL(); return False; }

	return True;
}

Void FreeOggDLL()
{
	BoCA::Utilities::FreeCodecDLL(oggdll);

	oggdll = NIL;
}

Bool LoadOpusDLL()
{
	opusdll = BoCA::Utilities::LoadCodecDLL("opus");

	if (opusdll == NIL) return False;

	ex_opus_encoder_create		= (OPUSENCODERCREATE) opusdll->GetFunctionAddress("opus_encoder_create");
	ex_opus_encode			= (OPUSENCODE) opusdll->GetFunctionAddress("opus_encode");
	ex_opus_encoder_ctl		= (OPUSENCODERCTL) opusdll->GetFunctionAddress("opus_encoder_ctl");
	ex_opus_encoder_destroy		= (OPUSENCODERDESTROY) opusdll->GetFunctionAddress("opus_encoder_destroy");
	ex_opus_get_version_string	= (OPUSGETVERSIONSTRING) opusdll->GetFunctionAddress("opus_get_version_string");

	if (ex_opus_encoder_create	== NIL ||
	    ex_opus_encode		== NIL ||
	    ex_opus_encoder_ctl		== NIL ||
	    ex_opus_encoder_destroy	== NIL ||
	    ex_opus_get_version_string	== NIL) { FreeOpusDLL(); return False; }

	return True;
}

Void FreeOpusDLL()
{
	BoCA::Utilities::FreeCodecDLL(opusdll);

	opusdll = NIL;
}
