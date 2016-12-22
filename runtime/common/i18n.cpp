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

#include <boca/common/i18n.h>
#include <boca/core/core.h>

BoCA::I18n *BoCA::I18n::instance = NIL;

BoCA::I18n::I18n() : S::I18n::Translator(BoCA::GetApplicationPrefix())
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

String BoCA::I18n::AddColon(const String &string)
{
	return TranslateString("%1:", "Characters").Replace("%1", string);
}

String BoCA::I18n::AddEllipsis(const String &string)
{
	return TranslateString("%1...", "Characters").Replace("%1", string);
}

String BoCA::I18n::AddBrackets(const String &string, const String &bracketed)
{
	return TranslateString("%1 (%2)", "Characters").Replace("%1", string).Replace("%2", bracketed);
}
