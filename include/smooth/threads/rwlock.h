 /* The smooth Class Library
  * Copyright (C) 1998-2017 Robert Kausch <robert.kausch@gmx.net>
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
				volatile Int		 readLocked;
				volatile Int		 writeLocked;

				Mutex			*exclusiveAccessMutex;
				Mutex			*sharedAccessMutex;
			public:
							 RWLock();
							 RWLock(const RWLock &);
							~RWLock();

				RWLock &operator	 =(const RWLock &);

				Bool			 LockForRead();
				Bool			 LockForWrite();

				Bool			 Release();
		};

		/* Simple scoped lockers for read/write locks.
		 */
		class SMOOTHAPI LockForRead
		{
			private:
				RWLock	&rwlock;
			public:
					 LockForRead(RWLock &l) : rwlock(l)	{ rwlock.LockForRead(); }
					~LockForRead()				{ rwlock.Release(); }
		};

		class SMOOTHAPI LockForWrite
		{
			private:
				RWLock	&rwlock;
			public:
					 LockForWrite(RWLock &l) : rwlock(l)	{ rwlock.LockForWrite(); }
					~LockForWrite()				{ rwlock.Release(); }
		};
	};
};

#endif
