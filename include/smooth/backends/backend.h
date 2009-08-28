 /* The smooth Class Library
  * Copyright (C) 1998-2009 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BACKEND
#define H_OBJSMOOTH_BACKEND

namespace smooth
{
	namespace Backends
	{
		class Backend;
	};
};

#include "../definitions.h"

namespace smooth
{
	namespace Backends
	{
		const Int	 BACKEND_NONE = 0;

		class Backend
		{
			private:
				static Array<Backend *, Void *>		*backends;
				static Array<Backend *(*)(), Void *>	*backend_creators;
			protected:
				Int					 type;
			public:
				static Int				 AddBackend(Backend *(*)());

				static Int				 GetNOfBackends();
				static Backend				*GetNthBackend(Int);

				static Int				 InitBackends();
				static Int				 DeinitBackends();

									 Backend();
				virtual					~Backend();

				virtual Int				 Init();
				virtual Int				 Deinit();

				Int					 GetBackendType() const;
		};
	};
};

#endif
