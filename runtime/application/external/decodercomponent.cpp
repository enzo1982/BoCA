 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/decodercomponent.h>
#include <boca/application/external/configlayer.h>

#include <boca/application/registry.h>
#include <boca/application/taggercomponent.h>

BoCA::AS::DecoderComponentExternal::DecoderComponentExternal(ComponentSpecs *specs) : DecoderComponent(specs)
{
	errorState	= False;
	errorString	= "Unknown error";

	configLayer	= NIL;

	packageSize	= 0;
	inBytes		= 0;
}

BoCA::AS::DecoderComponentExternal::~DecoderComponentExternal()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::AS::DecoderComponentExternal::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	for (Int i = 0; i < specs->formats.Length(); i++)
	{
		FileFormat	*format = specs->formats.GetNth(i);

		for (Int j = 0; j < format->GetExtensions().Length(); j++)
		{
			if (lcURI.EndsWith(String(".").Append(format->GetExtensions().GetNth(j)))) return True;
		}
	}

	return False;
}

Int BoCA::AS::DecoderComponentExternal::GetPackageSize()
{
	return packageSize;
}

Int BoCA::AS::DecoderComponentExternal::SetDriver(IO::Driver *driver)
{
	return IO::Filter::SetDriver(driver);
}

BoCA::ConfigLayer *BoCA::AS::DecoderComponentExternal::GetConfigurationLayer()
{
	if (configLayer == NIL && specs->external_parameters.Length() > 0) configLayer = new ConfigLayerExternal(specs);

	return configLayer;
}

Int BoCA::AS::DecoderComponentExternal::QueryTags(const String &streamURI, Track &track) const
{
	/* Get tagger mode and ID
	 */
	Int	 tagMode = TAG_MODE_NONE;
	String	 taggerID;

	String	 lcURI = streamURI.ToLower();

	foreach (FileFormat *format, specs->formats)
	{
		foreach (const String &extension, format->GetExtensions())
		{
			if (lcURI.EndsWith(String(".").Append(extension)))
			{
				tagMode	  = format->GetTagMode();
				taggerID  = format->GetTaggerID();

				break;
			}
		}
	}

	/* Read tag if requested
	 */
	if (tagMode != TAG_MODE_NONE)
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID(taggerID);

		if (tagger != NIL)
		{
			tagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(tagger);
		}
	}

	return tagMode;
}
