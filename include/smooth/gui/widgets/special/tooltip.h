 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_TOOLTIP
#define H_OBJSMOOTH_TOOLTIP

namespace smooth
{
	namespace GUI
	{
		class Tooltip;
		class ToolWindow;
	};

	namespace System
	{
		class Timer;
	};
};

#include "../widget.h"

namespace smooth
{
	namespace GUI
	{
		class SMOOTHAPI Tooltip : public Widget
		{
			private:
				ToolWindow		*toolWindow;
				System::Timer		*timer;
			protected:
				Int			 timeOut;

				Layer			*layer;
			public:
				static const Short	 classID;

							 Tooltip();
				virtual			~Tooltip();

				virtual Int		 Show();
				virtual Int		 Hide();
			accessors:
				Void			 SetTimeout(Int nTimeOut)	{ timeOut = nTimeOut; }

				Void			 SetLayer(Layer *nLayer)	{ layer = nLayer; }
			slots:
				Void			 OnToolWindowPaint();
				Void			 OnToolWindowEvent(Int, Int, Int);

				Void			 OnTimer();
		};
	};
};

#endif
