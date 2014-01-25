 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_YOUTUBE_SCRIPT_UTILITIES
#define H_YOUTUBE_SCRIPT_UTILITIES

#include <smooth.h>
#include <boca.h>

using namespace smooth;

using namespace BoCA;

#include <smooth-js/v8.h>

namespace BoCA
{
	void	 Alert(const v8::FunctionCallbackInfo<v8::Value> &);

	void	 DownloadURL(const v8::FunctionCallbackInfo<v8::Value> &);
};

#endif
