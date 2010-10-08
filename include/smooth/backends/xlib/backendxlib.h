 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BACKENDXLIB
#define H_OBJSMOOTH_BACKENDXLIB

namespace smooth
{
	namespace Backends
	{
		class BackendXLib;
	};
};

#include "../backend.h"

#include <X11/Xlib.h>

namespace smooth
{
	namespace Backends
	{
		const Short	 BACKEND_XLIB = 1;

		class BackendXLib : public Backend
		{
			private:
				static Display	*display;
			public:
						 BackendXLib();
				virtual		~BackendXLib();

				Int		 Init();
				Int		 Deinit();

				static Display	*GetDisplay();
		};
	};
};

#endif
