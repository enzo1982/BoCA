 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_THREADWIN32
#define H_OBJSMOOTH_THREADWIN32

namespace smooth
{
	namespace Threads
	{
		class ThreadWin32;
	};
};

#include "../threadbackend.h"

#include <winnt.h>

namespace smooth
{
	namespace Threads
	{
		const Short	 THREAD_WIN32	= 2;

		class ThreadWin32 : public ThreadBackend
		{
			protected:
				HANDLE	 thread;
				Int	 threadID;
				Bool	 myThread;
			public:
					 ThreadWin32(Void * = NIL);
					~ThreadWin32();

				Void	*GetSystemThread() const;

				Int	 GetThreadID() const;

				Int	 Start(Void (*)(Void *), Void *);
				Int	 Stop();

				Bool	 IsRunning() const;

				Void	 Exit();
				Void	*Self() const;
		};
	};
};

#endif
