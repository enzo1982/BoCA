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

#include <opus/opus_multistream.h>

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*opusdll;

Bool			 LoadOpusDLL();
Void			 FreeOpusDLL();

/* Opus API functions.
 */
typedef OpusMSEncoder *				(*OPUSMULTISTREAMSURROUNDENCODERCREATE)	 (opus_int32, int, int, int *, int *, unsigned char *, int, int *);
typedef int					(*OPUSMULTISTREAMENCODE)		 (OpusMSEncoder *, const opus_int16 *, int, unsigned char *, opus_int32);
typedef int					(*OPUSMULTISTREAMENCODEFLOAT)		 (OpusMSEncoder *, const float *, int, unsigned char *, opus_int32);
typedef int					(*OPUSMULTISTREAMENCODERCTL)		 (OpusMSEncoder *, int, ...);
typedef void					(*OPUSMULTISTREAMENCODERDESTROY)	 (OpusMSEncoder *);
typedef const char *				(*OPUSGETVERSIONSTRING)			 ();

extern OPUSMULTISTREAMSURROUNDENCODERCREATE	 ex_opus_multistream_surround_encoder_create;
extern OPUSMULTISTREAMENCODE			 ex_opus_multistream_encode;
extern OPUSMULTISTREAMENCODEFLOAT		 ex_opus_multistream_encode_float;
extern OPUSMULTISTREAMENCODERCTL		 ex_opus_multistream_encoder_ctl;
extern OPUSMULTISTREAMENCODERDESTROY		 ex_opus_multistream_encoder_destroy;
extern OPUSGETVERSIONSTRING			 ex_opus_get_version_string;
