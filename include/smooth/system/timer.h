 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_TIMER_
#define _H_OBJSMOOTH_TIMER_

namespace smooth
{
	namespace System
	{
		class Timer;
		class TimerBackend;
	};
};

#include "../basic/object.h"
#include "../templates/signals.h"

namespace smooth
{
	namespace System
	{
		const Int	 TIMER_STOPPED = 0;
		const Int	 TIMER_RUNNING = 1;

		class SMOOTHAPI Timer : public Object
		{
			private:
				TimerBackend		*backend;

				Int			 status;
			public:
				static const Int	 classID;

							 Timer();
				virtual			~Timer();

				Int			 Start(Int);
				Int			 Stop();

				Int			 Restart(Int);
			accessors:
				Int			 GetID() const;

				Int			 GetStatus() const;
			signals:
				Signal0<Void>		 onInterval;
		};
	};
};

#endif
