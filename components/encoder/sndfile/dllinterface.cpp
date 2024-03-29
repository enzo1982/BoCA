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

#include <boca.h>
#include "dllinterface.h"

SF_FORMAT_CHECK		 ex_sf_format_check	= NIL;
SF_OPEN_VIRTUAL		 ex_sf_open_virtual	= NIL;
SF_CLOSE		 ex_sf_close		= NIL;
SF_WRITE_SHORT		 ex_sf_write_short	= NIL;
SF_WRITE_INT		 ex_sf_write_int	= NIL;
SF_WRITE_FLOAT		 ex_sf_write_float	= NIL;
SF_SET_STRING		 ex_sf_set_string	= NIL;
SF_COMMAND		 ex_sf_command		= NIL;
SF_STRERROR		 ex_sf_strerror		= NIL;
SF_VERSION_STRING	 ex_sf_version_string	= NIL;

DynamicLoader *sndfiledll	= NIL;

Bool LoadSndFileDLL()
{
	sndfiledll = BoCA::Utilities::LoadCodecDLL("sndfile");

	if (sndfiledll == NIL) return False;

	ex_sf_format_check	= (SF_FORMAT_CHECK) sndfiledll->GetFunctionAddress("sf_format_check");
	ex_sf_open_virtual	= (SF_OPEN_VIRTUAL) sndfiledll->GetFunctionAddress("sf_open_virtual");
	ex_sf_close		= (SF_CLOSE) sndfiledll->GetFunctionAddress("sf_close");
	ex_sf_write_short	= (SF_WRITE_SHORT) sndfiledll->GetFunctionAddress("sf_write_short");
	ex_sf_write_int		= (SF_WRITE_INT) sndfiledll->GetFunctionAddress("sf_write_int");
	ex_sf_write_float	= (SF_WRITE_FLOAT) sndfiledll->GetFunctionAddress("sf_write_float");
	ex_sf_set_string	= (SF_SET_STRING) sndfiledll->GetFunctionAddress("sf_set_string");
	ex_sf_command		= (SF_COMMAND) sndfiledll->GetFunctionAddress("sf_command");
	ex_sf_strerror		= (SF_STRERROR) sndfiledll->GetFunctionAddress("sf_strerror");
	ex_sf_version_string	= (SF_VERSION_STRING) sndfiledll->GetFunctionAddress("sf_version_string");

	if (ex_sf_format_check		== NIL ||
	    ex_sf_open_virtual		== NIL ||
	    ex_sf_close			== NIL ||
	    ex_sf_write_short		== NIL ||
	    ex_sf_write_int		== NIL ||
	    ex_sf_write_float		== NIL ||
	    ex_sf_set_string		== NIL ||
	    ex_sf_command		== NIL ||
	    ex_sf_strerror		== NIL ||
	    ex_sf_version_string	== NIL) { FreeSndFileDLL(); return False; }

	return True;
}

Void FreeSndFileDLL()
{
	BoCA::Utilities::FreeCodecDLL(sndfiledll);

	sndfiledll = NIL;
}
