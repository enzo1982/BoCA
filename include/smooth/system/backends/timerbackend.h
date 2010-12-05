 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_TIMERBACKEND
#define H_OBJSMOOTH_TIMERBACKEND

namespace smooth
{
	namespace System
	{
		class Timer;
		class TimerBackend;
	};
};

#include "../../definitions.h"

namespace smooth
{
	namespace System
	{
		const Short	 TIMER_NONE = 0;

		class TimerBackend
		{
			private:
				static TimerBackend	*(*backend_creator)(Timer *);
			protected:
				Short			 type;

				Timer			*thisTimer;
			public:
				static Int		 SetBackend(TimerBackend *(*)(Timer *));
				static TimerBackend	*CreateBackendInstance(Timer *);

							 TimerBackend(Timer *);
				virtual			~TimerBackend();

				Short			 GetTimerType() const;

				virtual Int		 Start(Int);
				virtual Int		 Stop();

				virtual Int		 GetID() const;
		};
	};
};

#endif
