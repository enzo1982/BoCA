 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_TIMERBACKEND_
#define _H_OBJSMOOTH_TIMERBACKEND_

namespace smooth
{
	namespace System
	{
		class TimerBackend;
	};
};

#include "../../definitions.h"

namespace smooth
{
	namespace System
	{
		const Int	 TIMER_NONE = 0;

		class TimerBackend
		{
			private:
				static TimerBackend	*(*backend_creator)();
			protected:
				Int			 type;
			public:
				static Int		 SetBackend(TimerBackend *(*)());
				static TimerBackend	*CreateBackendInstance();

							 TimerBackend();
				virtual			~TimerBackend();

				Int			 GetTimerType() const;

				virtual Int		 Start(Int);
				virtual Int		 Stop();

				virtual Int		 GetID() const;
		};
	};
};

#endif
