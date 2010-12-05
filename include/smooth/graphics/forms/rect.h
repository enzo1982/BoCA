 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
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
#ifdef __WIN32__
				operator	 RECT() const;
				Rect &operator	 =(const RECT &);
#endif
				Int		 left;
				Int		 top;
				Int		 right;
				Int		 bottom;

						 Rect()						{ left = 0; top = 0; right = 0; bottom = 0; }
						 Rect(const Point &iPos, const Size &iSize)	{ left = iPos.x; top = iPos.y; right = left + iSize.cx; bottom = top + iSize.cy; }

				Rect operator	 +(const Point &) const;
				Rect operator	 -(const Point &) const;

				Rect operator	 +(const Size &) const;
				Rect operator	 -(const Size &) const;

				Rect operator	 *(const Float) const;
				Rect operator	 /(const Float) const;

				Bool operator	 ==(const Rect &) const;
				Bool operator	 !=(const Rect &) const;

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
