 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_MENUENTRY
#define H_OBJSMOOTH_MENUENTRY

namespace smooth
{
	namespace GUI
	{
		class MenuEntry;
		class Menu;

		class PopupMenu;
		class Shortcut;
		class Window;
	};
};

#include "../../widget.h"
#include "../../../../graphics/bitmap.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI MenuEntry : public Widget
		{
			protected:
				PopupMenu		*popup;

				Bitmap			 bitmap;
				Bitmap			 graymap;

				Shortcut		*shortcut;

				Bool			*bVar;
				Int			*iVar;
				Int			 iCode;
			public:
				static const Short	 classID;

							 MenuEntry(const String & = NIL, const Bitmap & = NIL, PopupMenu * = NIL, Bool * = NIL, Int * = NIL, Int = 0);
				virtual			~MenuEntry();
			accessors:
				Int			 SetShortcut(Int, Int, Window *);

				Int			 SetBitmap(const Bitmap &);
				const Bitmap		&GetBitmap() const;
			slots:
				Void			 InternalOnDeletePopup();
		};
	};
};

#endif
