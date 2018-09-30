 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/component.h>

BoCA::AS::Component::Component(ComponentSpecs *iSpecs)
{
	specs = iSpecs;

	if (specs->mode == COMPONENT_MODE_INTERNAL) component = specs->func_Create();
	else					    component = NIL;
}

BoCA::AS::Component::~Component()
{
	if (specs->mode == COMPONENT_MODE_INTERNAL) specs->func_Delete(component);
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

BoCA::ComponentType BoCA::AS::Component::GetType() const
{
	return specs->type;
}

const Array<BoCA::AS::InputSpec *> &BoCA::AS::Component::GetInputSpecs() const
{
	return specs->inputs;
}

const Array<BoCA::AS::Parameter *> &BoCA::AS::Component::GetParameters() const
{
	return specs->parameters;
}

const Array<BoCA::AS::FileFormat *> &BoCA::AS::Component::GetFormats() const
{
	return specs->formats;
}

const Array<BoCA::AS::TagSpec *> &BoCA::AS::Component::GetTagSpecs() const
{
	return specs->tags;
}

BoCA::ConfigLayer *BoCA::AS::Component::GetConfigurationLayer()
{
	return (ConfigLayer *) specs->func_GetConfigurationLayer(component);
}

Bool BoCA::AS::Component::GetErrorState() const
{
	return specs->func_GetErrorState(component);
}

const String &BoCA::AS::Component::GetErrorString() const
{
	return *((const String *) specs->func_GetErrorString(component));
}

const BoCA::Config *BoCA::AS::Component::GetConfiguration() const
{
	return (const Config *) specs->func_GetConfiguration(component);
}

Bool BoCA::AS::Component::SetConfiguration(const Config *configuration)
{
	return specs->func_SetConfiguration(component, configuration);
}
