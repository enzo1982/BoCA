 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
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

#include "voaacenc.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderVOAAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (voaacencdll != NIL)
	{
		componentSpecs = "										\
														\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>							\
		  <component>											\
		    <name>VisualOn AAC Encoder</name>								\
		    <version>1.0</version>									\
		    <id>voaacenc-enc</id>									\
		    <type>encoder</type>									\
														\
		";

		if (AS::Registry::Get().ComponentExists("mp4-mux"))
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
		    <input bits=\"16\" channels=\"1-2\"								\
			   rate=\"8000,11025,12000,16000,22050,24000,32000,44100,48000,64000,88200,96000\"/>	\
		    <parameters>										\
		      <range name=\"Bitrate per channel\" argument=\"-b %VALUE\" default=\"96\">		\
			<min alias=\"min\">8</min>								\
			<max alias=\"max\">128</max>								\
		      </range>											\
														\
		");

		if (AS::Registry::Get().ComponentExists("mp4-mux"))
		{
			componentSpecs.Append("									\
														\
			      <switch name=\"Create ADTS AAC files (no MP4 container)\" argument=\"--adts\"/>	\
														\
			");
		}

		componentSpecs.Append("										\
														\
		    </parameters>										\
		  </component>											\
														\
		");
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadVOAACEncDLL();
}

Void smooth::DetachDLL()
{
	FreeVOAACEncDLL();
}

BoCA::EncoderVOAAC::EncoderVOAAC()
{
	configLayer  = NIL;
	config	     = NIL;

	muxer	     = NIL;

	handle	     = NIL;

	frameSize    = 0;

	totalSamples = 0;
	delaySamples = 0;

	memset(&memOperator, 0, sizeof(memOperator));
	memset(&userData, 0, sizeof(userData));

	ex_voGetAACEncAPI(&api);
}

BoCA::EncoderVOAAC::~EncoderVOAAC()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderVOAAC::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info	= track.GetInfo();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	AS::Registry	&boca = AS::Registry::Get();

	if (!boca.ComponentExists("mp4-mux")) config->SetIntValue(ConfigureVOAAC::ConfigID, "MP4Container", False);

	Bool	 mp4Container = config->GetIntValue(ConfigureVOAAC::ConfigID, "MP4Container", True);
	Int	 bitrate      = config->GetIntValue(ConfigureVOAAC::ConfigID, "Bitrate", 96);

	/* Create VO AAC encoder.
	 */
	unsigned long	 samplesSize = 1024 * format.channels;
	unsigned long	 bufferSize  = samplesSize * 4;

	outBuffer.Resize(bufferSize);

	memOperator.Alloc = ex_cmnMemAlloc;
	memOperator.Copy  = ex_cmnMemCopy;
	memOperator.Free  = ex_cmnMemFree;
	memOperator.Set	  = ex_cmnMemSet;
	memOperator.Check = ex_cmnMemCheck;

	userData.memflag  = VO_IMF_USERMEMOPERATOR;
	userData.memData  = &memOperator;

	api.Init(&handle, VO_AUDIO_CodingAAC, &userData);

	/* Set encoder parameters.
	 */
	AACENC_PARAM	 params;

	params.sampleRate = format.rate;
	params.nChannels  = format.channels;
	params.bitRate	  = bitrate * 1000 * format.channels;
	params.adtsUsed	  = !mp4Container;

	api.SetParam(handle, VO_PID_AAC_ENCPARAM, &params);

	frameSize    = samplesSize / format.channels;
	delaySamples = frameSize + 576;

	/* Check whether to use MP4 container.
	 */
	if (mp4Container)
	{
		/* Get codec configuration.
		 */
		int			 sampleRateIndex = GetSampleRateIndex(format.rate);
		Buffer<UnsignedByte>	 codecConfig(2);

		codecConfig[0] = uint8_t(		      2 << 3 | sampleRateIndex >> 1);
		codecConfig[1] = uint8_t((sampleRateIndex & 1) << 7 | format.channels << 3);

		/* Create MP4 muxer.
		 */
		muxer = (AS::MuxerComponent *) boca.CreateComponentByID("mp4-mux");

		if (muxer != NIL)
		{
			muxer->SetConfiguration(config);
			muxer->SetAudioTrackInfo(track);
			muxer->SetDriver(driver);

			muxer->SetCodec(MUXER_CODEC_AAC);
			muxer->SetCodecConfig(codecConfig);
			muxer->SetTimeScale(format.rate);
		}

		if (muxer == NIL || !muxer->Open(MUXER_MODE_WRITE))
		{
			if (muxer != NIL) boca.DeleteComponent(muxer);

			muxer = NIL;

			api.Uninit(handle);

			return False;
		}

		totalSamples = 0;
	}

	/* Write ID3v2 tag if requested.
	 */
	if (muxer == NIL && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureVOAAC::ConfigID, "AllowID3v2", False))
	{
		if (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))
		{
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

Bool BoCA::EncoderVOAAC::Deactivate()
{
	/* Output remaining samples to encoder.
	 */
	EncodeFrames(True);

	api.Uninit(handle);

	/* Finish MP4 writing.
	 */
	Bool	 mp4Container = (muxer != NIL);

	if (muxer != NIL)
	{
		AS::Registry	&boca = AS::Registry::Get();

		/* Write gapless information.
		 */
		muxer->SetGaplessInfo(delaySamples, frameSize - (delaySamples + totalSamples) % frameSize, totalSamples);
		muxer->Close();

		/* Close output file as tagger needs to modify it.
		 */
		driver->Close();

		/* Write metadata to file.
		 */
		const Info	&info = track.GetInfo();

		if (config->GetIntValue("Tags", "EnableMP4Metadata", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))))
		{
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("mp4-tag");

			if (tagger != NIL)
			{
				tagger->SetConfiguration(config);
				tagger->RenderStreamInfo(track.outputFile, track);

				boca.DeleteComponent(tagger);
			}
		}
		else
		{
			/* Optimize file even when no tags are written.
			 */
			muxer->OptimizeFile(track.outputFile);
		}

		boca.DeleteComponent(muxer);

		muxer = NIL;
	}

	/* Write ID3v1 tag if requested.
	 */
	if (!mp4Container && config->GetIntValue("Tags", "EnableID3v1", False))
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
	if (!mp4Container && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureVOAAC::ConfigID, "AllowID3v2", False))
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

Int BoCA::EncoderVOAAC::WriteData(Buffer<UnsignedByte> &data)
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

Int BoCA::EncoderVOAAC::EncodeFrames(Bool flush)
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
		VO_CODECBUFFER		 input	     = { 0 };
		VO_CODECBUFFER		 output	     = { 0 };
		VO_AUDIO_OUTPUTINFO	 outputInfo  = { 0 };

		input.Buffer = (uint8_t *) ((int16_t *) samplesBuffer + framesProcessed * samplesPerFrame);
		input.Length = samplesPerFrame * sizeof(int16_t);

		/* Hand input data to encoder and retrieve output.
		 */
		api.SetInputData(handle, &input);

		output.Buffer = outBuffer;
		output.Length = outBuffer.Size();

		if (api.GetOutputData(handle, &output, &outputInfo) == VO_ERR_NONE)
		{
			dataLength += output.Length;

			if (muxer != NIL) muxer->WritePacket(outBuffer, output.Length, muxer->GetDuration() + frameSize, PACKET_FLAG_NONE);
			else		  driver->WriteData(outBuffer, output.Length);
		}

		framesProcessed++;
	}

	memmove(samplesBuffer, samplesBuffer + framesProcessed * samplesPerFrame, sizeof(int16_t) * (samplesBuffer.Size() - framesProcessed * samplesPerFrame));

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * samplesPerFrame);

	return dataLength;
}

Int BoCA::EncoderVOAAC::GetSampleRateIndex(Int sampleRate) const
{
	Int	 sampleRates[12] = { 96000, 88200, 64000, 48000, 44100, 32000,
				     24000, 22050, 16000, 12000, 11025,  8000 };

	for (Int i = 0; i < 12; i++)
	{
		if (sampleRate == sampleRates[i]) return i;
	}

	return -1;
}

Bool BoCA::EncoderVOAAC::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	if (n == 0 && AS::Registry::Get().ComponentExists("mp4-mux")) config->SetIntValue(ConfigureVOAAC::ConfigID, "MP4Container", True);
	else			       config->SetIntValue(ConfigureVOAAC::ConfigID, "MP4Container", False);

	return True;
}

String BoCA::EncoderVOAAC::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (config->GetIntValue(ConfigureVOAAC::ConfigID, "MP4Container", True))
	{
		switch (config->GetIntValue(ConfigureVOAAC::ConfigID, "MP4FileExtension", 0))
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

Bool BoCA::EncoderVOAAC::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "voaacenc-enc";

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureVOAAC::ConfigID, "MP4Container", True);

		config->SetIntValue(ConfigureVOAAC::ConfigID, "Bitrate", 96);
	}

	/* Get command line settings.
	 */
	Bool	 rawAAC	 = config->GetIntValue(encoderID, "Create ADTS AAC files (no MP4 container)", !config->GetIntValue(ConfigureVOAAC::ConfigID, "MP4Container", True));

	Int	 bitrate = config->GetIntValue(ConfigureVOAAC::ConfigID, "Bitrate", 96);

	if (config->GetIntValue(encoderID, "Set Bitrate per channel", False)) bitrate = config->GetIntValue(encoderID, "Bitrate per channel", bitrate);

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureVOAAC::ConfigID, "MP4Container", !rawAAC);

	config->SetIntValue(ConfigureVOAAC::ConfigID, "Bitrate", Math::Max(8, Math::Min(128, bitrate)));

	return True;
}

ConfigLayer *BoCA::EncoderVOAAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureVOAAC();

	return configLayer;
}
