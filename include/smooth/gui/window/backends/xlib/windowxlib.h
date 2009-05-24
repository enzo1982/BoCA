 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_WINDOWXLIB_
#define _H_OBJSMOOTH_WINDOWXLIB_

namespace smooth
{
	namespace GUI
	{
		class WindowXLib;
	};
};

#include "../windowbackend.h"
#include "../../../../graphics/forms/rect.h"

namespace smooth
{
	namespace GUI
	{
		const Int	 WINDOW_XLIB	= 2;

		class WindowXLib : public WindowBackend
		{
			private:
				static Array<WindowXLib *, Void *>	 windowBackends;

				Display					*display;
			protected:
				Int					 id;

				Window					 wnd;
				Window					 oldwnd;
			public:
									 WindowXLib(Void * = NIL);
									~WindowXLib();

				Void					*GetSystemWindow() const;

				static WindowXLib			*GetWindowBackend(::Window);

				Int					 ProcessSystemMessages(XEvent *);

				Int					 Open(const String &, const Point &, const Size &, Int);
				Int					 Close();

				Int					 SetTitle(const String &);

				Int					 Show();
				Int					 Hide();

				Int					 SetMetrics(const Point &, const Size &);
		};
	};
};

#endif
