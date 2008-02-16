 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_TIMERSDL_
#define _H_OBJSMOOTH_TIMERSDL_

namespace smooth
{
	namespace System
	{
		class TimerSDL;
	};
};

#include "../timerbackend.h"

namespace smooth
{
	namespace System
	{
		const Int	 TIMER_SDL = 2;

		class TimerSDL : public TimerBackend
		{
			private:
				Int			 timerid;

				static unsigned int	 TimerProc(unsigned int, void *);
			public:
							 TimerSDL();
							~TimerSDL();

				Int			 Start(Int);
				Int			 Stop();

				Int			 GetID() const;
		};
	};
};

#endif
