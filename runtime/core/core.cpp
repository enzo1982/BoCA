 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2016 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/core/core.h>

#include <boca/application/registry.h>

#include <boca/common/config.h>
#include <boca/common/i18n.h>
#include <boca/common/utilities.h>

#include <boca/common/communication/application.h>
#include <boca/common/communication/engine.h>
#include <boca/common/communication/joblist.h>
#include <boca/common/communication/menu.h>
#include <boca/common/communication/settings.h>

namespace BoCA
{
	static String	 applicationPrefix;
};

Void BoCA::Init(const String &appPrefix)
{
	applicationPrefix = Utilities::ReplaceIncompatibleCharacters(String(appPrefix).Replace(" ", NIL)).ToLower();
}

Void BoCA::Free()
{
	BoCA::AS::Registry::Free();

	BoCA::Application::Free();
	BoCA::Engine::Free();
	BoCA::JobList::Free();
	BoCA::Menu::Free();
	BoCA::Settings::Free();

	BoCA::I18n::Free();
	BoCA::Config::Free();
}

const String &BoCA::GetApplicationPrefix()
{
	return applicationPrefix;
}
