 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_MULTIMONITOR_
#define _H_OBJSMOOTH_MULTIMONITOR_

namespace smooth
{
	namespace System
	{
		class MultiMonitor;
	};
};

#include "../definitions.h"
#include "../graphics/forms/rect.h"

namespace smooth
{
	namespace System
	{
		class SMOOTHAPI MultiMonitor
		{
			private:
							 MultiMonitor();
							 MultiMonitor(const MultiMonitor &);
			public:
				static Void		 Initialize();
				static Void		 Free();

				static GUI::Rect	 GetActiveMonitorMetrics();
				static GUI::Rect	 GetActiveMonitorWorkArea();

				static GUI::Rect	 GetVirtualScreenMetrics();
		};
	};
};

#endif
