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

namespace smooth
{
	SMOOTHAPI Void	 foreach_initIndex(const String &);
	SMOOTHAPI Int	 foreach_indexValue(const String &);
	SMOOTHAPI Void	 foreach_incIndex(const String &);
}

#define foreach(type, var, array)					\
        smooth::foreach_initIndex(#var);				\
        for (type var;							\
	     var = array.GetNth(smooth::foreach_indexValue(#var)),	\
	     smooth::foreach_indexValue(#var) < array.Length();		\
	     smooth::foreach_incIndex(#var))

#endif
