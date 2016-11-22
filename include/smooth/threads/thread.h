 /* The smooth Class Library
  * Copyright (C) 1998-2016 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_THREADS_THREAD
#define H_OBJSMOOTH_THREADS_THREAD

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
#include "access.h"

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

		class SMOOTHAPI Thread : public Object
		{
			private:
				Short			 status;

				ThreadBackend		*backend;

				static Short		 nOfRunningThreads;

#if defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
				static Void		 MainCaller(Thread *) __attribute__((force_align_arg_pointer));
#else
				static Void		 MainCaller(Thread *);
#endif
			public:
				static const Short	 classID;

							 Thread(Void * = NIL);
							 Thread(const Thread &);
				virtual			~Thread();

				Thread &operator	 =(const Thread &);

				Short			 GetStatus() const;
				Int			 GetThreadID() const;

				Bool			 IsCurrentThread() const;

				Int			 Start();
				Int			 Stop();

				Int			 Wait();

				inline static Short	 GetNOfRunningThreads() { return Access::Value(nOfRunningThreads); }
			callbacks:
				Callback1<Int, Thread *> threadMain;
		};
	};
};

#endif
