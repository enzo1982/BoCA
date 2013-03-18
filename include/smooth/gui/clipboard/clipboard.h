 /* The smooth Class Library
  * Copyright (C) 1998-2012 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CLIPBOARD
#define H_OBJSMOOTH_CLIPBOARD

namespace smooth
{
	namespace GUI
	{
		class Window;

		class Clipboard;
		class ClipboardBackend;
	};
};

#include "../../definitions.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Clipboard
		{
			protected:
				ClipboardBackend	*backend;
			public:
							 Clipboard(Window *);
				virtual			~Clipboard();

				/* Get or set the current selection text.
				 */
				String			 GetSelectionText() const;
				Bool			 SetSelectionText(const String &);

				/* Get or set the persistent clipboard text.
				 */
				String			 GetClipboardText() const;
				Bool			 SetClipboardText(const String &);
		};
	};
};

#endif
