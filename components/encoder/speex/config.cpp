 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "config.h"

BoCA::ConfigureSpeex::ConfigureSpeex()
{
	Point	 pos;
	Size	 size;

	Config	*config = Config::Get();

	I18n	*i18n = I18n::Get();

	SetSize(Size(358, 102));
}

BoCA::ConfigureSpeex::~ConfigureSpeex()
{
}

Int BoCA::ConfigureSpeex::SaveSettings()
{
	Config	*config = Config::Get();

	return Success();
}
