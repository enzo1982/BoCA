 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/component/component.h>

BoCA::CS::Component::Component()
{
	errorState	= False;
	errorString	= "Unknown error";
}

BoCA::CS::Component::~Component()
{
}

BoCA::ConfigLayer *BoCA::CS::Component::GetConfigurationLayer()
{
	return NIL;
}
