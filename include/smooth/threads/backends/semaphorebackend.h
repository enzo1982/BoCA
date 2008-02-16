 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_SEMAPHOREBACKEND_
#define _H_OBJSMOOTH_SEMAPHOREBACKEND_

namespace smooth
{
	namespace Threads
	{
		class SemaphoreBackend;
	};
};

#include "../../definitions.h"

namespace smooth
{
	namespace Threads
	{
		const Int	 SEMAPHORE_NONE = 0;

		class SemaphoreBackend
		{
			private:
				static SemaphoreBackend		*(*backend_creator)(Int, Void *);
			protected:
				Int				 type;
			public:
				static Int			 SetBackend(SemaphoreBackend *(*)(Int, Void *));

				static SemaphoreBackend		*CreateBackendInstance(Int, Void * = NIL);

								 SemaphoreBackend(Int, Void * = NIL);
				virtual				~SemaphoreBackend();

				Int				 GetSemaphoreType() const;

				virtual Void			*GetSystemSemaphore() const;

				virtual Int			 Wait();
				virtual Int			 Release();
		};
	};
};

#endif
