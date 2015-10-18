 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
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
	delaySamples   = 0;
}

BoCA::EncoderFDKAAC::~EncoderFDKAAC()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderFDKAAC::Activate()
{
	const Format	&format = track.GetFormat();

	if (format.channels > 6)
	{
		errorString = "This encoder does not support more than 6 channels!";
		errorState  = True;

		return False;
	}

	if (GetSampleRateIndex(format.rate) == -1)
	{
		errorString = "Bad sampling rate! The selected sampling rate is not supported.";
		errorState  = True;

		return False;
	}

	const Config	*config = GetConfiguration();

	ex_aacEncOpen(&handle, 0x07, format.channels);

	ex_aacEncoder_SetParam(handle, AACENC_SAMPLERATE, format.rate);
	ex_aacEncoder_SetParam(handle, AACENC_CHANNELMODE, format.channels);
	ex_aacEncoder_SetParam(handle, AACENC_CHANNELORDER, 1);

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

	if (config->GetIntValue("FDKAAC", "MP4Container", 1))
	{
		mp4File		= ex_MP4CreateEx(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), 0, 1, 1, NIL, 0, NIL, 0);
		mp4Track	= ex_MP4AddAudioTrack(mp4File, format.rate, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);

		ex_MP4SetAudioProfileLevel(mp4File, 0x0F);
		ex_MP4SetTrackESConfiguration(mp4File, mp4Track, aacInfo.confBuf, aacInfo.confSize);

		totalSamples = 0;
	}

	frameSize    = aacInfo.frameLength;
	delaySamples = aacInfo.encoderDelay;

	/* Adjust delay for LD/ELD object types.
	 */
	if (config->GetIntValue("FDKAAC", "AACType", AOT_SBR) == AOT_ER_AAC_LD)	 delaySamples -= frameSize * 0.5625;
	if (config->GetIntValue("FDKAAC", "AACType", AOT_SBR) == AOT_ER_AAC_ELD) delaySamples -= frameSize;

	/* Write ID3v2 tag if requested.
	 */
	if (!config->GetIntValue("FDKAAC", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("FDKAAC", "AllowID3v2", 0))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(GetConfiguration());
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
	const Config	*config = GetConfiguration();

	/* Output remaining samples to encoder.
	 */
	EncodeFrames(samplesBuffer, outBuffer, True);

	ex_aacEncClose(&handle);

	/* Finish MP4 writing.
	 */
	if (config->GetIntValue("FDKAAC", "MP4Container", 1))
	{
		/* Write iTunes metadata with gapless information.
		 */
		MP4ItmfItem	*item  = ex_MP4ItmfItemAlloc("----", 1);
		String		 value = String().Append(" 00000000")
						 .Append(" ").Append(Number((Int64) delaySamples).ToHexString(8).ToUpper())
						 .Append(" ").Append(Number((Int64) frameSize - (delaySamples + totalSamples) % frameSize).ToHexString(8).ToUpper())
						 .Append(" ").Append(Number((Int64) totalSamples).ToHexString(16).ToUpper())
						 .Append(" 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000");

		item->mean = (char *) "com.apple.iTunes";
		item->name = (char *) "iTunSMPB";

		item->dataList.elements[0].typeCode  = MP4_ITMF_BT_UTF8;
		item->dataList.elements[0].value     = (uint8_t *) value.ConvertTo("UTF-8");
		item->dataList.elements[0].valueSize = value.Length();

		ex_MP4ItmfAddItem(mp4File, item);

		item->mean = NIL;
		item->name = NIL;

		item->dataList.elements[0].typeCode  = MP4_ITMF_BT_IMPLICIT;
		item->dataList.elements[0].value     = NIL;
		item->dataList.elements[0].valueSize = 0;

		ex_MP4ItmfItemFree(item);

		ex_MP4Close(mp4File, 0);

		/* Write metadata to file
		 */
		if (config->GetIntValue("Tags", "EnableMP4Metadata", True))
		{
			const Info	&info = track.GetInfo();

			if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
			{
				AS::Registry		&boca = AS::Registry::Get();
				AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("mp4-tag");

				if (tagger != NIL)
				{
					tagger->SetConfiguration(GetConfiguration());
					tagger->RenderStreamInfo(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), track);

					boca.DeleteComponent(tagger);
				}
			}
		}

		/* Stream contents of created MP4 file to output driver
		 */
		InStream		 in(STREAM_FILE, Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out"), IS_READ);
		Buffer<UnsignedByte>	 buffer(1024);
		Int64			 bytesLeft = in.Size();

		while (bytesLeft)
		{
			in.InputData(buffer, Math::Min(Int64(1024), bytesLeft));

			driver->WriteData(buffer, Math::Min(Int64(1024), bytesLeft));

			bytesLeft -= Math::Min(Int64(1024), bytesLeft);
		}

		in.Close();

		File(Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out")).Delete();
	}

	/* Write ID3v1 tag if requested.
	 */
	if (!config->GetIntValue("FDKAAC", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v1", False))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo())
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(GetConfiguration());
				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (!config->GetIntValue("FDKAAC", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("FDKAAC", "AllowID3v2", 0))
	{
		if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(GetConfiguration());
				tagger->RenderBuffer(id3Buffer, track);

				driver->Seek(0);
				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	return True;
}

Int BoCA::EncoderFDKAAC::WriteData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Convert samples to 16 bit.
	 */
	const Format	&format	 = track.GetFormat();
	Int		 samples = data.Size() / format.channels / (format.bits / 8);
	Int		 offset	 = samplesBuffer.Size();

	samplesBuffer.Resize(samplesBuffer.Size() + samples * format.channels);

	for (Int i = 0; i < samples * format.channels; i++)
	{
		if	(format.bits ==  8				) samplesBuffer[offset + i] =	    (				    data [i] - 128) * 256;
		else if	(format.bits == 16				) samplesBuffer[offset + i] = (int)  ((int16_t *) (unsigned char *) data)[i];
		else if (format.bits == 32				) samplesBuffer[offset + i] = (int) (((int32_t *) (unsigned char *) data)[i]	    / 65536);

		else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[offset + i] = (int) ((data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 65536);
		else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[offset + i] = (int) ((data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 65536);
	}

	/* Output samples to encoder.
	 */
	return EncodeFrames(samplesBuffer, outBuffer, False);
}

Int BoCA::EncoderFDKAAC::EncodeFrames(Buffer<int16_t> &samplesBuffer, Buffer<unsigned char> &outBuffer, Bool flush)
{
	const Config	*config = GetConfiguration();
	const Format	&format = track.GetFormat();

	/* Pad end of stream with empty samples.
	 */
	if (flush)
	{
		Int	 nullSamples = delaySamples;

		if ((samplesBuffer.Size() / format.channels + delaySamples) % frameSize > 0) nullSamples += frameSize - (samplesBuffer.Size() / format.channels + delaySamples) % frameSize;

		samplesBuffer.Resize(samplesBuffer.Size() + nullSamples * format.channels);

		memset(((int16_t *) samplesBuffer) + samplesBuffer.Size() - nullSamples * format.channels, 0, sizeof(int16_t) * nullSamples * format.channels);

		totalSamples += samplesBuffer.Size() / format.channels - nullSamples;
	}

	/* Encode samples.
	 */
	Int	 dataLength	 = 0;
	Int	 framesProcessed = 0;

	while (samplesBuffer.Size() - framesProcessed * frameSize * format.channels >= frameSize * format.channels)
	{
		/* Prepare buffer information.
		 */
		Void	*inputBuffer	   = (int16_t *) samplesBuffer + framesProcessed * frameSize * format.channels;
		Int	 inputBufferID	   = IN_AUDIO_DATA;
		Int	 inputBufferSize   = frameSize * format.channels;
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

		/* Hand input data to encoder and retrieve output.
		 */
		AACENC_InArgs	 inputInfo   = { 0 };
		AACENC_OutArgs	 outputInfo  = { 0 };

		inputInfo.numInSamples = frameSize * format.channels;

		if (ex_aacEncEncode(handle, &input, &output, &inputInfo, &outputInfo) == AACENC_OK)
		{
			if (!flush) totalSamples += frameSize;

			dataLength += outputInfo.numOutBytes;

			if (config->GetIntValue("FDKAAC", "MP4Container", 1)) ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, outputInfo.numOutBytes, frameSize, 0, true);
			else						      driver->WriteData(outBuffer, outputInfo.numOutBytes);
		}

		framesProcessed++;
	}

	memmove((int16_t *) samplesBuffer, ((int16_t *) samplesBuffer) + framesProcessed * frameSize * format.channels, sizeof(int16_t) * (samplesBuffer.Size() - framesProcessed * frameSize * format.channels));

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * frameSize * format.channels);

	return dataLength;
}

Int BoCA::EncoderFDKAAC::GetSampleRateIndex(Int sampleRate) const
{
	Int	 sampleRates[12] = { 96000, 88200, 64000, 48000, 44100, 32000,
				     24000, 22050, 16000, 12000, 11025,  8000 };

	for (Int i = 0; i < 12; i++)
	{
		if (sampleRate == sampleRates[i]) return i;
	}

	return -1;
}

String BoCA::EncoderFDKAAC::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

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
