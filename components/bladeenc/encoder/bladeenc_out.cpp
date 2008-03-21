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

#include <time.h>

#include "bladeenc_out.h"
#include "config.h"
#include "dllinterface.h"

const String &BoCA::BladeOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (bladedll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>BladeEnc MP3 Encoder</name>		\
		    <version>1.0</version>			\
		    <id>bladeenc-out</id>			\
		    <type>encoder</type>			\
		    <format>					\
		      <name>MPEG 1 Audio Layer 3</name>		\
		      <extension>mp3</extension>		\
		    </format>					\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

ConfigureBladeEnc	*configLayer = NIL;

Void smooth::AttachDLL(Void *instance)
{
	LoadBladeDLL();

	configLayer = new ConfigureBladeEnc();
}

Void smooth::DetachDLL()
{
	Object::DeleteObject(configLayer);

	FreeBladeDLL();
}

BoCA::BladeOut::BladeOut()
{
}

BoCA::BladeOut::~BladeOut()
{
}

Bool BoCA::BladeOut::Activate()
{
	if (format.rate != 32000 && format.rate != 44100 && format.rate != 48000)
	{
		Utilities::ErrorMessage("Bad sampling rate! BladeEnc supports only 32, 44.1 or 48kHz.");

		errorState = True;

		return False;
	}

	if (format.channels > 2)
	{
		Utilities::ErrorMessage("BonkEnc does not support more than 2 channels!");

		errorState = True;

		return False;
	}

	Config	*config = Config::Get();

	beConfig.dwConfig			= BE_CONFIG_MP3;
	beConfig.format.mp3.dwSampleRate	= format.rate;

	if (format.channels == 2)
	{
		if (config->GetIntValue("BladeEnc", "DualChannel", 0))	beConfig.format.mp3.byMode = BE_MP3_MODE_DUALCHANNEL;
		else							beConfig.format.mp3.byMode = BE_MP3_MODE_STEREO;
	}
	else if (format.channels == 1)
	{
		beConfig.format.mp3.byMode = BE_MP3_MODE_MONO;
	}

	beConfig.format.mp3.wBitrate	= config->GetIntValue("BladeEnc", "Bitrate", 0);
	beConfig.format.mp3.bCopyright	= config->GetIntValue("BladeEnc", "Copyright", 0);
	beConfig.format.mp3.bCRC	= config->GetIntValue("BladeEnc", "CRC", 0);
	beConfig.format.mp3.bOriginal	= config->GetIntValue("BladeEnc", "Original", 1);
	beConfig.format.mp3.bPrivate	= config->GetIntValue("BladeEnc", "Private", 0);

	unsigned long	 bufferSize	= 0;
	unsigned long	 samplesSize	= 0;

	ex_beInitStream(&beConfig, &samplesSize, &bufferSize, &handle);

	outBuffer.Resize(bufferSize);
	samplesBuffer.Resize(samplesSize);

	packageSize = samplesSize * (format.bits / 8);

	if ((format.artist != NIL || format.title != NIL) && config->enable_id3v2 && config->enable_id3)
	{
		Buffer<unsigned char>	 id3Buffer;
		Int			 size = format.RenderID3Tag(id3Buffer, 2);

		driver->WriteData(id3Buffer, size);
	}

	return True;
}

Bool BoCA::BladeOut::Deactivate()
{
	Config	*config = Config::Get();

	unsigned long	 bytes = 0;

	ex_beDeinitStream(handle, outBuffer, &bytes);

	driver->WriteData(outBuffer, bytes);

	ex_beCloseStream(handle);

	if ((format.artist != NIL || format.title != NIL) && config->enable_id3v1 && config->enable_id3)
	{
		Buffer<unsigned char>	 id3Buffer;
		Int			 size = format.RenderID3Tag(id3Buffer, 1);

		driver->WriteData(id3Buffer, size);
	}

	return True;
}

Int BoCA::BladeOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	unsigned long	 bytes = 0;

	if (format.bits != 16)
	{
		for (int i = 0; i < size / (format.bits / 8); i++)
		{
			if (format.bits == 8)	samplesBuffer[i] = (data[i] - 128) * 256;
			if (format.bits == 24)	samplesBuffer[i] = (int) (data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256;
			if (format.bits == 32)	samplesBuffer[i] = (int) ((long *) (unsigned char *) data)[i] / 65536;
		}

		ex_beEncodeChunk(handle, size / (format.bits / 8), samplesBuffer, outBuffer, &bytes);
	}
	else
	{
		ex_beEncodeChunk(handle, size / (format.bits / 8), (short *) (unsigned char *) data, outBuffer, &bytes);
	}

	driver->WriteData(outBuffer, bytes);

	return bytes;
}

ConfigLayer *BoCA::BladeOut::GetConfigurationLayer()
{
	return configLayer;
}
