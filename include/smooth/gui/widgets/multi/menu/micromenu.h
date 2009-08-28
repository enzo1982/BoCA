 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_MICROMENU
#define H_OBJSMOOTH_MICROMENU

namespace smooth
{
	namespace GUI
	{
		class MicroMenu;
		class PopupMenu;

		class Hotspot;
	};
};

#include "menu.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI MicroMenu : public Menu
		{
			protected:
				Hotspot			*hotspot;

				PopupMenu		*popup;
			public:
				static const Int	 classID;

							 MicroMenu(const Point &, const Size &, Int);
				virtual			~MicroMenu();

				virtual Int		 Paint(Int);

				MenuEntry		*AddEntry(const String & = NIL, const Bitmap & = NIL, PopupMenu * = NIL, Bool * = NIL, Int * = NIL, Int iCode = 0);
				virtual Int		 RemoveEntry(MenuEntry *);

				virtual Int		 RemoveAllEntries();

				Rect			 GetVisibleArea() const;
			signals:
				Signal0<Void>		 onOpenPopupMenu;
				Signal0<Void>		 onClosePopupMenu;
			slots:
				Void			 OpenPopupMenu();
				Void			 ClosePopupMenu();
		};
	};
};

#endif
