 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_POPUPMENU
#define H_OBJSMOOTH_POPUPMENU

namespace smooth
{
	namespace GUI
	{
		class PopupMenu;
		class ToolWindow;
	};
};

#include "menu.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI PopupMenu : public Menu
		{
			private:
				ToolWindow			*toolWindow;

				Bool				 closedByClick;
			public:
				static const Short		 classID;

								 PopupMenu();
				virtual				~PopupMenu();

				virtual Int			 Show();
				virtual Int			 Hide();

				MenuEntry			*AddEntry(const String & = NIL, const Bitmap & = NIL, PopupMenu * = NIL, Bool * = NIL, Int * = NIL, Int iCode = 0);

				Void				 CalculateSize();
			accessors:
				Bool				 IsClosedByClick() const		{ return closedByClick; }
				Void				 SetClosedByClick(Bool nClosedByClick)	{ closedByClick = nClosedByClick; }
			signals:
				static Signal1<Void, Int>	 internalOnOpenPopupMenu;

				Signal0<Void>			 internalRequestClose;
			slots:
				Void				 OnOpenPopupMenu(Int);
				Void				 OnToolWindowPaint();
		};
	};
};

#endif
