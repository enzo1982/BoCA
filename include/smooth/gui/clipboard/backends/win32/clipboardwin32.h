 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CLIPBOARDWIN32
#define H_OBJSMOOTH_CLIPBOARDWIN32

namespace smooth
{
	namespace GUI
	{
		class Window;

		class ClipboardWin32;
	};
};

#include "../clipboardbackend.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 CLIPBOARD_WIN32	= 1;

		class ClipboardWin32 : public ClipboardBackend
		{
			private:
				Window	*window;
			public:
					 ClipboardWin32(Window *);
					~ClipboardWin32();

				String	 GetClipboardText() const;
				Bool	 SetClipboardText(const String &);
		};
	};
};

#endif
