 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_EVENTXLIB
#define H_OBJSMOOTH_EVENTXLIB

namespace smooth
{
	namespace System
	{
		class EventXLib;
	};
};

#include "../eventbackend.h"

namespace smooth
{
	namespace System
	{
		const Short	 EVENT_XLIB = 2;

		class EventXLib : public EventBackend
		{
			private:
				Display	*display;
			public:
					 EventXLib();
					~EventXLib();

				Int	 ProcessNextEvent();
		};
	};
};

#endif
