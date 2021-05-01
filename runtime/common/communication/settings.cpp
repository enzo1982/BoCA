 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/communication/settings.h>

BoCA::Settings			*BoCA::Settings::instance = NIL;

Signal1<Void, const String &>	 BoCA::Settings::onChangeComponentSettings;

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
