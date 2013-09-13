 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_ACTIVEAREA
#define H_OBJSMOOTH_ACTIVEAREA

namespace smooth
{
	namespace GUI
	{
		class ActiveArea;
		class Hotspot;
	};
};

#include "../widget.h"
#include "../../../graphics/color.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI ActiveArea : public Widget
		{
			protected:
				Color			 areaColor;

				Hotspot			*hotspot;
			public:
				static const Short	 classID;

							 ActiveArea(const Color &, const Point &, const Size &);
				virtual			~ActiveArea();

				virtual Int		 Paint(Int);
			accessors:
				Int			 SetColor(const Color &);
				const Color		&GetColor() const		{ return areaColor; }
			slots:
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
