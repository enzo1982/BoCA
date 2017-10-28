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

Array<DynamicLoader *, Void *>	 winamp_out_plugins;
Array<Out_Module *, Void *>	 winamp_out_modules;

Bool LoadWinampDLLs()
{
	MoveFileA(GUI::Application::GetApplicationDirectory().Append("plugins\\plugins.ini"), GUI::Application::GetApplicationDirectory().Append("freac.ini"));

	Directory		 directory = GUI::Application::GetApplicationDirectory().Append("plugins\\");
	const Array<File>	&out_dlls   = directory.GetFilesByPattern("out_*.dll");

	for (Int i = 0; i < out_dlls.Length(); i++)
	{
		DynamicLoader	*dll = new DynamicLoader(out_dlls.GetNth(i));

		if (dll == NIL) continue;

		/* Get winampGetOutModule function address.
		 */
		Out_Module *(*proc)() = (Out_Module *(*)()) dll->GetFunctionAddress("winampGetOutModule");

		if (proc == NIL) { Object::DeleteObject(dll); continue; }

		/* Get output module description.
		 */
		Out_Module	*module = proc();

		if (module->version != OUT_VER &&
		    module->version != OUT_VER_U) { Object::DeleteObject(dll); continue; }

		/* Add module to list.
		 */
		winamp_out_plugins.Add(dll);
		winamp_out_modules.Add(module);

		module->hDllInstance = (HINSTANCE) dll->GetSystemModuleHandle();
		module->Init();
	}

	return True;
}

Void FreeWinampDLLs()
{
	for (Int j = 0; j < winamp_out_plugins.Length(); j++)
	{
		winamp_out_modules.GetNth(j)->Quit();

		Object::DeleteObject(winamp_out_plugins.GetNth(j));
	}

	winamp_out_plugins.RemoveAll();
	winamp_out_modules.RemoveAll();

	MoveFileA(GUI::Application::GetApplicationDirectory().Append("freac.ini"), GUI::Application::GetApplicationDirectory().Append("plugins\\plugins.ini"));
}
