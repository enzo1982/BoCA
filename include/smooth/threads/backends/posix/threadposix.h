 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_THREADPOSIX
#define H_OBJSMOOTH_THREADPOSIX

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
		const Short	 THREAD_POSIX	= 1;

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

				Bool		 IsCurrentThread() const;

				Int		 Start(Void (*)(Void *), Void *);
				Int		 Stop();

				Void		 Exit();
		};
	};
};

#endif
