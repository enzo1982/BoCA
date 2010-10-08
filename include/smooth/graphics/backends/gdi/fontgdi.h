 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_FONTGDI
#define H_OBJSMOOTH_FONTGDI

namespace smooth
{
	namespace GUI
	{
		class FontGDI;
	};
};

#include "../fontbackend.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 FONT_GDI	= 1;

		class FontGDI : public FontBackend
		{
			public:
					 FontGDI(const String &, Short, Short, Short, const Color &);
					~FontGDI();

				Size	 GetTextSize(const String &) const;
		};
	};
};

#endif
