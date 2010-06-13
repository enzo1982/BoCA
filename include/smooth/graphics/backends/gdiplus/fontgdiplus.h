 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_FONTGDIPLUS
#define H_OBJSMOOTH_FONTGDIPLUS

namespace smooth
{
	namespace GUI
	{
		class FontGDIPlus;
	};
};

#include "../fontbackend.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 FONT_GDIPLUS	= 4;

		class FontGDIPlus : public FontBackend
		{
			public:
					 FontGDIPlus(const String &, Int, Int, Int, const Color &);
					~FontGDIPlus();

				Size	 GetTextSize(const String &) const;
		};
	};
};

#endif
