 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_TITLEBAR
#define H_OBJSMOOTH_TITLEBAR

namespace smooth
{
	namespace GUI
	{
		class Titlebar;
		class Hotspot;
		class Shortcut;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 TB_NONE	= 0;
		const Short	 TB_MINBUTTON	= 1;
		const Short	 TB_MAXBUTTON	= 2;
		const Short	 TB_CLOSEBUTTON	= 4;

		class SMOOTHAPI Titlebar : public Widget
		{
			private:
				Bool			 paintActive;
				Point			 startMousePos;
			protected:
				Hotspot			*minHotspot;
				Hotspot			*maxHotspot;
				Hotspot			*closeHotspot;

				Hotspot			*dragHotspot;

				Shortcut		*closeShortcut;
			public:
				static const Short	 classID;

							 Titlebar(Int = TB_MINBUTTON | TB_MAXBUTTON | TB_CLOSEBUTTON);
			 	virtual			~Titlebar();

				virtual Int		 Paint(Int);
				virtual Int		 Process(Int, Int, Int);
			slots:
				Void			 OnMouseDragStart(const Point &);
				Void			 OnMouseDrag(const Point &);

				Void			 OnMinButtonClick();
				Void			 OnMaxButtonClick();
				Void			 OnCloseButtonClick();

				Bool			 ButtonHitTest(const Point &);
				Bool			 DragHitTest(const Point &);
		};
	};
};

#endif
