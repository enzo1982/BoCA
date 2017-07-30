 /* The smooth Class Library
  * Copyright (C) 1998-2017 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_DIVIDER
#define H_OBJSMOOTH_DIVIDER

namespace smooth
{
	namespace GUI
	{
		class Divider;
	};
};

#include "../widget.h"
#include "../hotspot/hotspot.h"

namespace smooth
{
	namespace GUI
	{
		const Short	 DIV_MOVABLE	= 16384;

		class SMOOTHAPI Divider : public Widget
		{
			private:
				Bool			 dragging;

				Int			 startPos;
				Point			 startMousePos;
			protected:
				Int			 position;

				Hotspot			*dragHotspot;

				Void			 UpdateMetrics();
			public:
				static const Short	 classID;

							 Divider(Int, Int = OR_HORZ | OR_TOP);
				virtual			~Divider();

				virtual Int		 Paint(Int);
			accessors:
				Int			 SetPos(Int);
				Int			 GetPos() const		{ return position; }
			slots:
				Void			 OnMouseOver();
				Void			 OnMouseOut();

				Void			 OnMouseDragStart(const Point &);
				Void			 OnMouseDrag(const Point &);
				Void			 OnMouseDragEnd(const Point &);
			signals:
				Signal1<Void, Int>	 onDrag;
		};
	};
};

#endif
