 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DRAGCONTROL
#define H_OBJSMOOTH_DRAGCONTROL

namespace smooth
{
	namespace GUI
	{
		class DragControl;
		class Hotspot;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI DragControl : public Widget
		{
			private:
				Point			 startMousePos;
			protected:
				Hotspot			*dragHotspot;
			public:
				static const Int	 classID;

							 DragControl();
				virtual			~DragControl();
			slots:
				Void			 OnMouseDragStart(const Point &);
				Void			 OnMouseDrag(const Point &);
		};
	};
};

#endif
