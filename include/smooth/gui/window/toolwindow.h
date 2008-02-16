 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_TOOLWINDOW_
#define _H_OBJSMOOTH_TOOLWINDOW_

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
				static const Int	 classID;

							 ToolWindow(const Point &, const Size &);
				virtual			~ToolWindow();

				virtual Int		 SetMetrics(const Point &, const Size &);

				virtual Bool		 IsRightToLeft() const;
				virtual Bool		 IsTypeCompatible(Int) const;
		};
	};
};

#endif
