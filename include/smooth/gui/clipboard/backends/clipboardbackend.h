 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_CLIPBOARDBACKEND
#define H_OBJSMOOTH_CLIPBOARDBACKEND

namespace smooth
{
	namespace GUI
	{
		class Window;

		class ClipboardBackend;
	};
};

#include "../../../definitions.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 CLIPBOARD_NONE = 0;

		class ClipboardBackend
		{
			private:
				static ClipboardBackend	*(*backend_creator)(Window *);
			protected:
				Short			 type;
			public:
				static Int		 SetBackend(ClipboardBackend *(*)(Window *));

				static ClipboardBackend	*CreateBackendInstance(Window *);

							 ClipboardBackend();
				virtual			~ClipboardBackend();

				Short			 GetClipboardType();

				virtual String		 GetClipboardText() const;
				virtual Bool		 SetClipboardText(const String &);
		};
	};
};

#endif
