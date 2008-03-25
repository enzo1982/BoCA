 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "bonk_out.h"
#include "config.h"
#include "dllinterface.h"

const String &BoCA::BonkOut::GetComponentSpecs()
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
		    <id>bonk-out</id>				\
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

BoCA::BonkOut::BonkOut()
{
	configLayer = NIL;
}

BoCA::BonkOut::~BonkOut()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::BonkOut::Activate()
{
	if (format.channels > 2)
	{
		Utilities::ErrorMessage("BonkEnc does not support more than 2 channels!");

		errorState = True;

		return False;
	}

	Config	*config = Config::Get();

	packageSize = int(1024.0 * format.rate / 44100) * format.channels * (config->GetIntValue("Bonk", "Lossless", 0) ? 1 : config->GetIntValue("Bonk", "Downsampling", 2)) * (format.bits / 8);

	dataBuffer.Resize(131072);

	encoder	= ex_bonk_encoder_create();

	if ((format.artist != NIL || format.title != NIL) && config->enable_id3v2 && config->enable_id3)
	{
		Buffer<unsigned char>	 id3Buffer;
		Int			 size = format.RenderID3Tag(id3Buffer, 2);

		ex_bonk_encoder_set_id3_data(encoder, id3Buffer, size);
	}

	ex_bonk_encoder_init(encoder,
		(unsigned int) Math::Max(format.length, 0), format.rate, format.channels,
		config->GetIntValue("Bonk", "Lossless", 0), config->GetIntValue("Bonk", "JointStereo", 0),
		config->GetIntValue("Bonk", "Predictor", 32), config->GetIntValue("Bonk", "Lossless", 0) ? 1 : config->GetIntValue("Bonk", "Downsampling", 2),
		int(1024.0 * format.rate / 44100),
		0.05 * (double) config->GetIntValue("Bonk", "Quantization", 8));

	return True;
}

Bool BoCA::BonkOut::Deactivate()
{
	int	 bytes = ex_bonk_encoder_finish(encoder, dataBuffer, dataBuffer.Size());

	if (bytes > dataBuffer.Size())
	{
		dataBuffer.Resize(bytes);

		bytes = ex_bonk_encoder_finish(encoder, dataBuffer, dataBuffer.Size());
	}

	driver->WriteData(dataBuffer, bytes);

	if (format.length == -1)
	{
		int	 sample_count = ex_bonk_encoder_get_sample_count(encoder);

		driver->Seek(ex_bonk_encoder_get_sample_count_offset(encoder));
		driver->WriteData((unsigned char *) &sample_count, 4);
	}

	ex_bonk_encoder_close(encoder);

	return True;
}

Int BoCA::BonkOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	int	 bytes = 0;

	if (format.bits != 16)
	{
		samplesBuffer.Resize(size);

		for (int i = 0; i < size / (format.bits / 8); i++)
		{
			if (format.bits == 8)	samplesBuffer[i] = (data[i] - 128) * 256;
			if (format.bits == 24)	samplesBuffer[i] = (int) (data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256;
			if (format.bits == 32)	samplesBuffer[i] = (int) ((long *) (unsigned char *) data)[i] / 65536;
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

ConfigLayer *BoCA::BonkOut::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureBonk();

	return configLayer;
}

Void BoCA::BonkOut::FreeConfigurationLayer()
{
	if (configLayer != NIL)
	{
		delete configLayer;

		configLayer = NIL;
	}
}
