 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_RIGHTTOLEFT_
#define _H_OBJSMOOTH_RIGHTTOLEFT_

#include "../../definitions.h"
#include "../forms/rect.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI RightToLeftModifier
		{
			private:
				Size		 surfaceSize;

				Bool		 rightToLeft;
			public:
						 RightToLeftModifier();
				virtual		~RightToLeftModifier();

				Void		 SetSurfaceSize(const Size &);
				const Size	&GetSurfaceSize() const;

				Void		 SetRightToLeft(Bool);
				Bool		 GetRightToLeft() const;

				Int		 TranslateX(Int) const;
				Int		 TranslateY(Int) const;
				Point		 TranslatePoint(const Point &) const;
				Rect		 TranslateRect(const Rect &) const;
		};
	};
};

#endif
