 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

Array<DynamicLoader *>	 winamp_in_plugins;
Array<In_Module *>	 winamp_in_modules;

Bool LoadWinampDLLs()
{
	MoveFileA(GUI::Application::GetApplicationDirectory().Append("plugins\\plugins.ini"), GUI::Application::GetApplicationDirectory().Append("BonkEnc.ini"));

	Directory		 directory = GUI::Application::GetApplicationDirectory().Append("plugins\\");
	const Array<File>	&in_dlls   = directory.GetFilesByPattern("in_*.dll");

	for (Int i = 0; i < in_dlls.Length(); i++)
	{
		DynamicLoader	*dll = new DynamicLoader(in_dlls.GetNth(i));

		if (dll != NIL)
		{
			In_Module *(*proc)() = (In_Module *(*)()) dll->GetFunctionAddress("winampGetInModule2");

			if (proc != NIL)
			{
				winamp_in_plugins.Add(dll);
				winamp_in_modules.Add(proc());

				proc()->hDllInstance = (HINSTANCE) dll->GetSystemModuleHandle();
				proc()->Init();
			}
			else
			{
				Object::DeleteObject(dll);
			}
		}
	}

	return True;
}

Void FreeWinampDLLs()
{
	for (Int i = 0; i < winamp_in_plugins.Length(); i++)
	{
		winamp_in_modules.GetNth(i)->Quit();

		Object::DeleteObject(winamp_in_plugins.GetNth(i));
	}

	winamp_in_plugins.RemoveAll();
	winamp_in_modules.RemoveAll();

	MoveFileA(GUI::Application::GetApplicationDirectory().Append("BonkEnc.ini"), GUI::Application::GetApplicationDirectory().Append("plugins\\plugins.ini"));
}
