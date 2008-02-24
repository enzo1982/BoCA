 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/registry.h>
#include <boca/application/decodercomponent.h>
#include <boca/application/encodercomponent.h>
#include <boca/application/outputcomponent.h>

BoCA::AS::Registry	*BoCA::AS::Registry::registry = NIL;

BoCA::AS::Registry &BoCA::AS::Registry::Get()
{
	if (registry == NIL) registry = new Registry();

	return *registry;
}

Bool BoCA::AS::Registry::Free()
{
	if (registry == NIL) return False;

	delete registry;

	registry = NIL;

	return True;
}

BoCA::AS::Registry::Registry()
{
	// Load the BoCA runtime library into memory first
	bocaLib = new DynamicLoader("boca/boca.1.0.dll");

	Directory		 dir(GUI::Application::GetApplicationDirectory().Append("boca"));

	const Array<File>	&files = dir.GetFilesByPattern("boca_*.dll");

	for (Int i = 0; i < files.Length(); i++)
	{
		File		 file = files.GetNth(i);
		ComponentSpecs	*specs = new ComponentSpecs();

		if (specs->LoadFromFile(file))
		{
			componentSpecs.Add(specs);
		}
		else
		{
			delete specs;
		}
	}
}

BoCA::AS::Registry::~Registry()
{
	for (Int i = 0; i < componentSpecs.Length(); i++)
	{
		delete componentSpecs.GetNth(i);
	}

	delete bocaLib;
}

Int BoCA::AS::Registry::GetNumberOfComponents()
{
	return componentSpecs.Length();
}

const String &BoCA::AS::Registry::GetComponentName(Int n)
{
	return componentSpecs.GetNth(n)->name;
}

const String &BoCA::AS::Registry::GetComponentVersion(Int n)
{
	return componentSpecs.GetNth(n)->version;
}

const String &BoCA::AS::Registry::GetComponentID(Int n)
{
	return componentSpecs.GetNth(n)->id;
}

Int BoCA::AS::Registry::GetComponentType(Int n)
{
	return componentSpecs.GetNth(n)->type;
}

const Array<BoCA::AS::Format *> &BoCA::AS::Registry::GetComponentFormats(Int n)
{
	return componentSpecs.GetNth(n)->formats;
}

BoCA::AS::Component *BoCA::AS::Registry::CreateComponentByID(const String &id)
{
	for (Int i = 0; i < componentSpecs.Length(); i++)
	{
		ComponentSpecs	*specs = componentSpecs.GetNth(i);

		if (specs->id == id)
		{
			if	(specs->type == COMPONENT_TYPE_DECODER)	return new DecoderComponent(specs);
			else if (specs->type == COMPONENT_TYPE_ENCODER)	return new EncoderComponent(specs);
			else if (specs->type == COMPONENT_TYPE_OUTPUT)	return new OutputComponent(specs);
			else						return new Component(specs);
		}
	}

	return NIL;
}

Bool BoCA::AS::Registry::DeleteComponent(Component *component)
{
	if (component == NIL) return False;

	delete component;

	return True;
}
