 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <boca/application/registry.h>

#include <boca/common/config.h>
#include <boca/common/i18n.h>

#include <boca/common/communication/joblist.h>
#include <boca/common/communication/menu.h>

#include <boca/core/dllinterfaces.h>

Void smooth::AttachDLL(Void *instance)
{
	BoCA::Config::Get();

	if (BoCA::DLLInterfaces::LoadID3DLL())	   BoCA::Config::Get()->enable_id3 = True;
	if (BoCA::DLLInterfaces::LoadMP4v2DLL())   BoCA::Config::Get()->enable_mp4 = True;
	if (BoCA::DLLInterfaces::LoadOggDLL())	   BoCA::Config::Get()->enable_ogg = True;
	if (BoCA::DLLInterfaces::LoadWMVCoreDLL()) BoCA::Config::Get()->enable_wma = True;
}

Void smooth::DetachDLL()
{
	BoCA::DLLInterfaces::FreeID3DLL();
	BoCA::DLLInterfaces::FreeMP4v2DLL();
	BoCA::DLLInterfaces::FreeOggDLL();
	BoCA::DLLInterfaces::FreeWMVCoreDLL();

	BoCA::AS::Registry::Free();

	BoCA::JobList::Free();
	BoCA::Menu::Free();

	BoCA::I18n::Free();
	BoCA::Config::Free();
}
