 /* The smooth Class Library
  * Copyright (C) 1998-2016 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_WINAPPLICATION
#define H_OBJSMOOTH_WINAPPLICATION

#include "init.h"
#include "version.h"
#include "system/system.h"
#include "gui/application/application.h"
#include "gui/dialogs/messagebox.h"

#if defined __WIN32__
#	include "init.win32.h"
#endif

namespace smooth
{
	static Void	 AttachDLL(Void *);
	static Void	 DetachDLL();
};

using namespace smooth;

extern "C"
{
#if defined __WIN32__
	BOOL WINAPI DllMain(HINSTANCE shInstance, DWORD reason, LPVOID reserved)
	{
		switch (reason)
		{
			case DLL_PROCESS_ATTACH:
				if (hInstance == NIL) hInstance = shInstance;

				hPrevInstance	= NIL;

				Init();

				if (System::System::GetAPIVersion() != (String) SMOOTH_APIVERSION)
				{
					GUI::Dialogs::QuickMessage("This DLL might not be compatible with the\nsmooth library installed on your system.", "Warning", MB_OK, IDI_QUESTION);
				}

				GUI::Application::GetStartupDirectory();
				GUI::Application::GetApplicationDirectory();

				AttachDLL(shInstance);

				break;
			case DLL_PROCESS_DETACH:
				DetachDLL();

				Free();

				break;
			case DLL_THREAD_ATTACH:
				break;
			case DLL_THREAD_DETACH:
				break;
		}

		return true;
	}
#else
	static __attribute__((__constructor__)) void processAttach()
	{
		Init();

		if (System::System::GetAPIVersion() != (String) SMOOTH_APIVERSION)
		{
			Free();

			return;
		}

		GUI::Application::GetStartupDirectory();
		GUI::Application::GetApplicationDirectory();

		AttachDLL(NIL);
	}

	static __attribute__((__destructor__)) void processDetach()
	{
#ifdef __HAIKU__
		/* Do not call the cleanup functions on Haiku for now as they
		 * might crash due to issues with the process cleanup code.
		 */
#else
		DetachDLL();

		Free();
#endif
	}
#endif
}

#endif
