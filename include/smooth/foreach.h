 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_FOREACH_
#define _H_OBJSMOOTH_FOREACH_

#include "definitions.h"

/* FIXME: The array argument is evaluated multiple times which
 *         prevents us from passing rvalues, yet.
 *
 *         Try using iterators in the future to solve this problem.
 */
#define foreach(var, array)										\
        if (Int _index_ = 0)    { }									\
        else                    for (Int _break_ = 0; !_break_ && _index_ < array.Length(); --_break_)	\
				for (var = array.GetNth(_index_++); !_break_; ++_break_)

/* The forever macro is quite simple...
 */
#define forever for (;;)

#endif
