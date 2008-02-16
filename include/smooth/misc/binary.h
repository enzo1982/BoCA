 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_BINARY_
#define _H_OBJSMOOTH_BINARY_

namespace smooth
{
	class Binary;
};

#include "../definitions.h"

namespace smooth
{
	class SMOOTHAPI Binary
	{
		private:
					 Binary();
					 Binary(const Binary &);
		public:
			static Bool	 GetBit(Int, UnsignedInt);
			static Int	 SetBit(Int &, UnsignedInt, Bool);

			static Int	 GetBits(Int, UnsignedInt, UnsignedInt);
			static Int	 SetBits(Int &, UnsignedInt, UnsignedInt, Int);

			static Int	 And(Int, Int);
			static Int	 Or(Int, Int);
			static Int	 Xor(Int, Int);
			static Int	 Not(Int);

			static Int	 ShiftL(Int, Int);
			static Int	 ShiftR(Int, Int);

			static Bool	 IsFlagSet(Int, Int);
			static Int	 SetFlag(Int &, Int);
	};
};

#endif
