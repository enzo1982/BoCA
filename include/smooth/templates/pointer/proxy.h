 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_POINTER_PROXY_
#define _H_OBJSMOOTH_POINTER_PROXY_

#include "../../definitions.h"

namespace smooth
{
	template <class t> class PointerProxy
	{
		private:
			t	*object;
		public:
				 PointerProxy(t *iObject)	{ object = iObject; object->objMutex->Lock(); }
				~PointerProxy()			{ object->objMutex->Release(); }

			t	*operator ->() const		{ return object; }
	};
};

#endif
