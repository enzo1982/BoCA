 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_MUTEXPOSIX_
#define _H_OBJSMOOTH_MUTEXPOSIX_

namespace smooth
{
	namespace Threads
	{
		class MutexPOSIX;
	};
};

#include "../mutexbackend.h"

#include <pthread.h>

namespace smooth
{
	namespace Threads
	{
		const Int	 MUTEX_POSIX	= 1;

		class MutexPOSIX : public MutexBackend
		{
			protected:
				pthread_mutex_t	*mutex;
				Bool		 myMutex;
			public:
						 MutexPOSIX(Void * = NIL);
						~MutexPOSIX();

				Void		*GetSystemMutex() const;

				Int		 Lock();
				Int		 Release();
		};
	};
};

#endif
