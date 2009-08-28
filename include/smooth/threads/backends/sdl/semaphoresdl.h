 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SEMAPHORESDL
#define H_OBJSMOOTH_SEMAPHORESDL

namespace smooth
{
	namespace Threads
	{
		class SemaphoreSDL;
	};
};

#include "../semaphorebackend.h"

#include <SDL/SDL.h>
#include <SDL/SDL_thread.h>

namespace smooth
{
	namespace Threads
	{
		const Int	 SEMAPHORE_SDL	= 3;

		class SemaphoreSDL : public SemaphoreBackend
		{
			protected:
				SDL_sem	*semaphore;
				Int	 initialValue;
				Bool	 mySemaphore;
			public:
					 SemaphoreSDL(Int, Void * = NIL);
					~SemaphoreSDL();

				Void	*GetSystemSemaphore() const;

				Int	 Wait();
				Int	 Release();
		};
	};
};

#endif
