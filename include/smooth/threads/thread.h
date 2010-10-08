 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_THREAD
#define H_OBJSMOOTH_THREAD

namespace smooth
{
	namespace Threads
	{
		class Thread;
		class ThreadBackend;
	};
};

#include "../basic/object.h"
#include "../templates/callbacks.h"

namespace smooth
{
	namespace Threads
	{
		const Short	 THREAD_CREATED		= 0;
		const Short	 THREAD_RUNNING		= 1;
		const Short	 THREAD_STOPPED		= 2;
		const Short	 THREAD_STARTME		= 3;
		const Short	 THREAD_STOPPED_SELF	= 4;

		const Short	 THREAD_KILLFLAG_KILL	= 0;
		const Short	 THREAD_KILLFLAG_WAIT	= 1;

		const Short	 THREAD_WAITFLAG_WAIT	= 0;
		const Short	 THREAD_WAITFLAG_START	= 2;

		class SMOOTHAPI Thread : public Object
		{
			private:
				Short			 status;

				ThreadBackend		*backend;

				static Short		 nOfRunningThreads;

				static Void		 MainCaller(Thread *);
			public:
				static const Short	 classID;

							 Thread(Void * = NIL);
							 Thread(const Thread &);
				virtual			~Thread();

				Thread &operator	 =(const Thread &);

				Short			 GetStatus() const;
				Int			 GetThreadID() const;

				Int			 Start();
				Int			 Stop();

				inline static Short	 GetNOfRunningThreads() { return nOfRunningThreads; }
			callbacks:
				Callback1<Int, Thread *> threadMain;
		};
	};
};

#endif
