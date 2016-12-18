 /* The smooth Class Library
  * Copyright (C) 1998-2016 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_EVENTPROCESSOR
#define H_OBJSMOOTH_EVENTPROCESSOR

namespace smooth
{
	namespace System
	{
		class EventProcessor;
		class EventBackend;
	};
};

#include "../definitions.h"
#include "../templates/callbacks.h"

namespace smooth
{
	namespace System
	{
		class SMOOTHAPI EventProcessor
		{
			private:
				EventBackend		*backend;
			public:
							 EventProcessor();
				virtual			~EventProcessor();

				Int			 ProcessNextEvent();
			callbacks:
				static Callback0<Int>	 allowTimerInterrupts;
				static Callback0<Int>	 denyTimerInterrupts;
		};
	};
};

#endif
