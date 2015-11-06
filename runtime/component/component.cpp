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

#include <boca/component/component.h>

BoCA::CS::Component::Component()
{
	configuration	= NIL;

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

const BoCA::Config *BoCA::CS::Component::GetConfiguration() const
{
	if (configuration != NIL) return configuration;
	else			  return Config::Get();
}

Bool BoCA::CS::Component::SetConfiguration(const Config *nConfiguration)
{
	configuration = nConfiguration;

	return True;
}
