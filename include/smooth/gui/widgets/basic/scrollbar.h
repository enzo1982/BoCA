 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_SCROLLBAR_
#define _H_OBJSMOOTH_SCROLLBAR_

namespace smooth
{
	namespace GUI
	{
		class Scrollbar;
	};
};

#include "arrows.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Scrollbar : public Arrows
		{
			private:
				Int			 mouseBias;
				Bool			 dragging;

				System::Timer		*clickTimer;
				Int			 clickTimerDirection;

				Void			 UpdateHotspotPositions();
			protected:
				UnsignedInt		 pageSize;

				Hotspot			*clickHotspot;
				Hotspot			*dragHotspot;
			public:
				static const Int	 classID;

							 Scrollbar(const Point &, const Size &, Int = OR_HORZ, Int * = NIL, Int = 0, Int = 100);
				virtual			~Scrollbar();

				virtual Int		 Paint(Int);
			accessors:
				Int			 SetPageSize(UnsignedInt);
				UnsignedInt		 GetPageSize();
			slots:
				Void			 OnMouseClick(const Point &);
				Void			 OnMouseClickTimer();

				Void			 OnMouseWheel(Int);

				Void			 OnMouseDragStart(const Point &);
				Void			 OnMouseDrag(const Point &);
				Void			 OnMouseDragEnd(const Point &);

				Void			 OnValueChange();
		};
	};
};

#endif
