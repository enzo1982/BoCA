 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>

#include "volume.h"
#include "config.h"

const String &BoCA::DSPVolume::GetComponentSpecs()
{
	I18n	*i18n = I18n::Get();

	i18n->SetContext("Components::DSP");

	static String	 componentSpecs = String("						\
												\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>					\
		  <component>									\
		    <name>").Append(i18n->TranslateString("Volume Adjustment")).Append("</name>	\
		    <version>1.0</version>							\
		    <id>volume-dsp</id>								\
		    <type>dsp</type>								\
		    <input float=\"true\"/>							\
		  </component>									\
												\
		");

	return componentSpecs;
}

BoCA::DSPVolume::DSPVolume()
{
	configLayer = NIL;

	scaleFactor = 1.0;
}

BoCA::DSPVolume::~DSPVolume()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DSPVolume::Activate()
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	scaleFactor = Math::Sqrt(Math::Pow(10.0, config->GetIntValue(ConfigureVolume::ConfigID, "qdB", 0) / 40.0));

	return True;
}

Int BoCA::DSPVolume::TransformData(Buffer<UnsignedByte> &data)
{
	/* Check if we need to do anything.
	 */
	if (Math::Abs(scaleFactor - 1.0) < 0.00001) return data.Size();

	/* Adjust volume.
	 */
	Int	 numSamples = data.Size() / sizeof(Float32);
	Float32	*samples    = (Float32 *) (UnsignedByte *) data;

	for (Int i = 0; i < numSamples; i++) samples[i] *= scaleFactor;

	return data.Size();
}

ConfigLayer *BoCA::DSPVolume::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureVolume();

	return configLayer;
}
