 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "mad/mad.h"

using namespace smooth;
using namespace smooth::System;

extern DynamicLoader	*maddll;

Bool			 LoadMADDLL();
Void			 FreeMADDLL();

typedef void			(*MAD_DECODER_INIT)	(mad_decoder *, void *, mad_flow (*)(void *, mad_stream *), mad_flow(*)(void *, mad_header const *), mad_flow(*)(void *, mad_stream const *, mad_frame *), mad_flow(*)(void *, mad_header const *, mad_pcm *), mad_flow(*)(void *, mad_stream *, mad_frame *), mad_flow(*)(void *, void *, unsigned int *));
typedef int			(*MAD_DECODER_RUN)	(mad_decoder *, mad_decoder_mode);
typedef int			(*MAD_DECODER_FINISH)	(mad_decoder *);
typedef void			(*MAD_STREAM_BUFFER)	(mad_stream *, unsigned char const *, unsigned long);

extern MAD_DECODER_INIT		 ex_mad_decoder_init;
extern MAD_DECODER_RUN		 ex_mad_decoder_run;
extern MAD_DECODER_FINISH	 ex_mad_decoder_finish;
extern MAD_STREAM_BUFFER	 ex_mad_stream_buffer;

