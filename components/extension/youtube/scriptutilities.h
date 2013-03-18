 /* BonkEnc Audio Encoder
  * Copyright (C) 2001-2012 Robert Kausch <robert.kausch@bonkenc.org>
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
	v8::Handle<v8::Value>	 Alert(const v8::Arguments &args);

	v8::Handle<v8::Value>	 DownloadURL(const v8::Arguments &args);
};

#endif
