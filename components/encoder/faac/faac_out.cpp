 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2009 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "faac_out.h"
#include "config.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::FAACOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (faacdll != NIL)
	{
		componentSpecs = "					\
									\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>		\
		  <component>						\
		    <name>FAAC MP4/AAC Encoder</name>			\
		    <version>1.0</version>				\
		    <id>faac-out</id>					\
		    <type>encoder</type>				\
									\
		";

		if (mp4v2dll != NIL)
		{
			componentSpecs.Append("				\
									\
			    <format>					\
			      <name>MP4 Audio Files</name>		\
			      <extension>m4a</extension>		\
			      <extension>m4b</extension>		\
			      <extension>m4r</extension>		\
			      <extension>mp4</extension>		\
			      <extension>3gp</extension>		\
			      <tag mode=\"other\">MP4Metadata</tag>	\
			    </format>					\
									\
			");
		}

		componentSpecs.Append("					\
									\
		    <format>						\
		      <name>Advanced Audio Files</name>			\
		      <extension>aac</extension>			\
		      <tag mode=\"prepend\">ID3v2</tag>			\
		    </format>						\
		  </component>						\
									\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadFAACDLL();
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeFAACDLL();
	FreeMP4v2DLL();
}

BoCA::FAACOut::FAACOut()
{
	configLayer = NIL;
}

BoCA::FAACOut::~FAACOut()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::FAACOut::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info = track.GetInfo();

	if (format.channels > 2)
	{
		Utilities::ErrorMessage("BonkEnc does not support more than 2 channels!");

		errorState = True;

		return False;
	}

	Config	*config = Config::Get();

	unsigned long	 samplesSize	= 0;
	unsigned long	 bufferSize	= 0;

	handle = ex_faacEncOpen(format.rate, format.channels, &samplesSize, &bufferSize);

	outBuffer.Resize(bufferSize);
	samplesBuffer.Resize(samplesSize);

	fConfig = ex_faacEncGetCurrentConfiguration(handle);

	fConfig->mpegVersion	= config->GetIntValue("FAAC", "MP4Container", 1) ? MPEG4 : config->GetIntValue("FAAC", "MPEGVersion", 0);
	fConfig->aacObjectType	= config->GetIntValue("FAAC", "AACType", 2);
	fConfig->allowMidside	= config->GetIntValue("FAAC", "AllowJS", 1);
	fConfig->useTns		= config->GetIntValue("FAAC", "UseTNS", 0);
	fConfig->bandWidth	= config->GetIntValue("FAAC", "BandWidth", 16000);

	if (config->GetIntValue("FAAC", "MP4Container", 1)) fConfig->outputFormat	= 0; // Raw AAC frame headers

	if (config->GetIntValue("FAAC", "SetQuality", 1))   fConfig->quantqual		= config->GetIntValue("FAAC", "AACQuality", 100);
	else						    fConfig->bitRate		= config->GetIntValue("FAAC", "Bitrate", 64) * 1000;

	if (format.bits == 8)	fConfig->inputFormat	= FAAC_INPUT_16BIT;
	if (format.bits == 16)	fConfig->inputFormat	= FAAC_INPUT_16BIT;
	if (format.bits == 24)	fConfig->inputFormat	= FAAC_INPUT_32BIT;
	if (format.bits == 32)	fConfig->inputFormat	= FAAC_INPUT_FLOAT;

	ex_faacEncSetConfiguration(handle, fConfig);

	if (config->GetIntValue("FAAC", "MP4Container", 1))
	{
		mp4File		= ex_MP4CreateEx(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), 0, 0, 1, 1, NIL, 0, NIL, 0);
		mp4Track	= ex_MP4AddAudioTrack(mp4File, format.rate, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);	

		ex_MP4SetAudioProfileLevel(mp4File, 0x0F);

		unsigned char	*buffer = NIL;

		ex_faacEncGetDecoderSpecificInfo(handle, &buffer, &bufferSize);

		ex_MP4SetTrackESConfiguration(mp4File, mp4Track, (uint8_t *) buffer, bufferSize);

		frameSize	= samplesSize / format.channels;

		totalSamples	= 0;
		encodedSamples	= 0;
		delaySamples	= frameSize;
	}

	packageSize	= samplesSize * (format.bits / 8);

	if (!config->GetIntValue("FAAC", "MP4Container", 1))
	{
		if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("FAAC", "AllowID3v2", 0))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) AS::Registry::Get().CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	return True;
}

Bool BoCA::FAACOut::Deactivate()
{
	Config		*config = Config::Get();

	unsigned long	 bytes = 0;

	do
	{
		bytes = ex_faacEncEncode(handle, NULL, 0, outBuffer, outBuffer.Size());

		if (config->GetIntValue("FAAC", "MP4Container", 1))
		{
			if (bytes > 0)
			{
				Int		 samplesLeft	= totalSamples - encodedSamples + delaySamples;
				MP4Duration	 dur		= samplesLeft > frameSize ? frameSize : samplesLeft;
				MP4Duration	 ofs		= encodedSamples > 0 ? 0 : delaySamples;

				ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, bytes, dur, ofs, true);

				encodedSamples += dur;
			}
		}
		else
		{
			driver->WriteData(outBuffer, bytes);
		}
	}
	while (bytes > 0);

	ex_faacEncClose(handle);

	if (config->GetIntValue("FAAC", "MP4Container", 1))
	{
		ex_MP4Close(mp4File);

		/* Write metadata to file
		 */
		if (config->GetIntValue("Tags", "EnableMP4Metadata", True))
		{
			const Info	&info = track.GetInfo();

			if (info.artist != NIL || info.title != NIL)
			{
				AS::Registry		&boca = AS::Registry::Get();
				AS::TaggerComponent	*tagger = (AS::TaggerComponent *) AS::Registry::Get().CreateComponentByID("mp4-tag");

				if (tagger != NIL)
				{
					tagger->RenderStreamInfo(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), track);

					boca.DeleteComponent(tagger);
				}
			}
		}

		/* Stream contents of created MP4 file to output driver
		 */
		InStream		 in(STREAM_FILE, Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), IS_READONLY);
		Buffer<UnsignedByte>	 buffer(1024);
		Int			 bytesLeft = in.Size();

		while (bytesLeft)
		{
			driver->WriteData((UnsignedByte *) in.InputData(buffer, Math::Min(1024, bytesLeft)), Math::Min(1024, bytesLeft));

			bytesLeft -= Math::Min(1024, bytesLeft);
		}

		in.Close();

		File(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out")).Delete();
	}

	return True;
}

Int BoCA::FAACOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	Config	*config = Config::Get();

	const Format	&format = track.GetFormat();

	unsigned long	 bytes = 0;
	Int		 samplesRead = size / (format.bits / 8);

	totalSamples += samplesRead / format.channels;

	if (format.bits != 16)
	{
		for (int i = 0; i < samplesRead; i++)
		{
			if (format.bits ==  8) ((short *) (int32_t *) samplesBuffer)[i] = (data[i] - 128) * 256;
			if (format.bits == 24) samplesBuffer[i] = data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0);
			if (format.bits == 32) ((float *) (int32_t *) samplesBuffer)[i] = (1.0 / 65536) * ((int32_t *) (unsigned char *) data)[i];
		}

		bytes = ex_faacEncEncode(handle, samplesBuffer, samplesRead, outBuffer, outBuffer.Size());
	}
	else
	{
		bytes = ex_faacEncEncode(handle, (int32_t *) (unsigned char *) data, samplesRead, outBuffer, outBuffer.Size());
	}

	if (config->GetIntValue("FAAC", "MP4Container", 1))
	{
		if (bytes > 0)
		{
			Int		 samplesLeft	= totalSamples - encodedSamples + delaySamples;
			MP4Duration	 dur		= samplesLeft > frameSize ? frameSize : samplesLeft;
			MP4Duration	 ofs		= encodedSamples > 0 ? 0 : delaySamples;

			ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, bytes, dur, ofs, true);

			encodedSamples += dur;
		}
	}
	else
	{
		driver->WriteData(outBuffer, bytes);
	}

	return bytes;
}

String BoCA::FAACOut::GetOutputFileExtension()
{
	Config	*config = Config::Get();

	if (config->GetIntValue("FAAC", "MP4Container", 1))
	{
		switch (config->GetIntValue("FAAC", "MP4FileExtension", 0))
		{
			default:
			case  0: return "m4a";
			case  1: return "m4b";
			case  2: return "m4r";
			case  3: return "mp4";
		}
	}

	return "aac";
}

ConfigLayer *BoCA::FAACOut::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureFAAC();

	return configLayer;
}

Void BoCA::FAACOut::FreeConfigurationLayer()
{
	if (configLayer != NIL)
	{
		delete configLayer;

		configLayer = NIL;
	}
}
