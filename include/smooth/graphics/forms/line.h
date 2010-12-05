 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_LINE
#define H_OBJSMOOTH_LINE

namespace smooth
{
	namespace GUI
	{
		class Line;
	};
};

#include "point.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Line
		{
			public:
				Int		 sx;
				Int		 sy;
				Int		 ex;
				Int		 ey;

						 Line()						{ sx = 0; sy = 0; ex = 0; ey = 0; }
						 Line(const Point &iStart, const Point &iEnd)	{ sx = iStart.x; sy = iStart.y; ex = iEnd.x; ey = iEnd.y; }

				Bool operator	 ==(const Line &) const;
				Bool operator	 !=(const Line &) const;
			accessors:
				Point		 GetStartPoint() const				{ return Point(sx, sy); }
				Point		 GetEndPoint() const				{ return Point(ex, ey); }
		};
	};
};

#endif
