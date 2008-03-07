 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <boca/common/config.h>
#include <boca/core/dllinterfaces.h>

Void smooth::AttachDLL(Void *instance)
{
	BoCA::Config::Get();

	if (BoCA::DLLInterfaces::LoadID3DLL()) BoCA::Config::Get()->enable_id3 = True;
}

Void smooth::DetachDLL()
{
	BoCA::DLLInterfaces::FreeID3DLL();

	BoCA::Config::Free();
}
