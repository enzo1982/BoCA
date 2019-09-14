 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/registry.h>
#include <boca/application/decodercomponent.h>
#include <boca/application/encodercomponent.h>
#include <boca/application/deviceinfocomponent.h>
#include <boca/application/dspcomponent.h>
#include <boca/application/extensioncomponent.h>
#include <boca/application/outputcomponent.h>
#include <boca/application/playlistcomponent.h>
#include <boca/application/taggercomponent.h>
#include <boca/application/verifiercomponent.h>

#include <boca/application/external/decodercomponentfile.h>
#include <boca/application/external/decodercomponentstdio.h>
#include <boca/application/external/encodercomponentfile.h>
#include <boca/application/external/encodercomponentstdio.h>

#include <boca/common/protocol.h>
#include <boca/common/utilities.h>

#include <boca/common/communication/engine.h>

#include <boca/core/core.h>

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
	Protocol	*debug = Protocol::Get("Debug output");

	debug->Write("Initializing BoCA...");

	/* Load available components.
	 */
	Config	*configuration	   = Config::Get();

	String	 bocaPrefix	   = "boca";
	String	 applicationPrefix = BoCA::GetApplicationPrefix();

	String	 bocaDirectory	   = Utilities::GetBoCADirectory();
	String	 configDirectory   = String(configuration->configDir).Append(bocaPrefix).Append(Directory::GetDirectoryDelimiter());

	if (configDirectory != bocaDirectory) LoadComponents(configDirectory, bocaPrefix);

	LoadComponents(bocaDirectory, bocaPrefix);

	if (applicationPrefix != NIL && applicationPrefix != bocaPrefix)
	{
		if (configDirectory != bocaDirectory) LoadComponents(configDirectory, applicationPrefix);

		LoadComponents(bocaDirectory, applicationPrefix);
		LoadComponents(String(bocaDirectory).Append("..").Append(Directory::GetDirectoryDelimiter()).Append(applicationPrefix), applicationPrefix);
	}

	/* Check which components to use and
	 * order them according to their specs.
	 */
	CheckComponents();
	OrderComponents();

	/* Tell components to initialize.
	 */
	Engine	*engine = Engine::Get();

	engine->onInitialize.Emit();

	debug->Write("BoCA is ready.");
}

BoCA::AS::Registry::~Registry()
{
	/* Tell components to clean up.
	 */
	Engine	*engine = Engine::Get();

	engine->onCleanup.Emit();

	/* Disconnect all initialization and cleanup
	 * handlers before freeing components.
	 */
	engine->onInitialize.DisconnectAll();
	engine->onCleanup.DisconnectAll();

	/* Unregister all components.
	 */
	foreach (ComponentSpecs *cs, componentSpecs) delete cs;
}

Void BoCA::AS::Registry::LoadComponents(const Directory &dir, const String &prefix)
{
	Protocol	*debug = Protocol::Get("Debug output");

	debug->Write(String("  Querying native components with prefix ").Append(prefix).Append("..."));

#if defined __WIN32__
	const Array<File>	&dllFiles = dir.GetFilesByPattern(prefix.Append("_*.dll"));
#elif defined __APPLE__
	const Array<File>	&dllFiles = dir.GetFilesByPattern(prefix.Append("_*.dylib"));
#else
	const Array<File>	&dllFiles = dir.GetFilesByPattern(prefix.Append("_*.so"));
#endif

	for (Int i = 0; i < dllFiles.Length(); i++)
	{
		const File	&file  = dllFiles.GetNth(i);
		ComponentSpecs	*specs = new ComponentSpecs();

		debug->Write(String("    Trying ").Append(file).Append("..."));

		if (specs->LoadFromDLL(file))
		{
			InsertComponent(specs);
		}
		else
		{
			debug->Write(String("      Failed to load component."));

			delete specs;
		}
	}

	debug->Write(String("  Querying script components with prefix ").Append(prefix).Append("..."));

	const Array<File>	&xmlFiles = dir.GetFilesByPattern(prefix.Append("_*.xml"));

	for (Int i = 0; i < xmlFiles.Length(); i++)
	{
		const File	&file  = xmlFiles.GetNth(i);
		ComponentSpecs	*specs = new ComponentSpecs();

		debug->Write(String("    Trying ").Append(file).Append("..."));

		if (specs->LoadFromXML(file))
		{
			InsertComponent(specs);
		}
		else
		{
			debug->Write(String("      Failed to load component."));

			delete specs;
		}
	}
}

Void BoCA::AS::Registry::InsertComponent(ComponentSpecs *specs)
{
	Protocol	*debug = Protocol::Get("Debug output");

	debug->Write(String("      Inserting ").Append(specs->name).Append("..."));

	/* Sort encoder components by name.
	 */
	if (specs->type == COMPONENT_TYPE_ENCODER)
	{
		for (Int i = 0; i < componentSpecs.Length(); i++)
		{
			ComponentSpecs	*entry = componentSpecs.GetNth(i);

			if (entry->type != COMPONENT_TYPE_ENCODER) continue;

			String		 lSpecsName = String(specs->name).ToLower();
			String		 lEntryName = String(entry->name).ToLower();

			for (Int n = 0; n < lSpecsName.Length(); n++)
			{
				if	(lSpecsName[n] < lEntryName[n]) { componentSpecs.InsertAtPos(i, specs); return; }
				else if (lSpecsName[n] > lEntryName[n]) { 					break;  }
			}
		}
	}

	componentSpecs.Add(specs);
}

Void BoCA::AS::Registry::CheckComponents()
{
	Protocol	*debug = Protocol::Get("Debug output");

	debug->Write("  Checking components...");

	for (Int i = 0; i < componentSpecs.Length(); i++)
	{
		ComponentSpecs	*cs = componentSpecs.GetNth(i);

		/* Check required components.
		 */
		foreach (const String &requireComponent, cs->requireComponents)
		{
			if (!ComponentExists(requireComponent)) { componentSpecs.RemoveNth(i--); delete cs; cs = NIL; break; }
		}

		if (cs == NIL) continue;

		/* Check conflicting components.
		 */
		foreach (const String &conflictComponent, cs->conflictComponents)
		{
			if (ComponentExists(conflictComponent)) { componentSpecs.RemoveNth(i--); delete cs; cs = NIL; break; }
		}

		if (cs == NIL) continue;

		/* Check replaced components.
		 */
		foreach (const String &replaceComponent, cs->replaceComponents)
		{
			for (Int j = 0; j < componentSpecs.Length(); j++)
			{
				/* Prevent components from replacing theirselves.
				 */
				if (j == i) continue;

				ComponentSpecs	*csr = componentSpecs.GetNth(j);

				if (csr->id == replaceComponent) { componentSpecs.RemoveNth(j); if (j < i) i--; delete csr; break; }
			}
		}
	}
}

Void BoCA::AS::Registry::OrderComponents()
{
	Protocol	*debug = Protocol::Get("Debug output");

	debug->Write("  Sorting components...");

	/* Check preceding components.
	 */
	for (Int i = 0; i < componentSpecs.Length(); i++)
	{
		ComponentSpecs	*cs = componentSpecs.GetNth(i);

		foreach (const String &precedeComponent, cs->precedeComponents)
		{
			for (Int j = 0; j < i; j++)
			{
				ComponentSpecs	*csp = componentSpecs.GetNth(j);

				if (csp->id != precedeComponent) continue;

				componentSpecs.RemoveNth(i);
				componentSpecs.InsertAtPos(j, cs);

				i = j;

				break;
			}
		}
	}

	/* Check succeeding components.
	 */
	for (Int i = componentSpecs.Length() - 1; i >= 0; i--)
	{
		ComponentSpecs	*cs = componentSpecs.GetNth(i);

		foreach (const String &succeedComponent, cs->succeedComponents)
		{
			for (Int j = componentSpecs.Length() - 1; j > i; j--)
			{
				ComponentSpecs	*csp = componentSpecs.GetNth(j);

				if (csp->id != succeedComponent) continue;

				componentSpecs.RemoveNth(i);
				componentSpecs.InsertAtPos(j, cs);

				i = j;

				break;
			}
		}
	}
}

Int BoCA::AS::Registry::GetNumberOfComponents()
{
	return componentSpecs.Length();
}

Int BoCA::AS::Registry::GetNumberOfComponentsOfType(ComponentType type)
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

BoCA::ComponentType BoCA::AS::Registry::GetComponentType(Int n)
{
	return componentSpecs.GetNth(n)->type;
}

const Array<BoCA::AS::Parameter *> &BoCA::AS::Registry::GetComponentParameters(Int n)
{
	return componentSpecs.GetNth(n)->parameters;
}

const Array<BoCA::AS::FileFormat *> &BoCA::AS::Registry::GetComponentFormats(Int n)
{
	return componentSpecs.GetNth(n)->formats;
}

const Array<BoCA::AS::TagSpec *> &BoCA::AS::Registry::GetComponentTagSpecs(Int n)
{
	return componentSpecs.GetNth(n)->tags;
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
				if	(specs->mode == COMPONENT_MODE_EXTERNAL_FILE)	return new DecoderComponentExternalFile(specs);
				else if	(specs->mode == COMPONENT_MODE_EXTERNAL_STDIO)	return new DecoderComponentExternalStdIO(specs);

				return new DecoderComponent(specs);
			case COMPONENT_TYPE_ENCODER:
				if	(specs->mode == COMPONENT_MODE_EXTERNAL_FILE)	return new EncoderComponentExternalFile(specs);
				else if	(specs->mode == COMPONENT_MODE_EXTERNAL_STDIO)	return new EncoderComponentExternalStdIO(specs);

				return new EncoderComponent(specs);
			case COMPONENT_TYPE_DEVICEINFO:
				return new DeviceInfoComponent(specs);
			case COMPONENT_TYPE_DSP:
				return new DSPComponent(specs);
			case COMPONENT_TYPE_EXTENSION:
				return new ExtensionComponent(specs);
			case COMPONENT_TYPE_OUTPUT:
				return new OutputComponent(specs);
			case COMPONENT_TYPE_PLAYLIST:
				return new PlaylistComponent(specs);
			case COMPONENT_TYPE_TAGGER:
				return new TaggerComponent(specs);
			case COMPONENT_TYPE_VERIFIER:
				return new VerifierComponent(specs);
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

BoCA::AS::DecoderComponent *BoCA::AS::Registry::CreateDecoderForStream(const String &streamURI, const Config *config)
{
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

				DecoderComponent	*component = (DecoderComponent *) CreateComponentByID(GetComponentID(i));

				if (component == NIL) continue;

				if (config != NIL) component->SetConfiguration(config);

				if (component->CanOpenStream(streamURI)) return component;

				DeleteComponent(component);
			}
		}

/* TODO: Implement protocols.
 */
/*		const Array<Protocol *>	&protocols = GetComponentProtocols(i);

		foreach (Protocol *protocol, protocols)
		{
			if (!streamURI.ToLower().StartsWith(protocol->GetIdentifier().Append("://"))) continue;

			DecoderComponent	*component = (DecoderComponent *) CreateComponentByID(GetComponentID(i));

			if (component == NIL) continue;

			if (config != NIL) component->SetConfiguration(config);

			if (component->CanOpenStream(streamURI)) return component;

			DeleteComponent(component);
		}
*/	}

	/* No suitable decoder found; try all decoders now.
	 */
	for (Int i = 0; i < GetNumberOfComponents(); i++)
	{
		if (GetComponentType(i) != COMPONENT_TYPE_DECODER) continue;

		DecoderComponent	*component = (DecoderComponent *) CreateComponentByID(GetComponentID(i));

		if (component == NIL) continue;

		if (config != NIL) component->SetConfiguration(config);

		if (component->CanOpenStream(streamURI)) return component;

		DeleteComponent(component);
	}

	return NIL;
}

BoCA::AS::VerifierComponent *BoCA::AS::Registry::CreateVerifierForTrack(const Track &track, const Config *config)
{
	/* Try all verifiers.
	 */
	for (Int i = 0; i < GetNumberOfComponents(); i++)
	{
		if (GetComponentType(i) != COMPONENT_TYPE_VERIFIER) continue;

		VerifierComponent	*component = (VerifierComponent *) CreateComponentByID(GetComponentID(i));

		if (component == NIL) continue;

		if (config != NIL) component->SetConfiguration(config);

		if (component->CanVerifyTrack(track)) return component;

		DeleteComponent(component);
	}

	return NIL;
}

BoCA::AS::DeviceInfoComponent *BoCA::AS::Registry::CreateDeviceInfoComponent()
{
	/* Try to find an active device info component.
	 */
	for (Int i = 0; i < GetNumberOfComponents(); i++)
	{
		if (GetComponentType(i) != COMPONENT_TYPE_DEVICEINFO) continue;

		DeviceInfoComponent	*component = (DeviceInfoComponent *) CreateComponentByID(GetComponentID(i));

		if (component != NIL) return component;
	}

	return NIL;
}
