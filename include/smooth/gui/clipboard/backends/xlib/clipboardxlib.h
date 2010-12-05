 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CLIPBOARDXLIB
#define H_OBJSMOOTH_CLIPBOARDXLIB

namespace smooth
{
	namespace GUI
	{
		class Window;

		class ClipboardXLib;
	};
};

#include "../clipboardbackend.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 CLIPBOARD_XLIB	= 2;

		class ClipboardXLib : public ClipboardBackend
		{
			private:
				Window		*window;

				unsigned char	*QueryAtom(X11::Display *, X11::Window, X11::Atom) const;
			public:
						 ClipboardXLib(Window *);
						~ClipboardXLib();

				String		 GetClipboardText() const;
				Bool		 SetClipboardText(const String &);
		};
	};
};

#endif
