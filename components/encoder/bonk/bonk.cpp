 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
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
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Bonk Audio Encoder</name>		\
		    <version>1.0</version>			\
		    <id>bonk-enc</id>				\
		    <type>encoder</type>			\
		    <format>					\
		      <name>Bonk Audio Files</name>		\
		      <extension>bonk</extension>		\
		    </format>					\
		  </component>					\
								\
		";
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
}

BoCA::EncoderBonk::~EncoderBonk()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderBonk::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info = track.GetInfo();

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	Config	*config = Config::Get();

	packageSize = int(1024.0 * format.rate / 44100) * format.channels * (config->GetIntValue("Bonk", "Lossless", 0) ? 1 : config->GetIntValue("Bonk", "Downsampling", 2)) * (format.bits / 8);

	dataBuffer.Resize(131072);

	encoder	= ex_bonk_encoder_create();

	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v2", True))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->RenderBuffer(id3Buffer, track);

			ex_bonk_encoder_set_id3_data(encoder, id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);

			dataBuffer.Resize(dataBuffer.Size() + id3Buffer.Size());
		}
	}

	ex_bonk_encoder_init(encoder,
		(unsigned int) Math::Max(track.length * format.channels, (Int64) 0), format.rate, format.channels,
		config->GetIntValue("Bonk", "Lossless", 0), config->GetIntValue("Bonk", "JointStereo", 0),
		config->GetIntValue("Bonk", "Predictor", 32), config->GetIntValue("Bonk", "Lossless", 0) ? 1 : config->GetIntValue("Bonk", "Downsampling", 2),
		int(1024.0 * format.rate / 44100),
		0.05 * (double) config->GetIntValue("Bonk", "Quantization", 8));

	return True;
}

Bool BoCA::EncoderBonk::Deactivate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	Int	 bytes = ex_bonk_encoder_finish(encoder, dataBuffer, dataBuffer.Size());

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

	return True;
}

Int BoCA::EncoderBonk::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Convert samples to 16 bit.
	 */
	const Format	&format = track.GetFormat();
	int		 bytes	= 0;

	if (format.bits != 16)
	{
		samplesBuffer.Resize(size);

		for (Int i = 0; i < size / (format.bits / 8); i++)
		{
			if	(format.bits ==  8				) samplesBuffer[i] =	   (				data [i] - 128) * 256;
			else if (format.bits == 32				) samplesBuffer[i] = (int) (((long *) (unsigned char *) data)[i]	/ 65536);

			else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[i] = (int) ((data[3 * i    ] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256);
			else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[i] = (int) ((data[3 * i + 2] + 256 * data[3 * i + 1] + 65536 * data[3 * i    ] - (data[3 * i    ] & 128 ? 16777216 : 0)) / 256);
		}

		bytes = ex_bonk_encoder_encode_packet(encoder, samplesBuffer, size / (format.bits / 8), dataBuffer, dataBuffer.Size());
	}
	else
	{
		bytes = ex_bonk_encoder_encode_packet(encoder, (short *) (unsigned char *) data, size / (format.bits / 8), dataBuffer, dataBuffer.Size());
	}

	driver->WriteData(dataBuffer, bytes);

	return bytes;
}

ConfigLayer *BoCA::EncoderBonk::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureBonk();

	return configLayer;
}
