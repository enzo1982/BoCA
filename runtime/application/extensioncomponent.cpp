 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/extensioncomponent.h>

BoCA::AS::ExtensionComponent::ExtensionComponent(ComponentSpecs *iSpecs) : Component(iSpecs)
{
	getMainTabLayer.Connect(&ExtensionComponent::GetMainTabLayer, this);
	getStatusBarLayer.Connect(&ExtensionComponent::GetStatusBarLayer, this);
}

BoCA::AS::ExtensionComponent::~ExtensionComponent()
{
}

Layer *BoCA::AS::ExtensionComponent::GetMainTabLayer()
{
	return (Layer *) specs->func_GetMainTabLayer(component);
}

Layer *BoCA::AS::ExtensionComponent::GetStatusBarLayer()
{
	return (Layer *) specs->func_GetStatusBarLayer(component);
}
