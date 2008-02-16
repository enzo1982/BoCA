 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_EVENTBACKEND_
#define _H_OBJSMOOTH_EVENTBACKEND_

namespace smooth
{
	namespace System
	{
		class EventBackend;
	};
};

#include "../../definitions.h"

namespace smooth
{
	namespace System
	{
		const Int	 EVENT_NONE = 0;

		class EventBackend
		{
			private:
				static EventBackend	*(*backend_creator)();
			protected:
				Int			 type;
			public:
				static Int		 SetBackend(EventBackend *(*)());

				static EventBackend	*CreateBackendInstance();

							 EventBackend();
				virtual			~EventBackend();

				Int			 GetEventType() const;

				virtual Int		 ProcessNextEvent(Bool);
		};
	};
};

#endif
