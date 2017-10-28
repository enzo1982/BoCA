 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"

Array<DynamicLoader *, Void *>	 winamp_in_plugins;
Array<In_Module *, Void *>	 winamp_in_modules;

Bool LoadWinampDLLs()
{
	MoveFileA(GUI::Application::GetApplicationDirectory().Append("plugins\\plugins.ini"), GUI::Application::GetApplicationDirectory().Append("freac.ini"));

	Directory		 directory = GUI::Application::GetApplicationDirectory().Append("plugins\\");
	const Array<File>	&in_dlls   = directory.GetFilesByPattern("in_*.dll");

	for (Int i = 0; i < in_dlls.Length(); i++)
	{
		DynamicLoader	*dll = new DynamicLoader(in_dlls.GetNth(i));

		if (dll == NIL) continue;

		/* Get winampGetInModule2 function address.
		 */
		In_Module *(*proc)() = (In_Module *(*)()) dll->GetFunctionAddress("winampGetInModule2");

		if (proc == NIL) { Object::DeleteObject(dll); continue; }

		/* Get input module description.
		 */
		In_Module	*module = proc();

		if (module->version != IN_VER &&
		    module->version != IN_VER_OLD) { Object::DeleteObject(dll); continue; }

		/* Add module to list.
		 */
		winamp_in_plugins.Add(dll);
		winamp_in_modules.Add(module);

		module->hDllInstance = (HINSTANCE) dll->GetSystemModuleHandle();
		module->hMainWindow = 0;
		module->Init();
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

	MoveFileA(GUI::Application::GetApplicationDirectory().Append("freac.ini"), GUI::Application::GetApplicationDirectory().Append("plugins\\plugins.ini"));
}
