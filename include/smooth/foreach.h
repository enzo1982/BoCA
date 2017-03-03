 /* The smooth Class Library
  * Copyright (C) 1998-2017 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_FOREACH
#define H_OBJSMOOTH_FOREACH

#include "definitions.h"

/* FIXME: The array argument is evaluated multiple times which
 *	  prevents us from passing rvalues, yet.
 *
 *	  Try using iterators in the future to solve this problem.
 */
#define foreach(var, array)												      \
	if (S::False) { }												      \
	else	      for (S::Int _break_ = 0, foreachindex = -1; !_break_ && foreachindex < (array).Length() - 1; --_break_) \
		      for (var = (array).GetNthReference(++foreachindex); !_break_; ++_break_)

#define foreachreverse(var, array)											      \
	if (S::False) { }												      \
	else	      for (S::Int _break_ = 0, foreachindex = (array).Length(); !_break_ && foreachindex > 0; --_break_)      \
		      for (var = (array).GetNthReference(--foreachindex); !_break_; ++_break_)

/* The forever macro is quite simple...
 */
#define forever for (;;)

#endif
