 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
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

#define SMOOTHAPI __attribute__ ((visibility ("default")))
#define SMOOTHVAR extern __attribute__ ((visibility ("default")))

#define SMOOTH_PLUGIN_API __attribute__ ((visibility ("default")))
#define SMOOTH_PLUGIN_VAR extern __attribute__ ((visibility ("default")))

#ifndef __declspec
#	define __declspec(x)
#endif

#endif
