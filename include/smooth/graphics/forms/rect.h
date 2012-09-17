 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_RECT
#define H_OBJSMOOTH_RECT

namespace smooth
{
	namespace GUI
	{
		class Rect;
	};
};

#include "point.h"
#include "size.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Rect
		{
			constants:
				static Int	 Outlined;
				static Int	 Filled;
				static Int	 Rounded;
				static Int	 Inverted;
				static Int	 Dotted;
			public:
				Int		 left;
				Int		 top;
				Int		 right;
				Int		 bottom;

						 Rect()						{ left = 0;	 top = 0;      right = 0;		bottom = 0;		 }
						 Rect(const Point &iPos, const Size &iSize)	{ left = iPos.x; top = iPos.y; right = left + iSize.cx; bottom = top + iSize.cy; }

				Rect operator	 +(const Point &point) const			{ return Rect(Point(left, top) + point, Size(right - left, bottom - top)); }
				Rect operator	 -(const Point &point) const			{ return Rect(Point(left, top) - point, Size(right - left, bottom - top)); }

				Rect operator	 +(const Size &size) const			{ return Rect(Point(left, top), Size(right - left, bottom - top) + size); }
				Rect operator	 -(const Size &size) const			{ return Rect(Point(left, top), Size(right - left, bottom - top) - size); }

				Rect operator	 *(const Float factor) const			{ return Rect(Point(left, top) * factor,  Size(right - left, bottom - top) * factor);  }
				Rect operator	 /(const Float divisor) const			{ return Rect(Point(left, top) / divisor, Size(right - left, bottom - top) / divisor); }

				Bool operator	 ==(const Rect &rect) const			{ return (left == rect.left && top == rect.top && right == rect.right && bottom == rect.bottom); }
				Bool operator	 !=(const Rect &rect) const			{ return (left != rect.left || top != rect.top || right != rect.right || bottom != rect.bottom); }

				static Bool	 DoRectsOverlap(const Rect &, const Rect &);
				static Rect	 OverlapRect(const Rect &, const Rect &);
			accessors:
				Point		 GetPosition() const				{ return Point(left, top); }
				Size		 GetSize() const				{ return Size(right - left, bottom - top); }

				Int		 GetWidth() const				{ return right - left; }
				Int		 GetHeight() const				{ return bottom - top; }
		};
	};
};

#endif
