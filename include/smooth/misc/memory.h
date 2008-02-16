 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_MEMORY_
#define _H_OBJSMOOTH_MEMORY_

namespace smooth
{
	class Memory;
};

#include "../definitions.h"

namespace smooth
{
	class SMOOTHAPI Memory
	{
		private:
			UnsignedByte	*memory;
		public:
					 Memory(Int);
					~Memory();

			Bool		 Resize(Int);

			operator	 void *() const;
	};
};

#endif
