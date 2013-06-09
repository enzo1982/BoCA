 /* The smooth Class Library
  * Copyright (C) 1998-2013 Robert Kausch <robert.kausch@gmx.net>
  *
  * This library is free software; you can redistribute it and/or
  * modify it under the terms of "The Artistic License, Version 2.0".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#ifndef H_OBJSMOOTH_MAIN
#define H_OBJSMOOTH_MAIN

#include "system/system.h"
#include "init.h"
#include "version.h"
#include "misc/args.h"
#include "misc/string.h"
#include "gui/application/application.h"
#include "gui/dialogs/messagebox.h"

#if defined __WIN32__
#	include "init.win32.h"
#endif

namespace smooth
{
	static Int	 Main();
};

using namespace smooth;

#if defined __WIN32__
int WINAPI WinMain(HINSTANCE shInstance, HINSTANCE shPrevInstance, LPSTR szCmdLine, int iCmdShow)
{
	Int		 retValue = -1;

	hInstance     = shInstance;
	hPrevInstance = shPrevInstance;

	ArgumentsParser	 args(NIL, szCmdLine);

	GUI::Application::SetCommand(args.GetCommand());
	GUI::Application::SetArguments(args.GetArguments());

	GUI::Application::GetStartupDirectory();
	GUI::Application::GetApplicationDirectory();

	if (Init())
	{
		if (System::System::GetAPIVersion() != (String) SMOOTH_APIVERSION)
		{
			if (IDNO == GUI::Dialogs::QuickMessage("This program might not be compatible with the smooth library\ninstalled on your system. Continue execution?", "Warning", MB_YESNO, IDI_QUESTION))
			{
				Free();

				return -1;
			}
		}

		retValue = Main();

		Free();
	}

	return retValue;
}
#else
int main(int argc, char **argv)
{
	Int		 retValue = -1;
	String		 cmdLine;

	for (Int i = 1; i < argc; i++) cmdLine.Append(String(i > 1 ? " " : "").Append(argv[i]));

	ArgumentsParser	 args(argv[0], cmdLine);

	GUI::Application::SetCommand(args.GetCommand());
	GUI::Application::SetArguments(args.GetArguments());

	GUI::Application::GetStartupDirectory();
	GUI::Application::GetApplicationDirectory();

	if (Init())
	{
		if (System::System::GetAPIVersion() != (String) SMOOTH_APIVERSION)
		{
			Free();

			return -1;
		}

		retValue = Main();

		Free();
	}

	return retValue;
}
#endif

#endif
