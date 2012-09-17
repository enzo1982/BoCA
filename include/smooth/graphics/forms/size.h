 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SIZE
#define H_OBJSMOOTH_SIZE

namespace smooth
{
	namespace GUI
	{
		class Size;
	};
};

#include "../../misc/math.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Size
		{
			public:
				Int		 cx;
				Int		 cy;

						 Size()				{ cx = 0;   cy = 0;   }
						 Size(Int iCX, Int iCY)		{ cx = iCX; cy = iCY; }

				Size operator	 +(const Size &size) const	{ return Size(cx + size.cx, cy + size.cy); }
				Size operator	 -(const Size &size) const	{ return Size(cx - size.cx, cy - size.cy); }

				Size operator	 *(const Float factor) const 	{ return Size(Math::Round(cx * factor),  Math::Round(cy * factor));  }
				Size operator	 /(const Float divisor) const	{ return Size(Math::Round(cx / divisor), Math::Round(cy / divisor)); }

				Bool operator	 ==(const Size &size) const	{ return (cx == size.cx && cy == size.cy); }
				Bool operator	 !=(const Size &size) const	{ return (cx != size.cx || cy != size.cy); }
		};
	};
};

#endif
