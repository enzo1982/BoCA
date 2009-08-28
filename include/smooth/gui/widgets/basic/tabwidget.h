 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_TABWIDGET
#define H_OBJSMOOTH_TABWIDGET

namespace smooth
{
	namespace GUI
	{
		class TabWidget;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI TabWidget : public Widget
		{
			public:
				static const Int	 classID;

							 TabWidget(const Point &, const Size &);
				virtual			~TabWidget();

				virtual Int		 Paint(Int);
				virtual Int		 Process(Int, Int, Int);

				virtual Int		 Add(Widget *);
				virtual Int		 Remove(Widget *);

				Int			 SelectTab(Int);
			slots:
				Void			 OnChangeSize(const Size &);
		};
	};
};

#endif
