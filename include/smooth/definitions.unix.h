 /* The smooth Class Library
  * Copyright (C) 1998-2011 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DEFS_UNIX
#define H_OBJSMOOTH_DEFS_UNIX

#include <stdlib.h>
#include <sys/types.h>

#define SMOOTHAPI
#define SMOOTHVAR extern

#define SMOOTH_PLUGIN_API
#define SMOOTH_PLUGIN_VAR extern

#ifndef __declspec
#	define __declspec(x)
#endif

#endif
