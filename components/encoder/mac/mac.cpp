 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

#include "mac.h"
#include "config.h"

#ifndef WAVE_FORMAT_PCM
#	define WAVE_FORMAT_PCM 1
#endif

using namespace smooth::IO;

const String &BoCA::EncoderMAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (macdll != NIL)
	{
		componentSpecs = "									\
													\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
		  <component>										\
		    <name>Monkey's Audio Encoder</name>							\
		    <version>1.0</version>								\
		    <id>mac-enc</id>									\
		    <type>encoder</type>								\
		    <format>										\
		      <name>Monkey's Audio</name>							\
		      <lossless>true</lossless>								\
		      <extension>ape</extension>							\
		      <extension>mac</extension>							\
		      <tag id=\"apev2-tag\" mode=\"append\">APEv2</tag>					\
		    </format>										\
		    <input bits=\"8\" signed=\"false\" channels=\"1-8\"/>				\
		    <input bits=\"16-32\" channels=\"1-8\"/>						\
		    <parameters>									\
		      <selection name=\"Compression mode\" argument=\"-m %VALUE\" default=\"high\">	\
			<option alias=\"Fast\">fast</option>						\
			<option alias=\"Normal\">normal</option>					\
			<option alias=\"High\">high</option>						\
			<option alias=\"Extra high\">extra</option>					\
			<option alias=\"Insane\">insane</option>					\
		      </selection>									\
		    </parameters>									\
		  </component>										\
													\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadMACDLL();
}

Void smooth::DetachDLL()
{
	FreeMACDLL();
}

BoCA::EncoderMAC::EncoderMAC()
{
	configLayer  = NIL;
	config	     = NIL;

	hAPECompress = NIL;
}

BoCA::EncoderMAC::~EncoderMAC()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderMAC::Activate()
{
	const Format	&format = track.GetFormat();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	/* Close output file as it will be written directly by APE.
	 */
	driver->Close();

	/* Create encoder and retrieve handle.
	 */
	int	 nRetVal = 0;

	hAPECompress = ex_APECompress_Create(&nRetVal);

	/* Init encoder using output file name and format info.
	 */
	APE::WAVEFORMATEX	 waveFormat;

	waveFormat.wFormatTag		= WAVE_FORMAT_PCM;
	waveFormat.nChannels		= format.channels;
	waveFormat.nSamplesPerSec	= format.rate;
	waveFormat.nAvgBytesPerSec	= format.rate * format.channels * (format.bits / 8);
	waveFormat.nBlockAlign		= format.channels * (format.bits / 8);
	waveFormat.wBitsPerSample	= format.bits;
	waveFormat.cbSize		= 0;

	ex_APECompress_StartW(hAPECompress, track.outputFile, &waveFormat, MAX_AUDIO_BYTES_UNKNOWN, (config->GetIntValue(ConfigureMAC::ConfigID, "CompressionMode", 2) + 1) * 1000, NIL, CREATE_WAV_HEADER_ON_DECOMPRESSION);

	return True;
}

Bool BoCA::EncoderMAC::Deactivate()
{
	/* Finish encoding and destroy the encoder.
	 */
	ex_APECompress_Finish(hAPECompress, NIL, 0, 0);
	ex_APECompress_Destroy(hAPECompress);

	/* Write APEv2 tag if requested.
	 */
	const Info	&info = track.GetInfo();

	if (config->GetIntValue("Tags", "EnableAPEv2", True) && info.HasBasicInfo())
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("apev2-tag");

		if (tagger != NIL)
		{
			OutStream		 out(STREAM_FILE, track.outputFile, OS_APPEND);
			Buffer<unsigned char>	 tagBuffer;

			tagger->SetConfiguration(config);
			tagger->RenderBuffer(tagBuffer, track);

			out.OutputData(tagBuffer, tagBuffer.Size());

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Int BoCA::EncoderMAC::WriteData(Buffer<UnsignedByte> &data)
{
	/* Hand data over to the encoder.
	 */
	ex_APECompress_AddData(hAPECompress, data, data.Size());

	return data.Size();
}

Bool BoCA::EncoderMAC::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "mac-enc";

	/* Get command line settings.
	 */
	String	 mode = "high";

	if (config->GetIntValue(encoderID, "Set Compression mode", False)) mode = config->GetStringValue(encoderID, "Compression mode", mode).ToLower();

	/* Set configuration values.
	 */
	Int	 compressionMode = 2;

	if	(mode == "fast"	 ) compressionMode = 0;
	else if (mode == "normal") compressionMode = 1;
	else if (mode == "high"	 ) compressionMode = 2;
	else if (mode == "extra" ) compressionMode = 3;
	else if (mode == "insane") compressionMode = 4;

	config->SetIntValue(ConfigureMAC::ConfigID, "CompressionMode", compressionMode);

	return True;
}

ConfigLayer *BoCA::EncoderMAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMAC();

	return configLayer;
}
