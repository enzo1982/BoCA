 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SIGNAL
#define H_OBJSMOOTH_SIGNAL

#include "../../basic/object.h"

namespace smooth
{
	class Signal
	{
		protected:
			Object	*parent;

			Void ProtectParent() const
			{
				if (parent != NIL) parent->EnterProtectedRegion();
			}

			Void UnprotectParent() const
			{
				if (parent != NIL) parent->LeaveProtectedRegion();
			}
		public:
			Signal()
			{
				parent = NIL;
			}

			virtual ~Signal()
			{
			}

			Int SetParentObject(Object *newParent)
			{
				parent = newParent;

				return Success();
			}
	};
};

#endif
