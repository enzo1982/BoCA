 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_WINDOWXLIB
#define H_OBJSMOOTH_WINDOWXLIB

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
		const Short	 WINDOW_XLIB	= 2;

		class WindowXLib : public WindowBackend
		{
			private:
				static Array<WindowXLib *, Void *>	 windowBackends;

				X11::Display				*display;
				X11::XIM				 im;
			protected:
				Int					 id;

				X11::Window				 wnd;
				X11::Window				 oldwnd;

				X11::XIC				 ic;

				Point					 pos;
				Size					 size;

				Size					 minSize;
				Size					 maxSize;

				Int					 flags;

				String					 selection;

				Void					 UpdateWMNormalHints();
			public:
									 WindowXLib(Void * = NIL);
									~WindowXLib();

				Void					*GetSystemWindow() const;

				static WindowXLib			*GetWindowBackend(X11::Window);

				Int					 ProcessSystemMessages(X11::XEvent *);

				Int					 Open(const String &, const Point &, const Size &, Int);
				Int					 Close();

				Int					 SetTitle(const String &);

				Int					 SetMinimumSize(const Size &);
				Int					 SetMaximumSize(const Size &);

				Int					 Show();
				Int					 Hide();

				Int					 SetMetrics(const Point &, const Size &);
			accessors:
				Void					 SetSelection(const String &nSelection) { selection = nSelection; }
		};
	};
};

#endif
