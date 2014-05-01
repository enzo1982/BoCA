 /* The smooth Class Library
  * Copyright (C) 1998-2014 Robert Kausch <robert.kausch@gmx.net>
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
		class SMOOTHAPI Color
		{
			public:
				enum ColorSpace
				{
					RGB	= 0,	// Red, Green, Blue
					RGBA	= 1,	// Red, Green, Blue, Alpha
					HSV	= 2,	// Hue, Saturation, Value
					YUV	= 3,	// Value, Blue, Red
					CMY	= 4,	// Cyan, Magenta, Yellow (subtractive)
					CMYK	= 5,	// Cyan, Magenta, Yellow, Black
					GRAY	= 6	// Black
				};

			protected:
				ColorSpace	 colorSpace;
				Long		 color;
			public:
						 Color()						{ color = 0; colorSpace = RGB; }
						 Color(const Color &iColor)				{ color = iColor.color; colorSpace = iColor.colorSpace; }
						 Color(Long iColor, ColorSpace iColorSpace = RGB)	{ color = iColor; colorSpace = iColorSpace; }
						 Color(Int r, Int g, Int b, ColorSpace c = RGB)		{ SetColor(r, g, b, c); }

				Int		 GetRed() const						{ return  color	       & 255; }
				Int		 GetGreen() const					{ return (color >>  8) & 255; }
				Int		 GetBlue() const					{ return (color >> 16) & 255; }
				Int		 GetAlpha() const					{ return (color >> 24) & 255; }

				ColorSpace	 GetColorSpace() const					{ return colorSpace; }

				Void		 SetColor(Long nColor, ColorSpace nColorSpace = RGB)	{ color = nColor; colorSpace = nColorSpace; }
				Void		 SetColor(Int r, Int g, Int b, ColorSpace c = RGB)	{ color = b << 16 | g << 8 | r; colorSpace = c; }

				Color		 ConvertTo(ColorSpace) const;
				Color		 Grayscale() const					{ return Color(ConvertTo(GRAY), ConvertTo(GRAY), ConvertTo(GRAY)); }

				Color		 Average(const Color &color2) const			{ return Color( (GetRed()   + color2.GetRed())	 / 2	    |
														       ((GetGreen() + color2.GetGreen()) / 2) <<  8 |
														       ((GetBlue()  + color2.GetBlue())	 / 2) << 16 |
														       ((GetAlpha() + color2.GetAlpha()) / 2) << 24); }

				Color		 Downsample(Int) const;
				Color		 Upsample(Int) const;

				Color &operator	 =(const Long nColor)					{ SetColor(nColor); return *this; }
				operator	 Long() const						{ return color; }
		};
	};
};

#endif
