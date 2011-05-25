 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_INIT_WIN32
#define H_OBJSMOOTH_INIT_WIN32

#include <windows.h>

#include "definitions.h"

namespace smooth
{
#ifdef __WIN32__
	SMOOTHVAR HINSTANCE	 hInstance;
	SMOOTHVAR HINSTANCE	 hPrevInstance;

	SMOOTHVAR HICON		 SMOOTHICON;
#endif
};

#endif
