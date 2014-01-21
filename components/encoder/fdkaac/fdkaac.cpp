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

#include "fdkaac.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderFDKAAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (fdkaacdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Fraunhofer FDK AAC Encoder</name>				\
		    <version>1.0</version>						\
		    <id>fdkaac-enc</id>							\
		    <type>encoder</type>						\
		    <replace>faac-enc</replace>						\
		    <replace>voaacenc-enc</replace>					\
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
	LoadFDKAACDLL();
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeFDKAACDLL();
	FreeMP4v2DLL();
}

BoCA::EncoderFDKAAC::EncoderFDKAAC()
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
}

BoCA::EncoderFDKAAC::~EncoderFDKAAC()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderFDKAAC::Activate()
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

	ex_aacEncOpen(&handle, 0x07, format.channels);

	ex_aacEncoder_SetParam(handle, AACENC_SAMPLERATE, format.rate);
	ex_aacEncoder_SetParam(handle, AACENC_CHANNELMODE, format.channels);

	ex_aacEncoder_SetParam(handle, AACENC_AOT, config->GetIntValue("FDKAAC", "MPEGVersion", 0) + config->GetIntValue("FDKAAC", "AACType", AOT_SBR));
	ex_aacEncoder_SetParam(handle, AACENC_BITRATE, config->GetIntValue("FDKAAC", "Bitrate", 96) * 1000);
	ex_aacEncoder_SetParam(handle, AACENC_AFTERBURNER, 1);
	ex_aacEncoder_SetParam(handle, AACENC_TRANSMUX, config->GetIntValue("FDKAAC", "MP4Container", 1) ? TT_MP4_RAW  : TT_MP4_ADTS);

	if (!config->GetIntValue("FDKAAC", "MP4Container", 1))
	{
		if (config->GetIntValue("FDKAAC", "AACType", AOT_SBR) == AOT_ER_AAC_LD ||
		    config->GetIntValue("FDKAAC", "AACType", AOT_SBR) == AOT_ER_AAC_ELD) ex_aacEncoder_SetParam(handle, AACENC_TRANSMUX, TT_MP4_LOAS);
	}

	AACENC_InfoStruct	 aacInfo;

	ex_aacEncEncode(handle, NULL, NULL, NULL, NULL);
	ex_aacEncInfo(handle, &aacInfo);

	outBuffer.Resize(aacInfo.maxOutBufBytes);
	samplesBuffer.Resize(aacInfo.frameLength * format.channels);

	if (config->GetIntValue("FDKAAC", "MP4Container", 1))
	{
		mp4File		= ex_MP4CreateEx(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), 0, 1, 1, NIL, 0, NIL, 0);
		mp4Track	= ex_MP4AddAudioTrack(mp4File, format.rate, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);

		ex_MP4SetAudioProfileLevel(mp4File, 0x0F);
		ex_MP4SetTrackESConfiguration(mp4File, mp4Track, aacInfo.confBuf, aacInfo.confSize);

		frameSize	= aacInfo.frameLength;

		totalSamples	= 0;
		encodedSamples	= 0;
		delaySamples	= aacInfo.encoderDelay;
	}

	packageSize	= samplesBuffer.Size() * (format.bits / 8);

	if (!config->GetIntValue("FDKAAC", "MP4Container", 1))
	{
		if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("FDKAAC", "AllowID3v2", 0))
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

Bool BoCA::EncoderFDKAAC::Deactivate()
{
	Config		*config = Config::Get();

	/* Prepare buffer information.
	 */
	Void	*inputBuffer	   = (int16_t *) samplesBuffer;
	Int	 inputBufferID	   = IN_AUDIO_DATA;
	Int	 inputBufferSize   = 0;
	Int	 inputElementSize  = 2;

	Void	*outputBuffer	   = (uint8_t *) outBuffer;
	Int	 outputBufferID	   = OUT_BITSTREAM_DATA;
	Int	 outputBufferSize  = outBuffer.Size();
	Int	 outputElementSize = 1;

	/* Configure buffer descriptors.
	 */
	AACENC_BufDesc	 input	     = { 0 };
	AACENC_BufDesc	 output	     = { 0 };

	input.numBufs		 = 1;
	input.bufs		 = &inputBuffer;
	input.bufferIdentifiers  = &inputBufferID;
	input.bufSizes		 = &inputBufferSize;
	input.bufElSizes	 = &inputElementSize;

	output.numBufs		 = 1;
	output.bufs		 = &outputBuffer;
	output.bufferIdentifiers = &outputBufferID;
	output.bufSizes		 = &outputBufferSize;
	output.bufElSizes	 = &outputElementSize;

	/* Flush stream buffers and retrieve output.
	 */
	AACENC_InArgs	 inputInfo   = { 0 };
	AACENC_OutArgs	 outputInfo  = { 0 };

	while (ex_aacEncEncode(handle, &input, &output, &inputInfo, &outputInfo) == AACENC_OK && outputInfo.numOutBytes > 0)
	{
		if (config->GetIntValue("FDKAAC", "MP4Container", 1))
		{
			Int		 samplesLeft = totalSamples - encodedSamples + delaySamples;
			MP4Duration	 dur	     = samplesLeft > frameSize ? frameSize : samplesLeft;
			MP4Duration	 ofs	     = encodedSamples > 0 ? 0 : delaySamples;

			ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, outputInfo.numOutBytes, dur, ofs, true);

			encodedSamples += dur;
		}
		else
		{
			driver->WriteData(outBuffer, outputInfo.numOutBytes);
		}
	}

	ex_aacEncClose(&handle);

	if (config->GetIntValue("FDKAAC", "MP4Container", 1))
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

Int BoCA::EncoderFDKAAC::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	Config	*config = Config::Get();

	/* Prepare buffer information.
	 */
	Void	*inputBuffer	   = (int16_t *) samplesBuffer;
	Int	 inputBufferID	   = IN_AUDIO_DATA;
	Int	 inputBufferSize   = samplesBuffer.Size();
	Int	 inputElementSize  = 2;

	Void	*outputBuffer	   = (uint8_t *) outBuffer;
	Int	 outputBufferID	   = OUT_BITSTREAM_DATA;
	Int	 outputBufferSize  = outBuffer.Size();
	Int	 outputElementSize = 1;

	/* Convert samples to 16 bit.
	 */
	const Format	&format	     = track.GetFormat();
	Int		 samplesRead = size / (format.bits / 8);

	if (format.bits != 16)
	{
		for (Int i = 0; i < samplesRead; i++)
		{
			if	(format.bits ==  8				) samplesBuffer[i] =	   (				   data [i] - 128) * 256;
			else if (format.bits == 32				) samplesBuffer[i] = (int) (((int32_t *) (unsigned char *) data)[i]	   / 65536);

			else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[i] = (int) ((data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 65536);
			else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[i] = (int) ((data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 65536);
		}
	}
	else
	{
		inputBuffer	= (uint8_t *) data;
		inputBufferSize = samplesRead * 2;
	}

	totalSamples += samplesRead / format.channels;

	/* Configure buffer descriptors.
	 */
	AACENC_BufDesc	 input	     = { 0 };
	AACENC_BufDesc	 output	     = { 0 };

	input.numBufs		 = 1;
	input.bufs		 = &inputBuffer;
	input.bufferIdentifiers  = &inputBufferID;
	input.bufSizes		 = &inputBufferSize;
	input.bufElSizes	 = &inputElementSize;

	output.numBufs		 = 1;
	output.bufs		 = &outputBuffer;
	output.bufferIdentifiers = &outputBufferID;
	output.bufSizes		 = &outputBufferSize;
	output.bufElSizes	 = &outputElementSize;

	/* Hand input data to encoder and retrieve output.
	 */
	AACENC_InArgs	 inputInfo   = { 0 };
	AACENC_OutArgs	 outputInfo  = { 0 };

	inputInfo.numInSamples = samplesRead;

	if (ex_aacEncEncode(handle, &input, &output, &inputInfo, &outputInfo) == AACENC_OK)
	{
		if (config->GetIntValue("FDKAAC", "MP4Container", 1))
		{
			Int		 samplesLeft = totalSamples - encodedSamples + delaySamples;
			MP4Duration	 dur	     = samplesLeft > frameSize ? frameSize : samplesLeft;
			MP4Duration	 ofs	     = encodedSamples > 0 ? 0 : delaySamples;

			ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, outputInfo.numOutBytes, dur, ofs, true);

			encodedSamples += dur;
		}
		else
		{
			driver->WriteData(outBuffer, outputInfo.numOutBytes);
		}
	}

	return outputInfo.numOutBytes;
}

Int BoCA::EncoderFDKAAC::GetSampleRateIndex(Int sampleRate)
{
	Int	 sampleRates[12] = { 96000, 88200, 64000, 48000, 44100, 32000,
				     24000, 22050, 16000, 12000, 11025,  8000 };

	for (Int i = 0; i < 12; i++)
	{
		if (sampleRate == sampleRates[i]) return i;
	}

	return -1;
}

String BoCA::EncoderFDKAAC::GetOutputFileExtension()
{
	Config	*config = Config::Get();

	if (config->GetIntValue("FDKAAC", "MP4Container", 1))
	{
		switch (config->GetIntValue("FDKAAC", "MP4FileExtension", 0))
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

ConfigLayer *BoCA::EncoderFDKAAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureFDKAAC();

	return configLayer;
}
