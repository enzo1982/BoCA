 /* The smooth Class Library
  * Copyright (C) 1998-2010 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_BACKENDWIN32
#define H_OBJSMOOTH_BACKENDWIN32

namespace smooth
{
	namespace Backends
	{
		class BackendWin32;
	};
};

#include "../backend.h"

extern HINSTANCE	 hDllInstance;

#ifndef SMOOTH_STATIC
extern "C"
{
	BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID);
}
#endif

namespace smooth
{
	Bool	 LoadIconvDLL();
	Void	 FreeIconvDLL();

	namespace Backends
	{
		const Short	 BACKEND_WIN32 = 2;

		class BackendWin32 : public Backend
		{
			public:
					 BackendWin32();
				virtual	~BackendWin32();

				Int	 Init();
				Int	 Deinit();
		};
	};
};

#endif
