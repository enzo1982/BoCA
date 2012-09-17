 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
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

#include "../../misc/math.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Point
		{
			public:
				Int		 x;
				Int		 y;

						 Point()			{ x = 0;  y = 0;  }
						 Point(Int iX, Int iY)		{ x = iX; y = iY; }

				Point operator	 +(const Point &point) const	{ return Point(x + point.x, y + point.y); }
				Point operator	 -(const Point &point) const	{ return Point(x - point.x, y - point.y); };

				Point operator	 *(const Float factor) const	{ return Point(Math::Round(x * factor),	 Math::Round(y * factor));  }
				Point operator	 /(const Float divisor) const	{ return Point(Math::Round(x / divisor), Math::Round(y / divisor)); }

				Point &operator	 +=(const Point &point)		{ x += point.x; y += point.y; return *this; }
				Point &operator	 -=(const Point &point)		{ x -= point.x; y -= point.y; return *this; }

				Point &operator	 *=(const Float factor)		{ x = Math::Round(x * factor);	y = Math::Round(y * factor);  return *this; }
				Point &operator	 /=(const Float divisor)	{ x = Math::Round(x * divisor); y = Math::Round(y * divisor); return *this; }

				Bool operator	 ==(const Point &point) const	{ return (x == point.x && y == point.y); }
				Bool operator	 !=(const Point &point) const	{ return (x != point.x || y != point.y); }
		};
	};
};

#endif
