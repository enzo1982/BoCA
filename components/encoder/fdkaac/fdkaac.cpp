 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

#include "fdkaac.h"
#include "config.h"

const String &BoCA::EncoderFDKAAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (fdkaacdll != NIL)
	{
		componentSpecs = "										\
														\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>							\
		  <component>											\
		    <name>Fraunhofer FDK AAC Encoder</name>							\
		    <version>1.0</version>									\
		    <id>fdkaac-enc</id>										\
		    <type>encoder</type>									\
		    <replace>faac-enc</replace>									\
		    <replace>voaacenc-enc</replace>								\
														\
		";

		if (mp4v2dll != NIL)
		{
			componentSpecs.Append("									\
														\
			    <format>										\
			      <name>MPEG-4 AAC Files</name>							\
			      <extension>m4a</extension>							\
			      <extension>m4b</extension>							\
			      <extension>m4r</extension>							\
			      <extension>mp4</extension>							\
			      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>				\
			    </format>										\
														\
			");
		}

		componentSpecs.Append("										\
														\
		    <format>											\
		      <name>Raw AAC Files</name>								\
		      <extension>aac</extension>								\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>					\
		    </format>											\
		    <input bits=\"16\" channels=\"1-6,8\"							\
			   rate=\"8000,11025,12000,16000,22050,24000,32000,44100,48000,64000,88200,96000\"/>	\
		    <parameters>										\
		      <selection name=\"AAC encoding mode\" argument=\"-m %VALUE\" default=\"LC\">		\
			<option alias=\"Low Complexity\">LC</option>						\
			<option alias=\"High Efficiency\">HE</option>						\
			<option alias=\"High Efficiency v2\">HEv2</option>					\
			<option alias=\"Low Delay\">LD</option>							\
			<option alias=\"Enhanced Low Delay\">ELD</option>					\
		      </selection>										\
		      <range name=\"Bitrate per channel\" argument=\"-b %VALUE\" default=\"64\">		\
			<min alias=\"min\">8</min>								\
			<max alias=\"max\">256</max>								\
		      </range>											\
		      <switch name=\"Write raw AAC files\" argument=\"--raw\"/>					\
		    </parameters>										\
		  </component>											\
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
	configLayer  = NIL;

	mp4File	     = NIL;
	handle	     = NIL;

	mp4Track     = -1;
	sampleId     = 0;

	frameSize    = 0;

	totalSamples = 0;
	delaySamples = 0;

	config	     = Config::Copy(GetConfiguration());

	ConvertArguments(config);
}

BoCA::EncoderFDKAAC::~EncoderFDKAAC()
{
	Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderFDKAAC::Activate()
{
	const Format	&format = track.GetFormat();

	/* Get configuration.
	 */
	Bool	 mp4Container = config->GetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", True);
	Int	 mpegVersion  = config->GetIntValue(ConfigureFDKAAC::ConfigID, "MPEGVersion", 0);
	Int	 aacType      = config->GetIntValue(ConfigureFDKAAC::ConfigID, "AACType", AOT_AAC_LC);
	Int	 bitrate      = config->GetIntValue(ConfigureFDKAAC::ConfigID, "Bitrate", 64);

	/* Create and configure FDK AAC encoder.
	 */
	ex_aacEncOpen(&handle, 0x07, format.channels);

	Int	 channelMode = MODE_UNKNOWN;

	switch (format.channels)
	{
		case 1: channelMode = MODE_1;	      break;
		case 2: channelMode = MODE_2;	      break;
		case 3: channelMode = MODE_1_2;	      break;
		case 4: channelMode = MODE_1_2_1;     break;
		case 5: channelMode = MODE_1_2_2;     break;
		case 6: channelMode = MODE_1_2_2_1;   break;
		case 8: channelMode = MODE_1_2_2_2_1; break;
	}

	ex_aacEncoder_SetParam(handle, AACENC_SAMPLERATE, format.rate);
	ex_aacEncoder_SetParam(handle, AACENC_CHANNELMODE, channelMode);
	ex_aacEncoder_SetParam(handle, AACENC_CHANNELORDER, 1 /* WAVE channel order */);

	ex_aacEncoder_SetParam(handle, AACENC_AOT, mpegVersion + aacType);
	ex_aacEncoder_SetParam(handle, AACENC_BITRATE, bitrate * 1000 * format.channels);
	ex_aacEncoder_SetParam(handle, AACENC_AFTERBURNER, 1);
	ex_aacEncoder_SetParam(handle, AACENC_TRANSMUX, mp4Container ? TT_MP4_RAW : TT_MP4_ADTS);

	if (!mp4Container)
	{
		if (aacType == AOT_ER_AAC_LD ||
		    aacType == AOT_ER_AAC_ELD) ex_aacEncoder_SetParam(handle, AACENC_TRANSMUX, TT_MP4_LOAS);
	}

	AACENC_InfoStruct	 aacInfo;

	ex_aacEncEncode(handle, NULL, NULL, NULL, NULL);
	ex_aacEncInfo(handle, &aacInfo);

	outBuffer.Resize(aacInfo.maxOutBufBytes);

	frameSize    = aacInfo.frameLength;
	delaySamples = aacInfo.encoderDelay;

	/* Adjust delay for LD/ELD object types.
	 */
	if (aacType == AOT_ER_AAC_LD)  delaySamples -= frameSize * 0.5625;
	if (aacType == AOT_ER_AAC_ELD) delaySamples -= frameSize;

	/* Check whether to use MP4 container.
	 */
	if (mp4Container)
	{
		/* Close output file as it will be written directly by MP4v2.
		 */
		driver->Close();

		/* Create MP4 file.
		 */
		mp4File		= ex_MP4CreateEx(track.outfile.ConvertTo("UTF-8"), 0, 1, 1, NIL, 0, NIL, 0);
		mp4Track	= ex_MP4AddAudioTrack(mp4File, format.rate, MP4_INVALID_DURATION, MP4_MPEG4_AUDIO_TYPE);

		ex_MP4SetAudioProfileLevel(mp4File, 0x0F);
		ex_MP4SetTrackESConfiguration(mp4File, mp4Track, aacInfo.confBuf, aacInfo.confSize);

		totalSamples = 0;
	}

	/* Write ID3v2 tag if requested.
	 */
	if (mp4File == NIL && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureFDKAAC::ConfigID, "AllowID3v2", False))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(config);
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
	/* Output remaining samples to encoder.
	 */
	EncodeFrames(True);

	ex_aacEncClose(&handle);

	/* Finish MP4 writing.
	 */
	if (mp4File != NIL)
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
					tagger->SetConfiguration(config);
					tagger->RenderStreamInfo(track.outfile, track);

					boca.DeleteComponent(tagger);
				}
			}
		}
		else
		{
			/* Optimize file even when no tags are written.
			 */
			String	 tempFile = String(track.outfile).Append(".temp");

			ex_MP4Optimize(track.outfile.ConvertTo("UTF-8"), tempFile.ConvertTo("UTF-8"));

			File(track.outfile).Delete();
			File(tempFile).Move(track.outfile);
		}
	}

	/* Write ID3v1 tag if requested.
	 */
	if (mp4File == NIL && config->GetIntValue("Tags", "EnableID3v1", False))
	{
		const Info	&info = track.GetInfo();

		if (info.HasBasicInfo())
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(config);
				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (mp4File == NIL && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureFDKAAC::ConfigID, "AllowID3v2", False))
	{
		if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->SetConfiguration(config);
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
	const Format	&format	= track.GetFormat();

	/* Copy data to samples buffer.
	 */
	Int	 samples = data.Size() / 2;

	samplesBuffer.Resize(samplesBuffer.Size() + samples);

	memcpy(samplesBuffer + samplesBuffer.Size() - samples, data, data.Size());

	/* Output samples to encoder.
	 */
	totalSamples += data.Size() / format.channels / (format.bits / 8);

	return EncodeFrames(False);
}

Int BoCA::EncoderFDKAAC::EncodeFrames(Bool flush)
{
	const Format	&format = track.GetFormat();

	/* Pad end of stream with empty samples.
	 */
	if (flush)
	{
		Int	 nullSamples = delaySamples;

		if ((samplesBuffer.Size() / format.channels + delaySamples) % frameSize > 0) nullSamples += frameSize - (samplesBuffer.Size() / format.channels + delaySamples) % frameSize;

		samplesBuffer.Resize(samplesBuffer.Size() + nullSamples * format.channels);

		memset(samplesBuffer + samplesBuffer.Size() - nullSamples * format.channels, 0, sizeof(int16_t) * nullSamples * format.channels);
	}

	/* Encode samples.
	 */
	Int	 dataLength	 = 0;
	Int	 framesProcessed = 0;

	Int	 samplesPerFrame = frameSize * format.channels;

	while (samplesBuffer.Size() - framesProcessed * samplesPerFrame >= samplesPerFrame)
	{
		/* Prepare buffer information.
		 */
		Void	*inputBuffer	   = (int16_t *) samplesBuffer + framesProcessed * samplesPerFrame;
		Int	 inputBufferID	   = IN_AUDIO_DATA;
		Int	 inputBufferSize   = samplesPerFrame;
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

		inputInfo.numInSamples = samplesPerFrame;

		if (ex_aacEncEncode(handle, &input, &output, &inputInfo, &outputInfo) == AACENC_OK)
		{
			dataLength += outputInfo.numOutBytes;

			if (mp4File != NIL) ex_MP4WriteSample(mp4File, mp4Track, (uint8_t *) (unsigned char *) outBuffer, outputInfo.numOutBytes, frameSize, 0, true);
			else		    driver->WriteData(outBuffer, outputInfo.numOutBytes);
		}

		framesProcessed++;
	}

	memmove(samplesBuffer, samplesBuffer + framesProcessed * samplesPerFrame, sizeof(int16_t) * (samplesBuffer.Size() - framesProcessed * samplesPerFrame));

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * samplesPerFrame);

	return dataLength;
}

Bool BoCA::EncoderFDKAAC::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	if (n == 0 && mp4v2dll != NIL)
	{
		config->SetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", True);
		config->SetIntValue(ConfigureFDKAAC::ConfigID, "MPEGVersion", 0);
	}
	else
	{
		config->SetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", False);
	}

	return True;
}

String BoCA::EncoderFDKAAC::GetOutputFileExtension() const
{
	if (config->GetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", True))
	{
		switch (config->GetIntValue(ConfigureFDKAAC::ConfigID, "MP4FileExtension", 0))
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

Bool BoCA::EncoderFDKAAC::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "fdkaac-enc";

	/* Get command line settings.
	 */
	Int	 bitrate = 64;
	String	 mode	 = "LC";

	if (config->GetIntValue(encoderID, "Set Bitrate per channel", False)) bitrate = config->GetIntValue(encoderID, "Bitrate per channel", bitrate);
	if (config->GetIntValue(encoderID, "Set AAC encoding mode", False))   mode    = config->GetStringValue(encoderID, "AAC encoding mode", mode).ToUpper();

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureFDKAAC::ConfigID, "MPEGVersion", 0);

	config->SetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", !config->GetIntValue(encoderID, "Write raw AAC files", False));

	config->SetIntValue(ConfigureFDKAAC::ConfigID, "Bitrate", Math::Max(8, Math::Min(256, bitrate)));

	Int	 aacType = AOT_AAC_LC;

	if	(mode == "LC"  ) aacType = AOT_AAC_LC;
	else if (mode == "HE"  ) aacType = AOT_SBR;
	else if (mode == "HEV2") aacType = AOT_PS;
	else if (mode == "LD"  ) aacType = AOT_ER_AAC_LD;
	else if (mode == "ELD" ) aacType = AOT_ER_AAC_ELD;

	config->SetIntValue(ConfigureFDKAAC::ConfigID, "AACType", aacType);

	return True;
}

ConfigLayer *BoCA::EncoderFDKAAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureFDKAAC();

	return configLayer;
}
