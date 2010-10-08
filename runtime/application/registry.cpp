 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
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

#ifdef __WIN32__
#	include <boca/application/external/decodercomponentfile.h>
#	include <boca/application/external/decodercomponentstdio.h>
#	include <boca/application/external/encodercomponentfile.h>
#	include <boca/application/external/encodercomponentstdio.h>
#endif

#include <boca/application/deviceinfocomponent.h>
#include <boca/application/dspcomponent.h>
#include <boca/application/extensioncomponent.h>
#include <boca/application/outputcomponent.h>
#include <boca/application/taggercomponent.h>

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
	Directory		 dir(GUI::Application::GetApplicationDirectory().Append("boca"));

#ifdef __WIN32__
	const Array<File>	&dllFiles = dir.GetFilesByPattern("boca_*.dll");
#else
	const Array<File>	&dllFiles = dir.GetFilesByPattern("boca_*.so");
#endif

	for (Int i = 0; i < dllFiles.Length(); i++)
	{
		File		 file = dllFiles.GetNth(i);
		ComponentSpecs	*specs = new ComponentSpecs();

		if (specs->LoadFromDLL(file))
		{
			componentSpecs.Add(specs);
		}
		else
		{
			delete specs;
		}
	}

	const Array<File>	&xmlFiles = dir.GetFilesByPattern("boca_*.xml");

	for (Int i = 0; i < xmlFiles.Length(); i++)
	{
		File		 file = xmlFiles.GetNth(i);
		ComponentSpecs	*specs = new ComponentSpecs();

		if (specs->LoadFromXML(file))
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
	foreach (ComponentSpecs *cs, componentSpecs) delete cs;
}

Int BoCA::AS::Registry::GetNumberOfComponents()
{
	return componentSpecs.Length();
}

Int BoCA::AS::Registry::GetNumberOfComponentsOfType(Int type)
{
	Int	 number = 0;

	for (Int i = 0; i < componentSpecs.Length(); i++)
	{
		if (componentSpecs.GetNth(i)->type == type) number++;
	}

	return number;
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

const Array<BoCA::AS::FileFormat *> &BoCA::AS::Registry::GetComponentFormats(Int n)
{
	return componentSpecs.GetNth(n)->formats;
}

const Array<BoCA::AS::TagFormat *> &BoCA::AS::Registry::GetComponentTagFormats(Int n)
{
	return componentSpecs.GetNth(n)->tag_formats;
}

Bool BoCA::AS::Registry::ComponentExists(const String &id)
{
	for (Int i = 0; i < componentSpecs.Length(); i++)
	{
		if (GetComponentID(i) == id) return True;
	}

	return False;
}

BoCA::AS::Component *BoCA::AS::Registry::CreateComponentByID(const String &id)
{
	for (Int i = 0; i < componentSpecs.Length(); i++)
	{
		ComponentSpecs	*specs = componentSpecs.GetNth(i);

		if (specs->id != id) continue;

		switch (specs->type)
		{
			case COMPONENT_TYPE_DECODER:
				if	(specs->mode == INTERNAL)	return new DecoderComponent(specs);
#ifdef __WIN32__
				else if (specs->mode == EXTERNAL_FILE)	return new DecoderComponentExternalFile(specs);
				else if (specs->mode == EXTERNAL_STDIO)	return new DecoderComponentExternalStdIO(specs);
#endif
				break;
			case COMPONENT_TYPE_ENCODER:
				if	(specs->mode == INTERNAL)	return new EncoderComponent(specs);
#ifdef __WIN32__
				else if (specs->mode == EXTERNAL_FILE)	return new EncoderComponentExternalFile(specs);
				else if (specs->mode == EXTERNAL_STDIO)	return new EncoderComponentExternalStdIO(specs);
#endif
				break;
			case COMPONENT_TYPE_DEVICEINFO:
				return new DeviceInfoComponent(specs);
			case COMPONENT_TYPE_DSP:
				return new DSPComponent(specs);
			case COMPONENT_TYPE_EXTENSION:
				return new ExtensionComponent(specs);
			case COMPONENT_TYPE_OUTPUT:
				return new OutputComponent(specs);
			case COMPONENT_TYPE_TAGGER:
				return new TaggerComponent(specs);
			default:
				return new Component(specs);
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

BoCA::AS::DecoderComponent *BoCA::AS::Registry::CreateDecoderForStream(const String &streamURI)
{
	DecoderComponent	*component = NIL;

	/* Check those decoders that claim to
	 * support the file extension first.
	 */
	for (Int i = 0; i < GetNumberOfComponents(); i++)
	{
		if (GetComponentType(i) != COMPONENT_TYPE_DECODER) continue;

		const Array<FileFormat *>	&formats = GetComponentFormats(i);

		foreach (FileFormat *format, formats)
		{
			const Array<String>	&extensions = format->GetExtensions();

			foreach (const String &extension, extensions)
			{
				if (!streamURI.ToLower().EndsWith(String(".").Append(extension.ToLower()))) continue;

				component = (DecoderComponent *) CreateComponentByID(GetComponentID(i));

				if (component->CanOpenStream(streamURI)) return component;

				DeleteComponent(component);
			}
		}

/* TODO: Implement protocols.
 */
/*		const Array<Protocol *>	&protocols = GetComponentProtocols(i);

		foreach (Protocol *protocol, protocols)
		{
			if (!streamURI.ToLower().StartsWith(String(protocol->GetIdentifier()).Append("://"))) continue;

			component = (DecoderComponent *) CreateComponentByID(GetComponentID(i));

			if (component->CanOpenStream(streamURI)) return component;

			DeleteComponent(component);
		}
*/	}

	/* No suitable decoder found; try all decoders now.
	 */
	for (Int i = 0; i < GetNumberOfComponents(); i++)
	{
		if (GetComponentType(i) != COMPONENT_TYPE_DECODER) continue;

		component = (DecoderComponent *) CreateComponentByID(GetComponentID(i));

		if (component->CanOpenStream(streamURI)) return component;

		DeleteComponent(component);
	}

	return NIL;
}
