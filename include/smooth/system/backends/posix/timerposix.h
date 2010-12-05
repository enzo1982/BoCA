 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_TIMERPOSIX
#define H_OBJSMOOTH_TIMERPOSIX

namespace smooth
{
	namespace System
	{
		class TimerPOSIX;
	};
};

#include "../timerbackend.h"

#include <time.h>
#include <signal.h>

namespace smooth
{
	namespace System
	{
		const Short	 TIMER_POSIX = 2;

		class TimerPOSIX : public TimerBackend
		{
			private:
				timer_t		*timer;

				static void	 TimerProc(int, siginfo_t *, void *);
			public:
						 TimerPOSIX(Timer *);
						~TimerPOSIX();

				Int		 Start(Int);
				Int		 Stop();

				Int		 GetID() const;
		};
	};
};

#endif
