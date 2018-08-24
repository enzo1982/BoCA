 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

#include "bonk.h"
#include "config.h"

const String &BoCA::EncoderBonk::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (bonkdll != NIL)
	{
		componentSpecs = "										\
														\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>							\
		  <component>											\
		    <name>Bonk Audio Encoder %VERSION%</name>							\
		    <version>1.0</version>									\
		    <id>bonk-enc</id>										\
		    <type>encoder</type>									\
		    <format>											\
		      <name>Bonk Audio Files</name>								\
		      <extension>bonk</extension>								\
		    </format>											\
		    <input bits=\"16\" channels=\"1-2\"/>							\
		    <parameters>										\
		      <range name=\"Quantization factor\" argument=\"-q %VALUE\" default=\"0.4\" step=\"0.05\">	\
			<min alias=\"0\">0</min>								\
			<max alias=\"2\">2</max>								\
		      </range>											\
		      <range name=\"Predictor size\" argument=\"-s %VALUE\" default=\"32\">			\
			<min alias=\"min\">0</min>								\
			<max alias=\"max\">512</max>								\
		      </range>											\
		      <range name=\"Downsampling ratio\" argument=\"-r %VALUE\" default=\"2\">			\
			<min alias=\"1\">1</min>								\
			<max alias=\"10\">10</max>								\
		      </range>											\
		      <switch name=\"Use Joint Stereo\" argument=\"--js\"/>					\
		      <switch name=\"Use lossless compression\" argument=\"--lossless\"/>			\
		    </parameters>										\
		  </component>											\
														\
		";

		componentSpecs.Replace("%VERSION%", String("v").Append(ex_bonk_get_version_string()));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadBonkDLL();
}

Void smooth::DetachDLL()
{
	FreeBonkDLL();
}

BoCA::EncoderBonk::EncoderBonk()
{
	configLayer = NIL;

	encoder	    = NIL;

	frameSize   = 0;

	config	    = Config::Copy(GetConfiguration());

	ConvertArguments(config);
}

BoCA::EncoderBonk::~EncoderBonk()
{
	Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderBonk::IsLossless() const
{
	return config->GetIntValue(ConfigureBonk::ConfigID, "Lossless", 0);
}

Bool BoCA::EncoderBonk::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info	= track.GetInfo();

	/* Configure and create Bonk encoder.
	 */
	frameSize = Int(1024.0 * format.rate / 44100) * format.channels * (config->GetIntValue(ConfigureBonk::ConfigID, "Lossless", 0) ? 1 : config->GetIntValue(ConfigureBonk::ConfigID, "Downsampling", 2));

	dataBuffer.Resize(131072);

	encoder	= ex_bonk_encoder_create();

	/* Write ID3v2 tag if requested.
	 */
	if (config->GetIntValue("Tags", "EnableID3v2", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(config);
			tagger->RenderBuffer(id3Buffer, track);

			ex_bonk_encoder_set_id3_data(encoder, id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);

			dataBuffer.Resize(dataBuffer.Size() + id3Buffer.Size());
		}
	}

	/* Init Bonk encoder.
	 */
	ex_bonk_encoder_init(encoder,
		(unsigned int) Math::Max(track.length * format.channels, (Int64) 0), format.rate, format.channels,
		config->GetIntValue(ConfigureBonk::ConfigID, "Lossless", 0), config->GetIntValue(ConfigureBonk::ConfigID, "JointStereo", 0),
		config->GetIntValue(ConfigureBonk::ConfigID, "Predictor", 32), config->GetIntValue(ConfigureBonk::ConfigID, "Lossless", 0) ? 1 : config->GetIntValue(ConfigureBonk::ConfigID, "Downsampling", 2),
		int(1024.0 * format.rate / 44100),
		0.05 * (double) config->GetIntValue(ConfigureBonk::ConfigID, "Quantization", 8));

	return True;
}

Bool BoCA::EncoderBonk::Deactivate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Output remaining samples.
	 */
	Int	 bytes = ex_bonk_encoder_encode_packet(encoder, samplesBuffer, samplesBuffer.Size(), dataBuffer, dataBuffer.Size());

	driver->WriteData(dataBuffer, bytes);

	/* Finalize stream.
	 */
	bytes = ex_bonk_encoder_finish(encoder, dataBuffer, dataBuffer.Size());

	if (bytes > dataBuffer.Size())
	{
		dataBuffer.Resize(bytes);

		bytes = ex_bonk_encoder_finish(encoder, dataBuffer, dataBuffer.Size());
	}

	driver->WriteData(dataBuffer, bytes);

	if (track.length == -1)
	{
		Int	 sample_count = ex_bonk_encoder_get_sample_count(encoder);

		driver->Seek(ex_bonk_encoder_get_sample_count_offset(encoder));

		if (endianness == EndianLittle)	for (Int i = 0; i <= 3; i++) driver->WriteData(((unsigned char *) &sample_count) + i, 1);
		else				for (Int i = 3; i >= 0; i--) driver->WriteData(((unsigned char *) &sample_count) + i, 1);
	}

	ex_bonk_encoder_close(encoder);

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True) && config->GetIntValue("Tags", "EnableID3v2", True))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->SetConfiguration(config);
			tagger->RenderBuffer(id3Buffer, track);

			driver->Seek(2);
			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Int BoCA::EncoderBonk::WriteData(Buffer<UnsignedByte> &data)
{
	/* Copy data to samples buffer.
	 */
	Int	 samples = data.Size() / 2;

	samplesBuffer.Resize(samplesBuffer.Size() + samples);

	memcpy(samplesBuffer + samplesBuffer.Size() - samples, data, data.Size());

	/* Output samples to encoder.
	 */
	Int	 dataLength	 = 0;
	Int	 framesProcessed = 0;

	while (samplesBuffer.Size() - framesProcessed * frameSize >= frameSize)
	{
		Int	 bytes = ex_bonk_encoder_encode_packet(encoder, samplesBuffer + framesProcessed * frameSize, frameSize, dataBuffer, dataBuffer.Size());

		dataLength += bytes;

		driver->WriteData(dataBuffer, bytes);

		framesProcessed++;
	}

	memmove(samplesBuffer, samplesBuffer + framesProcessed * frameSize, sizeof(short) * (samplesBuffer.Size() - framesProcessed * frameSize));

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * frameSize);

	return dataLength;
}

Bool BoCA::EncoderBonk::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "bonk-enc";

	/* Get command line settings.
	 */
	Int	 quantization = 8;
	Int	 predictor    = 32;
	Int	 downsampling = 2;

	if (config->GetIntValue(encoderID, "Set Quantization factor", False)) quantization = config->GetIntValue(encoderID, "Quantization factor", quantization);
	if (config->GetIntValue(encoderID, "Set Predictor size", False))      predictor    = config->GetIntValue(encoderID, "Predictor size", predictor);
	if (config->GetIntValue(encoderID, "Set Downsampling ratio", False))  downsampling = config->GetIntValue(encoderID, "Downsampling ratio", downsampling);

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureBonk::ConfigID, "JointStereo", config->GetIntValue(encoderID, "Use Joint Stereo", False));
	config->SetIntValue(ConfigureBonk::ConfigID, "Lossless", config->GetIntValue(encoderID, "Use lossless compression", False));

	config->SetIntValue(ConfigureBonk::ConfigID, "Quantization", Math::Max(0, Math::Min(40, quantization)));
	config->SetIntValue(ConfigureBonk::ConfigID, "Predictor", Math::Max(0, Math::Min(512, predictor)));
	config->SetIntValue(ConfigureBonk::ConfigID, "Downsampling", Math::Max(0, Math::Min(10, downsampling)));

	return True;
}

ConfigLayer *BoCA::EncoderBonk::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureBonk();

	return configLayer;
}
