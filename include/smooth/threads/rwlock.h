 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_RWLOCK_
#define _H_OBJSMOOTH_RWLOCK_

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
				static const Int	 MAX_READ_LOCKS;

				Bool			 writeLocked;
				Int			 readLocks;

				Mutex			*readLockMutex;

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
