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

Void smooth::AttachDLL(Void *instance)
{
	BoCA::Config	*config = BoCA::Config::Get();

	/* Set number of threads for OpenMP optimized encoders.
	 */
	if (config->GetIntValue("OpenMP", "NumThreads", 0) > 0) SetEnvironmentVariableA("OMP_NUM_THREADS", String::FromInt(config->GetIntValue("OpenMP", "NumThreads", 0)));
}

Void smooth::DetachDLL()
{
	BoCA::AS::Registry::Free();

	BoCA::JobList::Free();
	BoCA::Menu::Free();

	BoCA::I18n::Free();
	BoCA::Config::Free();
}
