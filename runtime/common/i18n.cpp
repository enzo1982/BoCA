 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/i18n.h>

BoCA::I18n *BoCA::I18n::instance = NIL;

BoCA::I18n::I18n() : S::I18n::Translator("freac")
{
	SetInternalLanguageInfo("English", "Robert Kausch <robert.kausch@freac.org>", "http://www.freac.org/", False);
}

BoCA::I18n::~I18n()
{
}

BoCA::I18n *BoCA::I18n::Get()
{
	if (instance == NIL)
	{
		instance = new I18n();
	}

	return instance;
}

Void BoCA::I18n::Free()
{
	if (instance != NIL)
	{
		delete instance;

		instance = NIL;
	}
}
