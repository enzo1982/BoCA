 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_THREADS_RWLOCK
#define H_OBJSMOOTH_THREADS_RWLOCK

namespace smooth
{
	namespace Threads
	{
		class Mutex;
		class Semaphore;
		class RWLock;
	};
};

#include "../definitions.h"

namespace smooth
{
	namespace Threads
	{
		class SMOOTHAPI RWLock
		{
			private:
				static const Short	 maxReadLocks;

				Bool			 writeLocked;

				Mutex			*exclusiveAccessMutex;
				Semaphore		*sharedAccessSemaphore;
			public:
							 RWLock();
							 RWLock(const RWLock &);
							~RWLock();

				RWLock &operator	 =(const RWLock &);

				Int			 LockForRead();
				Int			 LockForWrite();

				Int			 Release();
		};
	};
};

#endif
