 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_SEMAPHOREWIN32
#define H_OBJSMOOTH_SEMAPHOREWIN32

namespace smooth
{
	namespace Threads
	{
		class SemaphoreWin32;
	};
};

#include "../semaphorebackend.h"

#include <winnt.h>

namespace smooth
{
	namespace Threads
	{
		const Short	 SEMAPHORE_WIN32	= 2;

		class SemaphoreWin32 : public SemaphoreBackend
		{
			protected:
				HANDLE	 semaphore;
				Int	 initialValue;
				Bool	 mySemaphore;
			public:
					 SemaphoreWin32(Int, Void * = NIL);
					~SemaphoreWin32();

				Void	*GetSystemSemaphore() const;

				Int	 Wait();
				Int	 Release();
		};
	};
};

#endif
