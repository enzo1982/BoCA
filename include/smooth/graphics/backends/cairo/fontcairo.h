 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_FONTCAIRO_
#define _H_OBJSMOOTH_FONTCAIRO_

namespace smooth
{
	namespace GUI
	{
		class FontCairo;
	};
};

#include "../fontbackend.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 FONT_CAIRO	= 3;

		class FontCairo : public FontBackend
		{
			public:
					 FontCairo(const String &, Int, Int, Int, const Color &);
					~FontCairo();

				Size	 GetTextSize(const String &) const;
		};
	};
};

#endif
