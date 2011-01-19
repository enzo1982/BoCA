 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_POINT
#define H_OBJSMOOTH_POINT

namespace smooth
{
	namespace GUI
	{
		class Point;
	};
};

#include "../../definitions.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Point
		{
			public:
				Int		 x;
				Int		 y;

						 Point()		{ x = 0; y = 0; }
						 Point(Int iX, Int iY)	{ x = iX; y = iY; }

				Point operator	 +(const Point &) const;
				Point operator	 -(const Point &) const;

				Point operator	 *(const Float) const;
				Point operator	 /(const Float) const;

				Point &operator	 +=(const Point &);
				Point &operator	 -=(const Point &);

				Point &operator	 *=(const Float);
				Point &operator	 /=(const Float);

				Bool operator	 ==(const Point &) const;
				Bool operator	 !=(const Point &) const;
		};
	};
};

#endif
