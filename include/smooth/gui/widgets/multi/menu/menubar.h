 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_MENUBAR
#define H_OBJSMOOTH_MENUBAR

namespace smooth
{
	namespace GUI
	{
		class Menubar;
	};
};

#include "menu.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 MB_GRAYSCALE	= 0;
		const Short	 MB_COLOR	= 1;
		const Short	 MB_POPUPOPEN	= 2;

		class SMOOTHAPI Menubar : public Menu
		{
			public:
				static const Short	 classID;

							 Menubar();
				virtual			~Menubar();

				virtual Int		 Paint(Int);

				MenuEntry		*AddEntry(const String & = NIL, const Bitmap & = NIL, PopupMenu * = NIL, Bool * = NIL, Int * = NIL, Int iCode = 0);
		};
	};
};

#endif
