 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
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

#include "bladeenc.h"
#include "config.h"

const String &BoCA::EncoderBlade::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (bladedll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>BladeEnc MP3 Encoder</name>				\
		    <version>1.0</version>					\
		    <id>bladeenc-enc</id>					\
		    <type>encoder</type>					\
		    <format>							\
		      <name>MPEG 1 Audio Layer 3</name>				\
		      <extension>mp3</extension>				\
		      <tag id=\"id3v1-tag\" mode=\"append\">ID3v1</tag>		\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>	\
		    </format>							\
		  </component>							\
										\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadBladeDLL();
}

Void smooth::DetachDLL()
{
	FreeBladeDLL();
}

BoCA::EncoderBlade::EncoderBlade() : beConfig()
{
	configLayer = NIL;

	handle	    = NIL;
}

BoCA::EncoderBlade::~EncoderBlade()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderBlade::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info = track.GetInfo();

	if (format.rate != 32000 && format.rate != 44100 && format.rate != 48000)
	{
		errorString = "Bad sampling rate! BladeEnc supports only 32, 44.1 or 48 kHz.";
		errorState  = True;

		return False;
	}

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

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

	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v2", True))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->RenderBuffer(id3Buffer, track);

			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Bool BoCA::EncoderBlade::Deactivate()
{
	Config		*config = Config::Get();
	const Info	&info = track.GetInfo();

	unsigned long	 bytes = 0;

	ex_beDeinitStream(handle, outBuffer, &bytes);

	driver->WriteData(outBuffer, bytes);

	ex_beCloseStream(handle);

	if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v1", False))
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

		if (tagger != NIL)
		{
			Buffer<unsigned char>	 id3Buffer;

			tagger->RenderBuffer(id3Buffer, track);

			driver->WriteData(id3Buffer, id3Buffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Int BoCA::EncoderBlade::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	unsigned long	 bytes = 0;

	const Format	&format = track.GetFormat();

	if (format.bits != 16)
	{
		for (int i = 0; i < size / (format.bits / 8); i++)
		{
			if	(format.bits ==  8) samplesBuffer[i] =	     (				  data [i] - 128) * 256;
			else if (format.bits == 32) samplesBuffer[i] = (int) (((long *) (unsigned char *) data)[i]	  / 65536);

			else if (format.bits == 24) samplesBuffer[i] = (int) ((data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256);
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

ConfigLayer *BoCA::EncoderBlade::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureBlade();

	return configLayer;
}
