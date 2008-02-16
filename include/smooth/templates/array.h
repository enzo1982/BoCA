 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_ARRAY_INCLUDE_
#define _H_OBJSMOOTH_ARRAY_INCLUDE_

#ifdef _MSC_VER
#	define ARRAY_NULLVALUE		(0)
#	define ARRAY_NULLPOINTER	(0)
#else
#	define ARRAY_NULLVALUE		((s)				0)
#	define ARRAY_NULLPOINTER	((smooth::ArrayEntry<s> *)	0)
#endif

#include "array/arrayentry.h"
#include "array/arraybackend.h"
#include "array/array.h"

#endif
