 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_TOOLWINDOW
#define H_OBJSMOOTH_TOOLWINDOW

namespace smooth
{
	namespace GUI
	{
		class ToolWindow;
	};
};

#include "window.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI ToolWindow : public Window
		{
			protected:
				virtual Bool		 Create();
			public:
				static const Short	 classID;

							 ToolWindow(const Point &, const Size &);
				virtual			~ToolWindow();

				virtual Int		 SetMetrics(const Point &, const Size &);

				virtual Bool		 IsRightToLeft() const;
				virtual Bool		 IsTypeCompatible(Short) const;
		};
	};
};

#endif
