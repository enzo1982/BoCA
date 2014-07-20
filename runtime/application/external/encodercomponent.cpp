 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/encodercomponent.h>
#include <boca/application/external/configlayer.h>

#include <boca/application/registry.h>
#include <boca/application/taggercomponent.h>

#include <boca/common/config.h>

BoCA::AS::EncoderComponentExternal::EncoderComponentExternal(ComponentSpecs *specs) : EncoderComponent(specs)
{
	errorState	= False;
	errorString	= "Unknown error";

	configLayer = NIL;
}

BoCA::AS::EncoderComponentExternal::~EncoderComponentExternal()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

String BoCA::AS::EncoderComponentExternal::GetOutputFileExtension()
{
	return specs->formats.GetFirst()->GetExtensions().GetFirst();
}

BoCA::ConfigLayer *BoCA::AS::EncoderComponentExternal::GetConfigurationLayer()
{
	if (configLayer == NIL && specs->external_parameters.Length() > 0) configLayer = new ConfigLayerExternal(specs);

	return configLayer;
}

Int BoCA::AS::EncoderComponentExternal::RenderTag(const String &streamURI, const Track &track, Buffer<UnsignedByte> &tagBuffer)
{
	Config	*config = Config::Get();

	/* Get tagger mode, format and ID
	 */
	Int	 tagMode = TAG_MODE_NONE;
	String	 tagFormat;
	String	 taggerID;

	String	 lcURI = streamURI.ToLower();

	foreach (FileFormat *format, specs->formats)
	{
		foreach (const String &extension, format->GetExtensions())
		{
			if (lcURI.EndsWith(String(".").Append(extension)))
			{
				tagMode	  = format->GetTagMode();
				tagFormat = format->GetTagFormat();
				taggerID  = format->GetTaggerID();

				break;
			}
		}
	}

	/* Create tag if requested
	 */
	const Info	&info = track.GetInfo();

	if (tagMode != TAG_MODE_NONE && (info.artist != NIL || info.title != NIL))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID(taggerID);

		if (tagger != NIL)
		{
			foreach (TagSpec *spec, tagger->GetTagSpecs())
			{
				if (spec->GetName() != tagFormat) continue;

				if (config->GetIntValue("Tags", String("Enable").Append(String(tagFormat).Replace(" ", NIL)), spec->IsDefault()))
				{
					if (tagMode == TAG_MODE_OTHER)	tagger->RenderStreamInfo(streamURI, track);
					else				tagger->RenderBuffer(tagBuffer, track);
				}

				break;
			}

			boca.DeleteComponent(tagger);
		}
	}

	return tagMode;
}
