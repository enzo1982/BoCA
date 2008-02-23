 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_FONTGDI_
#define _H_OBJSMOOTH_FONTGDI_

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
		const Int	 FONT_GDI	= 1;

		class FontGDI : public FontBackend
		{
			public:
					 FontGDI(const String &, Int, Int, Int, const Color &);
					~FontGDI();

				Size	 GetTextSize(const String &) const;
		};
	};
};

#endif
