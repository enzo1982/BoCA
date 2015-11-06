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

#include <boca/common/communication/engine.h>

BoCA::Engine *BoCA::Engine::instance = NIL;

BoCA::Engine::Engine()
{
}

BoCA::Engine::~Engine()
{
}

BoCA::Engine *BoCA::Engine::Get()
{
	if (instance == NIL)
	{
		instance = new Engine();
	}

	return instance;
}

Void BoCA::Engine::Free()
{
	if (instance != NIL)
	{
		delete instance;

		instance = NIL;
	}
}
