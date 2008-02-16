 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_MUTEXWIN32_
#define _H_OBJSMOOTH_MUTEXWIN32_

namespace smooth
{
	namespace Threads
	{
		class MutexWin32;
	};
};

#include "../mutexbackend.h"

#include <winnt.h>

namespace smooth
{
	namespace Threads
	{
		const Int	 MUTEX_WIN32	= 2;

		class MutexWin32 : public MutexBackend
		{
			protected:
				HANDLE	 mutex;
				Bool	 myMutex;
			public:
					 MutexWin32(Void * = NIL);
					~MutexWin32();

				Void	*GetSystemMutex() const;

				Int	 Lock();
				Int	 Release();
		};
	};
};

#endif
