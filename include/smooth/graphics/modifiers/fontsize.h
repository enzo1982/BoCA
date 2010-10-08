 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_FONTSIZE
#define H_OBJSMOOTH_FONTSIZE

#include "../../definitions.h"
#include "../forms/rect.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI FontSizeModifier
		{
			private:
				Short		 fontSize;
			public:
						 FontSizeModifier();
						~FontSizeModifier();

				Void		 SetFontSize(Short);
				Short		 GetFontSize() const;

				Int		 TranslateX(Int) const;
				Int		 TranslateY(Int) const;
				Point		 TranslatePoint(const Point &) const;
				Rect		 TranslateRect(const Rect &) const;
		};
	};
};

#endif
