 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/component.h>

BoCA::AS::Component::Component(ComponentSpecs *iSpecs)
{
	specs = iSpecs;

	if (specs->mode == INTERNAL) component = specs->func_Create();
}

BoCA::AS::Component::~Component()
{
	if (specs->mode == INTERNAL) specs->func_Delete(component);
}

String BoCA::AS::Component::GetComponentSpecs()
{
	return specs->func_GetComponentSpecs();
}

const String &BoCA::AS::Component::GetName() const
{
	return specs->name;
}

const String &BoCA::AS::Component::GetVersion() const
{
	return specs->version;
}

const String &BoCA::AS::Component::GetID() const
{
	return specs->id;
}

Int BoCA::AS::Component::GetType() const
{
	return specs->type;
}

const Array<BoCA::AS::FileFormat *> &BoCA::AS::Component::GetFormats() const
{
	return specs->formats;
}

BoCA::ConfigLayer *BoCA::AS::Component::GetConfigurationLayer()
{
	return (ConfigLayer *) specs->func_GetConfigurationLayer(component);
}

Void BoCA::AS::Component::FreeConfigurationLayer()
{
	specs->func_FreeConfigurationLayer(component);
}

Bool BoCA::AS::Component::GetErrorState()
{
	return specs->func_GetErrorState(component);
}

const String &BoCA::AS::Component::GetErrorString()
{
	return *((const String *) specs->func_GetErrorString(component));
}
