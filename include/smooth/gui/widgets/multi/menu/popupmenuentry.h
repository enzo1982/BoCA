 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_POPUPMENU_ENTRY
#define H_OBJSMOOTH_POPUPMENU_ENTRY

namespace smooth
{
	namespace GUI
	{
		class PopupMenuEntry;

		class PopupMenu;
		class Hotspot;
	};

	namespace System
	{
		class Timer;
	};
};

#include "menuentry.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI PopupMenuEntry : public MenuEntry
		{
			private:
				UnsignedInt64		 popupMenuClosed;
			protected:
				Hotspot			*hotspot;
				PopupMenu		*owner;

				System::Timer		*timer;

				Int			 shortcutOffset;
			public:
				static const Short	 classID;

							 PopupMenuEntry(const String & = NIL, const Bitmap & = NIL, PopupMenu * = NIL);
				virtual			~PopupMenuEntry();

				virtual Int		 Paint(Int);

				virtual Int		 Show();
				virtual Int		 Hide();

				Size			 GetMinimumSize() const;

				Int			 GetShortcutTextSize() const;
				Void			 SetShortcutOffset(Int);

				virtual Bool		 IsTypeCompatible(Short) const;
			accessors:
				Void			 SetOwner(PopupMenu *nOwner)	{ owner = nOwner; }
			slots:
				virtual Void		 OnMouseOver();
				virtual Void		 OnMouseOut();

				virtual Void		 OnClickEntry();
				virtual Void		 OnChangeSize(const Size &);

				Void			 OpenPopupMenu();
				Void			 ClosePopupMenu();
		};
	};
};

#endif
