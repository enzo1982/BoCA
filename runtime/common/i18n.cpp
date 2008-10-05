 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/common/i18n.h>

BoCA::I18n *BoCA::I18n::instance = NIL;

BoCA::I18n::I18n() : S::I18n::Translator("BonkEnc")
{
	SetInternalLanguageInfo("English", "Robert Kausch <robert.kausch@bonkenc.org>", "http://www.bonkenc.org/", False);
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
