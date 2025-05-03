 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2025 Robert Kausch <robert.kausch@freac.org>
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
#	define WAVE_FORMAT_PCM	      0x0001
#	define WAVE_FORMAT_IEEE_FLOAT 0x0003
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
		    <name>Monkey's Audio Encoder %VERSION%</name>					\
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
		    <input bits=\"8\" signed=\"false\" channels=\"1-32\"/>				\
		    <input bits=\"16-32\" channels=\"1-32\"/>						\
													\
		";

		if (ex_GetLibraryInterfaceVersion() >= 10)
		{
			componentSpecs.Append("								\
													\
			    <input float=\"true\" channels=\"1-32\"/>					\
													\
			");
		}

		componentSpecs.Append("									\
													\
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
		");

		componentSpecs.Replace("%VERSION%", String("v").Append(ex_GetLibraryVersionString()));
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

	/* Get number of threads to use.
	 */
	if (ex_APECompress_SetNumberOfThreads != NIL)
	{
		Bool	 enableParallel	 = config->GetIntValue("Resources", "EnableParallelConversions", True);
		Bool	 enableSuperFast = config->GetIntValue("Resources", "EnableSuperFastMode", True);
		Int	 numberOfThreads = enableParallel && enableSuperFast ? config->GetIntValue("Resources", "NumberOfConversionThreads", 0) : 1;

		if (enableParallel && enableSuperFast && numberOfThreads <= 1) numberOfThreads = CPU().GetNumCores() + (CPU().GetNumLogicalCPUs() - CPU().GetNumCores()) / 2;

		ex_APECompress_SetNumberOfThreads(hAPECompress, 2);//numberOfThreads);
	}

	/* Init encoder using output file name and format info.
	 */
	APE::WAVEFORMATEX	 waveFormat;

	waveFormat.wFormatTag		= format.fp ? WAVE_FORMAT_IEEE_FLOAT : WAVE_FORMAT_PCM;
	waveFormat.nChannels		= format.channels;
	waveFormat.nSamplesPerSec	= format.rate;
	waveFormat.nAvgBytesPerSec	= format.rate * format.channels * (format.bits / 8);
	waveFormat.nBlockAlign		= format.channels * (format.bits / 8);
	waveFormat.wBitsPerSample	= format.bits;
	waveFormat.cbSize		= 0;

	int64 maxAudioBytes = MAX_AUDIO_BYTES_UNKNOWN;

	if (track.length >= 0) maxAudioBytes = track.length * format.channels * (format.bits / 8);

	ex_APECompress_StartW(hAPECompress, track.outputFile, &waveFormat, maxAudioBytes, (config->GetIntValue(ConfigureMAC::ConfigID, "CompressionMode", 2) + 1) * 1000, NIL, CREATE_WAV_HEADER_ON_DECOMPRESSION);

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

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureMAC::ConfigID, "CompressionMode", 2);
	}

	/* Get command line settings.
	 */
	Int	 compressionMode     = config->GetIntValue(ConfigureMAC::ConfigID, "CompressionMode", 2);
	String	 compressionModeName = "fast";

	if	(compressionMode == 1) compressionModeName = "normal";
	else if	(compressionMode == 2) compressionModeName = "high";
	else if	(compressionMode == 3) compressionModeName = "extra";
	else if	(compressionMode == 4) compressionModeName = "insane";

	if (config->GetIntValue(encoderID, "Set Compression mode", False)) compressionModeName = config->GetStringValue(encoderID, "Compression mode", compressionModeName).ToLower();

	/* Set configuration values.
	 */
	if	(compressionModeName == "fast"	) compressionMode = 0;
	else if (compressionModeName == "normal") compressionMode = 1;
	else if (compressionModeName == "high"	) compressionMode = 2;
	else if (compressionModeName == "extra" ) compressionMode = 3;
	else if (compressionModeName == "insane") compressionMode = 4;

	config->SetIntValue(ConfigureMAC::ConfigID, "CompressionMode", compressionMode);

	return True;
}

ConfigLayer *BoCA::EncoderMAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMAC();

	return configLayer;
}
