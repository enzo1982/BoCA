 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2020 Robert Kausch <robert.kausch@freac.org>
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
#include <smooth/dll.h>

#include "sndfile.h"
#include "config.h"

const String &BoCA::EncoderSndFile::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (sndfiledll != NIL)
	{
		I18n	*i18n = I18n::Get();

		i18n->SetContext("Components::Encoders");

		componentSpecs = String("								\
													\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
		  <component>										\
		    <name>").Append(i18n->TranslateString("Windows Wave File Output")).Append("</name>	\
		    <version>1.0</version>								\
		    <id>sndfile-wave-enc</id>								\
		    <type>encoder</type>								\
		    <require>sndfile-enc</require>							\
		    <replace>wave-enc</replace>								\
		    <format>										\
		      <name>Microsoft Wave Files</name>							\
		      <extension>wav</extension>							\
		      <tag id=\"riff-tag\" mode=\"other\">RIFF INFO Tag</tag>				\
		      <tag id=\"cart-tag\" mode=\"other\">RIFF Cart Tag</tag>				\
		      <tag id=\"id3v2-tag\" mode=\"other\">ID3v2</tag>					\
		    </format>										\
		    <input bits=\"8-32\"/>								\
		    <input float=\"true\"/>								\
		  </component>										\
													\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadSndFileDLL();
}

Void smooth::DetachDLL()
{
	FreeSndFileDLL();
}

BoCA::EncoderSndFile::EncoderSndFile()
{
	configLayer = NIL;
	config	    = NIL;

	encoder	    = NIL;

	fileFormat  = 0;

	sndf	    = NIL;
}

BoCA::EncoderSndFile::~EncoderSndFile()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderSndFile::Activate()
{
	const Format	&format = track.GetFormat();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	/* Create downstream encoder.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	encoder = (AS::EncoderComponent *) boca.CreateComponentByID("sndfile-enc");

	if (encoder == NIL) return False;

	config->SetIntValue("SndFile", "Format", SF_FORMAT_WAV);
	config->SetIntValue("SndFile", "SubFormat", config->GetIntValue(ConfigureSndFile::ConfigID, "SubFormat", 0));

	encoder->SetConfiguration(config);

	encoder->SetAudioTrackInfo(track);
	encoder->SetDriver(driver);

	if(!encoder->Activate())
	{
		errorState  = True;
		errorString = encoder->GetErrorString();

		boca.DeleteComponent(encoder);

		return False;
	}

	return True;
}

Bool BoCA::EncoderSndFile::Deactivate()
{
	/* Forward deactivate call.
	 */
	Bool	 result = encoder->Deactivate();

	/* Free downstream encoder.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	boca.DeleteComponent(encoder);

	return result;
}

Int BoCA::EncoderSndFile::WriteData(Buffer<UnsignedByte> &data)
{
	/* Forward write call.
	 */
	return encoder->WriteData(data);
}

Bool BoCA::EncoderSndFile::SetOutputFormat(Int n)
{
	return EncoderComponent::SetOutputFormat(n);
}

String BoCA::EncoderSndFile::GetOutputFileExtension() const
{
	return EncoderComponent::GetOutputFileExtension();
}

ConfigLayer *BoCA::EncoderSndFile::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureSndFile();

	return configLayer;
}
