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

#include "voaacenc.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderVOAAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (voaacencdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>VisualOn AAC Encoder</name>					\
		    <version>1.0</version>						\
		    <id>voaacenc-enc</id>						\
		    <type>encoder</type>						\
											\
		";

		if (mp4v2dll != NIL)
		{
			componentSpecs.Append("						\
											\
			    <format>							\
			      <name>MP4 Audio Files</name>				\
			      <extension>m4a</extension>				\
			      <extension>m4b</extension>				\
			      <extension>m4r</extension>				\
			      <extension>mp4</extension>				\
			      <extension>3gp</extension>				\
			      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
			    </format>							\
											\
			");
		}

		componentSpecs.Append("							\
											\
		    <format>								\
		      <name>Advanced Audio Files</name>					\
		      <extension>aac</extension>					\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>		\
		    </format>								\
		  </component>								\
											\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadVOAACEncDLL();
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeVOAACEncDLL();
	FreeMP4v2DLL();
}

BoCA::EncoderVOAAC::EncoderVOAAC()
{
	configLayer    = NIL;

	mp4File	       = NIL;
	handle	       = NIL;

	mp4Track       = -1;
	sampleId       = 0;

	frameSize      = 0;

	totalSamples   = 0;
	encodedSamples = 0;
	delaySamples   = frameSize;

	memset(&memOperator, 0, sizeof(memOperator));
	memset(&userData, 0, sizeof(userData));

	ex_voGetAACEncAPI(&api);
}

BoCA::EncoderVOAAC::~EncoderVOAAC()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderVOAAC::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info = track.GetInfo();

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	if (GetSampleRateIndex(format.rate) == -1)
	{
		errorString = "Bad sampling rate! The selected sampling rate is not supported.";
		errorState  = True;

		return False;
	}

	Config	*config = Config::Get();

	unsigned long	 samplesSize = 1024 * format.channels;
	unsigned long	 bufferSize  = samplesSize * 4;

	outBuffer.Resize(bufferSize);
	samplesBuffer.Resize(samplesSize);

	memOperator.Alloc = ex_cmnMemAlloc;
	memOperator.Copy  = ex_cmnMemCopy;
	memOperator.Free  = ex_cmnMemFree;
	memOperator.Set	  = ex_cmnMemSet;
	memOperator.Check = ex_cmnMemCheck;

	userData.memflag  = VO_IMF_USERMEMOPERATOR;
	userData.memData  = &memOperator;

	api.Init(&handle, VO_AUDIO_CodingAAC, &userData);

	AACENC_PARAM	 params;

	params.sampleRate = format.rate;
	params.nChannels  = format.channels;
	params.bitRate	  = config->GetIntValue("VOAACEnc", "Bitrate", 128) * 1000;
	params.adtsUsed	  = !config->GetIntValue("VOAACEnc", "MP4Container", 1);

	api.SetParam(handle, VO_PID_AAC_ENCPARAM, &params);

	if (config->GetIntValue("VOAACEnc", "MP4Container", 1))
	{
		mp4File		= ex_MP4CreateEx(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), 0, 1, 1, NIL, 0, NIL, 0);
		mp4Track	= ex_MP4AddAudioTrack(mp4File, format.rate, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);

		ex_MP4SetAudioProfileLevel(mp4File, 0x0F);

		int	 sampleRateIndex = GetSampleRateIndex(format.rate);
		uint8_t	 esConfig[2]	 = { uint8_t(			 2 << 3 | sampleRateIndex >> 1),
					     uint8_t((sampleRateIndex & 1) << 7 | format.channels << 3) };

		ex_MP4SetTrackESConfiguration(mp4File, mp4Track, esConfig, 2);

		frameSize	= samplesSize / format.channels;

		totalSamples	= 0;
		encodedSamples	= 0;
		delaySamples	= frameSize;
	}

	packageSize	= samplesSize * (format.bits / 8);

	if (!config->GetIntValue("VOAACEnc", "MP4Container", 1))
	{
		if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("VOAACEnc", "AllowID3v2", 0))
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
	}

	return True;
}

Bool BoCA::EncoderVOAAC::Deactivate()
{
	Config		*config = Config::Get();

	VO_CODECBUFFER		 output	    = { 0 };
	VO_AUDIO_OUTPUTINFO	 outputInfo = { 0 };

	output.Buffer = outBuffer;
	output.Length = outBuffer.Size();

	while (api.GetOutputData(handle, &output, &outputInfo) == VO_ERR_NONE)
	{
		if (config->GetIntValue("VOAACEnc", "MP4Container", 1))
		{
			Int		 samplesLeft = totalSamples - encodedSamples + delaySamples;
			MP4Duration	 dur	     = samplesLeft > frameSize ? frameSize : samplesLeft;
			MP4Duration	 ofs	     = encodedSamples > 0 ? 0 : delaySamples;

			ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, output.Length, dur, ofs, true);

			encodedSamples += dur;
		}
		else
		{
			driver->WriteData(outBuffer, output.Length);
		}
	}

	api.Uninit(handle);

	if (config->GetIntValue("VOAACEnc", "MP4Container", 1))
	{
		ex_MP4Close(mp4File, 0);

		/* Write metadata to file
		 */
		if (config->GetIntValue("Tags", "EnableMP4Metadata", True))
		{
			const Info	&info = track.GetInfo();

			if (info.artist != NIL || info.title != NIL)
			{
				AS::Registry		&boca = AS::Registry::Get();
				AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("mp4-tag");

				if (tagger != NIL)
				{
					tagger->RenderStreamInfo(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), track);

					boca.DeleteComponent(tagger);
				}
			}
		}

		/* Stream contents of created MP4 file to output driver
		 */
		InStream		 in(STREAM_FILE, Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), IS_READ);
		Buffer<UnsignedByte>	 buffer(1024);
		Int			 bytesLeft = in.Size();

		while (bytesLeft)
		{
			in.InputData(buffer, Math::Min(1024, bytesLeft));

			driver->WriteData(buffer, Math::Min(1024, bytesLeft));

			bytesLeft -= Math::Min(1024, bytesLeft);
		}

		in.Close();

		File(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out")).Delete();
	}

	return True;
}

Int BoCA::EncoderVOAAC::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	Config	*config = Config::Get();

	/* Convert samples to 16 bit.
	 */
	const Format		&format	     = track.GetFormat();
	Int			 samplesRead = size / (format.bits / 8);

	VO_CODECBUFFER		 input	     = { 0 };
	VO_CODECBUFFER		 output	     = { 0 };
	VO_AUDIO_OUTPUTINFO	 outputInfo  = { 0 };

	if (format.bits != 16)
	{
		for (Int i = 0; i < samplesRead; i++)
		{
			if	(format.bits ==  8				) samplesBuffer[i] =	   (				   data [i] - 128) * 256;
			else if (format.bits == 32				) samplesBuffer[i] = (int) (((int32_t *) (unsigned char *) data)[i]	   / 65536);

			else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[i] = (int) ((data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 65536);
			else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[i] = (int) ((data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 65536);
		}

		input.Buffer = (uint8_t *) (int16_t *) samplesBuffer;
		input.Length = samplesRead * 2;
	}
	else
	{
		input.Buffer = data;
		input.Length = samplesRead * 2;
	}

	totalSamples += samplesRead / format.channels;

	/* Hand input data to encoder and retrieve output.
	 */
	api.SetInputData(handle, &input);

	output.Buffer = outBuffer;
	output.Length = outBuffer.Size();

	if (api.GetOutputData(handle, &output, &outputInfo) == VO_ERR_NONE)
	{
		if (config->GetIntValue("VOAACEnc", "MP4Container", 1))
		{
			Int		 samplesLeft = totalSamples - encodedSamples + delaySamples;
			MP4Duration	 dur	     = samplesLeft > frameSize ? frameSize : samplesLeft;
			MP4Duration	 ofs	     = encodedSamples > 0 ? 0 : delaySamples;

			ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, output.Length, dur, ofs, true);

			encodedSamples += dur;
		}
		else
		{
			driver->WriteData(outBuffer, output.Length);
		}
	}

	return output.Length;
}

Int BoCA::EncoderVOAAC::GetSampleRateIndex(Int sampleRate)
{
	Int	 sampleRates[12] = { 96000, 88200, 64000, 48000, 44100, 32000,
				     24000, 22050, 16000, 12000, 11025,  8000 };

	for (Int i = 0; i < 12; i++)
	{
		if (sampleRate == sampleRates[i]) return i;
	}

	return -1;
}

String BoCA::EncoderVOAAC::GetOutputFileExtension()
{
	Config	*config = Config::Get();

	if (config->GetIntValue("VOAACEnc", "MP4Container", 1))
	{
		switch (config->GetIntValue("VOAACEnc", "MP4FileExtension", 0))
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

ConfigLayer *BoCA::EncoderVOAAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureVOAAC();

	return configLayer;
}
