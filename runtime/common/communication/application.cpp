 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/communication/application.h>

BoCA::Application *BoCA::Application::instance = NIL;

BoCA::Application::Application()
{
}

BoCA::Application::~Application()
{
}

BoCA::Application *BoCA::Application::Get()
{
	if (instance == NIL)
	{
		instance = new Application();
	}

	return instance;
}

Void BoCA::Application::Free()
{
	if (instance != NIL)
	{
		delete instance;

		instance = NIL;
	}
}
