 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BACKENDGDIPLUS
#define H_OBJSMOOTH_BACKENDGDIPLUS

namespace smooth
{
	namespace Backends
	{
		class BackendGDIPlus;
	};
};

#include "../backend.h"

namespace smooth
{
	namespace Backends
	{
		const Short	 BACKEND_GDIPLUS = 4;

		class BackendGDIPlus : public Backend
		{
			private:
				ULONG_PTR	 gdiplusToken;
			public:
						 BackendGDIPlus();
				virtual		~BackendGDIPlus();

				Int		 Init();
				Int		 Deinit();
		};
	};
};

#endif
