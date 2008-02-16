 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_THREADPOSIX_
#define _H_OBJSMOOTH_THREADPOSIX_

namespace smooth
{
	namespace Threads
	{
		class ThreadPOSIX;
	};
};

#include "../threadbackend.h"

#include <pthread.h>

namespace smooth
{
	namespace Threads
	{
		const Int	 THREAD_POSIX	= 1;

		class ThreadPOSIX : public ThreadBackend
		{
			protected:
				pthread_t	*thread;
				Bool		 myThread;
			public:
						 ThreadPOSIX(Void * = NIL);
						~ThreadPOSIX();

				Void		*GetSystemThread() const;

				Int		 GetThreadID() const;

				Int		 Start(Void (*)(Void *), Void *);
				Int		 Stop();

				Void		 Exit();
				Void		*Self() const;
		};
	};
};

#endif
