 /* The smooth Class Library
  * Copyright (C) 1998-2008 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef _H_OBJSMOOTH_MAIN_
#define _H_OBJSMOOTH_MAIN_

#include "system/system.h"
#include "init.h"
#include "version.h"
#include "misc/string.h"
#include "gui/application/application.h"
#include "gui/dialogs/messagebox.h"

using namespace smooth;

#if defined __WIN32__ && !defined __WINE__
int WINAPI WinMain(HINSTANCE shInstance, HINSTANCE shPrevInstance, LPSTR sszCmdLine, int siCmdShow)
{
	hInstance		= shInstance;
	hPrevInstance		= shPrevInstance;
	szCmdLine		= String(sszCmdLine);
	iCmdShow		= siCmdShow;

	Init();

	if (System::System::GetAPIVersion() != (String) SMOOTH_APIVERSION)
	{
		if (IDNO == GUI::Dialogs::QuickMessage("This program might not be compatible with the smooth library\ninstalled on your system. Continue execution?", "Warning", MB_YESNO, IDI_QUESTION))
		{
			Free();

			return -1;
		}
	}

	GUI::Application::GetStartupDirectory();
	GUI::Application::GetApplicationDirectory();

	Int	 retValue = Main();

	Free();

	return retValue;
}
#else
int main(int argc, char **argv)
{
	for (Int i = 1; i < argc; i++) szCmdLine.Append(String(i > 1 ? " " : "").Append(argv[i]));

	Init();

	if (System::System::GetAPIVersion() != (String) SMOOTH_APIVERSION)
	{
		Free();

		return -1;
	}

	GUI::Application::GetStartupDirectory();
	GUI::Application::GetApplicationDirectory();

	Int	 retValue = Main();

	Free();

	return retValue;
}
#endif

#endif
