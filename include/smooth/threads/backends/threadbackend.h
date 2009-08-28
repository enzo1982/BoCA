 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_THREADBACKEND
#define H_OBJSMOOTH_THREADBACKEND

namespace smooth
{
	namespace Threads
	{
		class ThreadBackend;
	};
};

#include "../../definitions.h"

namespace smooth
{
	namespace Threads
	{
		const Int	 THREAD_NONE = 0;

		class ThreadBackend
		{
			private:
				static ThreadBackend	*(*backend_creator)(Void *);
			protected:
				Int			 type;
			public:
				static Int		 SetBackend(ThreadBackend *(*)(Void *));

				static ThreadBackend	*CreateBackendInstance(Void * = NIL);

							 ThreadBackend(Void * = NIL);
				virtual			~ThreadBackend();

				Int			 GetThreadType() const;

				virtual Void		*GetSystemThread() const;

				virtual Int		 GetThreadID() const;

				virtual Int		 Start(Void (*)(Void *), Void *);
				virtual Int		 Stop();

				virtual Void		 Exit();
				virtual Void		*Self() const;
		};
	};
};

#endif
