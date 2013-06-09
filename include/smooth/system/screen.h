 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SCREEN
#define H_OBJSMOOTH_SCREEN

namespace smooth
{
	namespace System
	{
		class Screen;
		class ScreenBackend;
	};
};

#include "../definitions.h"
#include "../graphics/forms/rect.h"

namespace smooth
{
	namespace System
	{
		class SMOOTHAPI Screen
		{
			private:
				static ScreenBackend	*backend;

							 Screen();
							 Screen(const Screen &);
			public:
				static Int		 Initialize();
				static Int		 Free();

				static GUI::Rect	 GetActiveScreenMetrics();
				static GUI::Rect	 GetActiveScreenWorkArea();

				static GUI::Rect	 GetVirtualScreenMetrics();
		};
	};
};

#endif
