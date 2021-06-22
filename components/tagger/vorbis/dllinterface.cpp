 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

using namespace BoCA;

OGGSTREAMINIT		 ex_ogg_stream_init		= NIL;
OGGSTREAMPACKETIN	 ex_ogg_stream_packetin		= NIL;
OGGSTREAMPACKETOUT	 ex_ogg_stream_packetout	= NIL;
OGGSTREAMFLUSH		 ex_ogg_stream_flush		= NIL;
OGGSTREAMPAGEIN		 ex_ogg_stream_pagein		= NIL;
OGGPAGESERIALNO		 ex_ogg_page_serialno		= NIL;
OGGSTREAMCLEAR		 ex_ogg_stream_clear		= NIL;
OGGSYNCINIT		 ex_ogg_sync_init		= NIL;
OGGSYNCBUFFER		 ex_ogg_sync_buffer		= NIL;
OGGSYNCWROTE		 ex_ogg_sync_wrote		= NIL;
OGGSYNCPAGEOUT		 ex_ogg_sync_pageout		= NIL;
OGGSYNCCLEAR		 ex_ogg_sync_clear		= NIL;

DynamicLoader *oggdll = NIL;

Bool LoadOggDLL()
{
	oggdll = BoCA::Utilities::LoadCodecDLL("ogg");

	if (oggdll == NIL) return False;

	ex_ogg_stream_init		= (OGGSTREAMINIT) oggdll->GetFunctionAddress("ogg_stream_init");
	ex_ogg_stream_packetin		= (OGGSTREAMPACKETIN) oggdll->GetFunctionAddress("ogg_stream_packetin");
	ex_ogg_stream_packetout		= (OGGSTREAMPACKETOUT) oggdll->GetFunctionAddress("ogg_stream_packetout");
	ex_ogg_stream_flush		= (OGGSTREAMFLUSH) oggdll->GetFunctionAddress("ogg_stream_flush");
	ex_ogg_stream_pagein		= (OGGSTREAMPAGEIN) oggdll->GetFunctionAddress("ogg_stream_pagein");
	ex_ogg_page_serialno		= (OGGPAGESERIALNO) oggdll->GetFunctionAddress("ogg_page_serialno");
	ex_ogg_stream_clear		= (OGGSTREAMCLEAR) oggdll->GetFunctionAddress("ogg_stream_clear");
	ex_ogg_sync_init		= (OGGSYNCINIT) oggdll->GetFunctionAddress("ogg_sync_init");
	ex_ogg_sync_buffer		= (OGGSYNCBUFFER) oggdll->GetFunctionAddress("ogg_sync_buffer");
	ex_ogg_sync_wrote		= (OGGSYNCWROTE) oggdll->GetFunctionAddress("ogg_sync_wrote");
	ex_ogg_sync_pageout		= (OGGSYNCPAGEOUT) oggdll->GetFunctionAddress("ogg_sync_pageout");
	ex_ogg_sync_clear		= (OGGSYNCCLEAR) oggdll->GetFunctionAddress("ogg_sync_clear");

	if (ex_ogg_stream_init		== NIL ||
	    ex_ogg_stream_packetin	== NIL ||
	    ex_ogg_stream_packetout	== NIL ||
	    ex_ogg_stream_flush		== NIL ||
	    ex_ogg_stream_pagein	== NIL ||
	    ex_ogg_page_serialno	== NIL ||
	    ex_ogg_stream_clear		== NIL ||
	    ex_ogg_sync_init		== NIL ||
	    ex_ogg_sync_buffer		== NIL ||
	    ex_ogg_sync_wrote		== NIL ||
	    ex_ogg_sync_pageout		== NIL ||
	    ex_ogg_sync_clear		== NIL) { FreeOggDLL(); return False; }

	return True;
}

Void FreeOggDLL()
{
	BoCA::Utilities::FreeCodecDLL(oggdll);

	oggdll = NIL;
}
