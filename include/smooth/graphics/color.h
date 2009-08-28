 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_COLOR
#define H_OBJSMOOTH_COLOR

namespace smooth
{
	namespace GUI
	{
		class Color;
	};
};

#include "../definitions.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 RGBA	= 0;	// Red, Green, Blue, Alpha
		const Int	 HSV	= 1;	// Hue, Saturation, Value
		const Int	 YUV	= 2;	// Value, Blue, Red
		const Int	 CMY	= 3;	// Cyan, Magenta, Yellow (subtractive)
		const Int	 CMYK	= 4;	// Cyan, Magenta, Yellow, Black
		const Int	 GRAY	= 5;	// Black

		class SMOOTHAPI Color
		{
			protected:
				Int		 colorSpace;
				Long		 color;
			public:
						 Color()					{ color = 0; colorSpace = RGBA; }
						 Color(const Color &iColor)			{ color = iColor.color; colorSpace = iColor.colorSpace; }
						 Color(Long iColor, Int iColorSpace = RGBA)	{ color = iColor; colorSpace = iColorSpace; }
						 Color(Int r, Int g, Int b, Int c = RGBA)	{ SetColor(r, g, b, c); }

				Int		 GetRed() const					{ return color & 255; }
				Int		 GetGreen() const				{ return (color >> 8) & 255; }
				Int		 GetBlue() const				{ return (color >> 16) & 255; }
				Int		 GetAlpha() const				{ return (color >> 24) & 255; }

				Void		 SetColor(Long nColor, Int nColorSpace = RGBA)	{ color = nColor; colorSpace = nColorSpace; }
				Void		 SetColor(Int r, Int g, Int b, Int c = RGBA)	{ color = r + g * 256 + b * 65536; colorSpace = c; }

				Color		 ConvertTo(Int) const;
				Color		 Grayscale() const				{ return Color(ConvertTo(GRAY), ConvertTo(GRAY), ConvertTo(GRAY)); }

				Color		 Average(const Color &color2) const		{ return Color((GetRed() + color2.GetRed()) / 2, (GetGreen() + color2.GetGreen()) / 2, (GetBlue() + color2.GetBlue()) / 2, (GetAlpha() + color2.GetAlpha()) / 2); }

				Color		 Downsample(Int) const;
				Color		 Upsample(Int) const;

				Color &operator	 =(const Long nColor)				{ SetColor(nColor); return *this; }
				operator	 Long() const					{ return color; }
		};
	};
};

#endif
