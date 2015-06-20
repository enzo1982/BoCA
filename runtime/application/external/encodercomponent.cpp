 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
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

String BoCA::AS::EncoderComponentExternal::GetOutputFileExtension() const
{
	return specs->formats.GetFirst()->GetExtensions().GetFirst();
}

Int BoCA::AS::EncoderComponentExternal::GetNumberOfPasses() const
{
	return 1;
}

Bool BoCA::AS::EncoderComponentExternal::IsLossless() const
{
	return specs->formats.GetFirst()->IsLossless();
}

BoCA::ConfigLayer *BoCA::AS::EncoderComponentExternal::GetConfigurationLayer()
{
	if (configLayer == NIL && specs->external_parameters.Length() > 0) configLayer = new ConfigLayerExternal(specs);

	return configLayer;
}

Int BoCA::AS::EncoderComponentExternal::RenderTags(const String &streamURI, const Track &track, Buffer<UnsignedByte> &tagBufferPrepend, Buffer<UnsignedByte> &tagBufferAppend)
{
	Config	*config = Config::Get();

	/* Only render tags if at least artist and title are set.
	 */
	const Info	&info = track.GetInfo();

	if ((track.tracks.Length() == 0 || !config->GetIntValue("Tags", "WriteChapters", True)) &&
	    (info.artist == NIL && info.title == NIL)) return Success();

	/* Loop over supported formats.
	 */
	String	 lcURI = streamURI.ToLower();

	foreach (FileFormat *format, specs->formats)
	{
		foreach (const String &extension, format->GetExtensions())
		{
			if (!lcURI.EndsWith(String(".").Append(extension))) continue;

			/* Render supported tag formats.
			 */
			const Array<TagFormat>	&tagFormats = format->GetTagFormats();

			foreach (const TagFormat &tagFormat, tagFormats)
			{
				AS::Registry		&boca	= AS::Registry::Get();
				AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID(tagFormat.GetTagger());

				if (tagger != NIL)
				{
					foreach (TagSpec *spec, tagger->GetTagSpecs())
					{
						if (spec->GetName() != tagFormat.GetName()) continue;

						if (config->GetIntValue("Tags", String("Enable").Append(tagFormat.GetName().Replace(" ", NIL)), spec->IsDefault()))
						{
							Buffer<UnsignedByte>	 tagBuffer;

							if (tagFormat.GetMode() == TAG_MODE_OTHER) tagger->RenderStreamInfo(streamURI, track);
							else					   tagger->RenderBuffer(tagBuffer, track);

							/* Add tag to prepend or append buffer.
							 */
							if (tagFormat.GetMode() == TAG_MODE_PREPEND)
							{
								tagBufferPrepend.Resize(tagBufferPrepend.Size() + tagBuffer.Size());

								memcpy((UnsignedByte *) tagBufferPrepend + tagBufferPrepend.Size() - tagBuffer.Size(), (UnsignedByte *) tagBuffer, tagBuffer.Size());
							}
							else if (tagFormat.GetMode() == TAG_MODE_APPEND)
							{
								tagBufferAppend.Resize(tagBufferAppend.Size() + tagBuffer.Size());

								memcpy((UnsignedByte *) tagBufferAppend + tagBufferAppend.Size() - tagBuffer.Size(), (UnsignedByte *) tagBuffer, tagBuffer.Size());
							}
						}

						break;
					}

					boca.DeleteComponent(tagger);
				}
			}

			break;
		}
	}

	return Success();
}
