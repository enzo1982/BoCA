 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <boca/application/external/encodercomponent.h>
#include <boca/application/external/configlayer.h>

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

Void BoCA::AS::EncoderComponentExternal::FreeConfigurationLayer()
{
	if (configLayer != NIL)
	{
		delete configLayer;

		configLayer = NIL;
	}
}
