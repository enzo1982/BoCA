 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SEMAPHOREPOSIX
#define H_OBJSMOOTH_SEMAPHOREPOSIX

namespace smooth
{
	namespace Threads
	{
		class SemaphorePOSIX;
	};
};

#include "../semaphorebackend.h"

#include <semaphore.h>

namespace smooth
{
	namespace Threads
	{
		const Short	 SEMAPHORE_POSIX	= 1;

		class SemaphorePOSIX : public SemaphoreBackend
		{
			protected:
				sem_t	*semaphore;
				Int	 initialValue;
				Bool	 mySemaphore;
			public:
					 SemaphorePOSIX(Int, Void * = NIL);
					~SemaphorePOSIX();

				Void	*GetSystemSemaphore() const;

				Int	 Wait();
				Int	 Release();
		};
	};
};

#endif
