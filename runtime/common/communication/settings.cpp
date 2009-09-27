 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/communication/settings.h>

BoCA::Settings *BoCA::Settings::instance = NIL;

BoCA::Settings::Settings()
{
}

BoCA::Settings::~Settings()
{
}

BoCA::Settings *BoCA::Settings::Get()
{
	if (instance == NIL)
	{
		instance = new Settings();
	}

	return instance;
}

Void BoCA::Settings::Free()
{
	if (instance != NIL)
	{
		delete instance;

		instance = NIL;
	}
}
