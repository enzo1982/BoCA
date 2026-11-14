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

typedef OpusMSDecoder *			(*OPUSMULTISTREAMDECODERCREATE)		 (opus_int32, int, int, int, const unsigned char *, int *);
typedef int				(*OPUSMULTISTREAMDECODE)		 (OpusMSDecoder *, const unsigned char *, opus_int32, opus_int16 *, int, int);
typedef int				(*OPUSMULTISTREAMDECODEFLOAT)		 (OpusMSDecoder *, const unsigned char *, opus_int32, float *, int, int);
typedef int				(*OPUSMULTISTREAMDECODERCTL)		 (OpusMSDecoder *, int, ...);
typedef void				(*OPUSMULTISTREAMDECODERDESTROY)	 (OpusMSDecoder *);
typedef OpusDecoder *			(*OPUSDECODERCREATE)			 (opus_int32, int, int *);
typedef int				(*OPUSDECODERCTL)			 (OpusDecoder *, int, ...);
typedef void				(*OPUSDECODERDESTROY)			 (OpusDecoder *);
typedef const char *			(*OPUSGETVERSIONSTRING)			 ();

extern OPUSMULTISTREAMDECODERCREATE	 ex_opus_multistream_decoder_create;
extern OPUSMULTISTREAMDECODE		 ex_opus_multistream_decode;
extern OPUSMULTISTREAMDECODEFLOAT	 ex_opus_multistream_decode_float;
extern OPUSMULTISTREAMDECODERCTL	 ex_opus_multistream_decoder_ctl;
extern OPUSMULTISTREAMDECODERDESTROY	 ex_opus_multistream_decoder_destroy;
extern OPUSDECODERCREATE		 ex_opus_decoder_create;
extern OPUSDECODERCTL			 ex_opus_decoder_ctl;
extern OPUSDECODERDESTROY		 ex_opus_decoder_destroy;
extern OPUSGETVERSIONSTRING		 ex_opus_get_version_string;
