 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_MUTEXBACKEND
#define H_OBJSMOOTH_MUTEXBACKEND

namespace smooth
{
	namespace Threads
	{
		class MutexBackend;
	};
};

#include "../../definitions.h"

namespace smooth
{
	namespace Threads
	{
		const Int	 MUTEX_NONE = 0;

		class MutexBackend
		{
			private:
				static MutexBackend	*(*backend_creator)(Void *);
			protected:
				Int			 type;
			public:
				static Int		 SetBackend(MutexBackend *(*)(Void *));

				static MutexBackend	*CreateBackendInstance(Void * = NIL);

							 MutexBackend(Void * = NIL);
				virtual			~MutexBackend();

				Int			 GetMutexType() const;

				virtual Void		*GetSystemMutex() const;

				virtual Int		 Lock();
				virtual Int		 Release();
		};
	};
};

#endif
