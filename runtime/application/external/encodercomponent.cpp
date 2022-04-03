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

#include <boca/application/external/encodercomponent.h>
#include <boca/application/external/configlayer.h>

#include <boca/application/registry.h>
#include <boca/application/taggercomponent.h>

#include <boca/common/config.h>

BoCA::AS::EncoderComponentExternal::EncoderComponentExternal(ComponentSpecs *specs) : EncoderComponent(specs)
{
	configuration	= NIL;

	configLayer	= NIL;
}

BoCA::AS::EncoderComponentExternal::~EncoderComponentExternal()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::AS::EncoderComponentExternal::SetOutputFormat(Int n)
{
	return True;
}

Bool BoCA::AS::EncoderComponentExternal::SetAudioTrackInfo(const Track &track)
{
	this->track  = track;
	this->format = track.GetFormat();

	return True;
}

String BoCA::AS::EncoderComponentExternal::GetOutputFileExtension() const
{
	return specs->formats.GetFirst()->GetExtensions().GetFirst();
}

File BoCA::AS::EncoderComponentExternal::GetCompanionFile(const String &file) const
{
	String	 companionExt = specs->formats.GetFirst()->GetCompanionExtensions().GetFirst();

	if (companionExt == NIL) return File();

	return file.Head(file.FindLast(".") + 1).Append(companionExt);
}

Bool BoCA::AS::EncoderComponentExternal::IsLossless() const
{
	return specs->formats.GetFirst()->IsLossless();
}

Bool BoCA::AS::EncoderComponentExternal::Activate()
{
	/* Setup format converter and adjust track info.
	 */
	Format	 target = FormatConverter::GetBestTargetFormat(track.GetFormat(), this);

	if (target.bits == 8)	 target.sign = False;
	if (target.fp	== True) target.fp   = False;

	converter = new FormatConverter(track.GetFormat(), target);
	format	  = target;

	if (converter->GetErrorState())
	{
		errorState  = True;
		errorString = converter->GetErrorString();

		delete converter;

		return False;
	}

	track.AdjustSampleCounts(format);
	track.SetFormat(format);

	return True;
}

Bool BoCA::AS::EncoderComponentExternal::Deactivate()
{
	/* Flush and clean up format converter.
	 */
	Buffer<UnsignedByte>	 buffer;

	converter->Finish(buffer);

	delete converter;

	converter = NIL;

	if (buffer.Size() != 0) WriteData(buffer);

	return True;
}

Int BoCA::AS::EncoderComponentExternal::ProcessData(Buffer<UnsignedByte> &buffer)
{
	if (converter != NIL) converter->Transform(buffer);

	/* Calculate MD5 if requested.
	 */
	if (calculateMD5) md5.Feed(buffer);

	return buffer.Size();
}

BoCA::ConfigLayer *BoCA::AS::EncoderComponentExternal::GetConfigurationLayer()
{
	if (configLayer == NIL && specs->parameters.Length() > 0) configLayer = new ConfigLayerExternal(specs);

	return configLayer;
}

const BoCA::Config *BoCA::AS::EncoderComponentExternal::GetConfiguration() const
{
	if (configuration != NIL) return configuration;
	else			  return Config::Get();
}

Bool BoCA::AS::EncoderComponentExternal::SetConfiguration(const Config *nConfiguration)
{
	configuration = nConfiguration;

	return True;
}

Int BoCA::AS::EncoderComponentExternal::RenderTags(const String &streamURI, const Track &track, Buffer<UnsignedByte> &tagBufferPrepend, Buffer<UnsignedByte> &tagBufferAppend)
{
	const Config	*config = GetConfiguration();

	/* Only render tags if at least artist and title are set.
	 */
	const Info	&info = track.GetInfo();

	if ((track.tracks.Length() == 0 || !config->GetIntValue("Tags", "WriteChapters", True)) && !info.HasBasicInfo()) return Success();

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
					tagger->SetConfiguration(GetConfiguration());

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
