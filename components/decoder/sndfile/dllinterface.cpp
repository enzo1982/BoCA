 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca.h>
#include "dllinterface.h"

SF_OPEN_FD	 ex_sf_open_fd		= NIL;
SF_CLOSE	 ex_sf_close		= NIL;
SF_SEEK		 ex_sf_seek		= NIL;
SF_READ_SHORT	 ex_sf_read_short	= NIL;
SF_READ_INT	 ex_sf_read_int		= NIL;
SF_GET_STRING	 ex_sf_get_string	= NIL;

DynamicLoader *sndfiledll	= NIL;

Bool LoadSndFileDLL()
{
	sndfiledll = BoCA::Utilities::LoadCodecDLL("sndfile");

	if (sndfiledll == NIL) return False;

	ex_sf_open_fd		= (SF_OPEN_FD) sndfiledll->GetFunctionAddress("sf_open_fd");
	ex_sf_close		= (SF_CLOSE) sndfiledll->GetFunctionAddress("sf_close");
	ex_sf_seek		= (SF_SEEK) sndfiledll->GetFunctionAddress("sf_seek");
	ex_sf_read_short	= (SF_READ_SHORT) sndfiledll->GetFunctionAddress("sf_read_short");
	ex_sf_read_int		= (SF_READ_INT) sndfiledll->GetFunctionAddress("sf_read_int");
	ex_sf_get_string	= (SF_GET_STRING) sndfiledll->GetFunctionAddress("sf_get_string");

	if (ex_sf_open_fd	== NIL ||
	    ex_sf_close		== NIL ||
	    ex_sf_seek		== NIL ||
	    ex_sf_read_short	== NIL ||
	    ex_sf_read_int	== NIL ||
	    ex_sf_get_string	== NIL) { FreeSndFileDLL(); return False; }

	return True;
}

Void FreeSndFileDLL()
{
	BoCA::Utilities::FreeCodecDLL(sndfiledll);

	sndfiledll = NIL;
}
