 /* The smooth Class Library
  * Copyright (C) 1998-2015 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_THREADS_ACCESS
#define H_OBJSMOOTH_THREADS_ACCESS

namespace smooth
{
	namespace Threads
	{
		class Access;
		class Mutex;
	};
};

#include "../definitions.h"

namespace smooth
{
	namespace Threads
	{
		class SMOOTHAPI Access
		{
			private:
				static Mutex	*mutex;
			public:
				static Int	 Initialize();
				static Int	 Free();

				/* Access functions for Bool.
				 */
				static Bool	 Value(volatile Bool &);

				static Bool	 Set(volatile Bool &, Bool);

				/* Access functions for Short.
				 */
				static Short	 Value(volatile Short &);

				static Short	 Increment(volatile Short &);
				static Short	 Decrement(volatile Short &);

				static Short	 Set(volatile Short &, Short);

				/* Access functions for Int.
				 */
				static Int	 Value(volatile Int &);

				static Int	 Increment(volatile Int &);
				static Int	 Decrement(volatile Int &);

				static Int	 Set(volatile Int &, Int);
		};
	};
};

#endif
