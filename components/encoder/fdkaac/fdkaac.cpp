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

#include "fdkaac.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderFDKAAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (fdkaacdll != NIL)
	{
		LIB_INFO	 info[FDK_MODULE_LAST];

		FDKinitLibInfo(info);
		ex_aacEncGetLibInfo(info);

		UINT	 sbrFlags = FDKlibInfo_getCapabilities(info, FDK_SBRENC);

		componentSpecs = "										\
														\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>							\
		  <component>											\
		    <name>FDK-AAC Encoder %VERSION%</name>							\
		    <version>1.0</version>									\
		    <id>fdkaac-enc</id>										\
		    <type>encoder</type>									\
		    <replace>faac-enc</replace>									\
		    <replace>voaacenc-enc</replace>								\
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
		    <input bits=\"16\" channels=\"1-6,8\"							\
			   rate=\"8000,11025,12000,16000,22050,24000,32000,44100,48000,64000,88200,96000\"/>	\
		    <parameters>										\
		      <selection name=\"AAC encoding mode\" argument=\"-m %VALUE\" default=\"LC\">		\
			<option alias=\"Low Complexity\">LC</option>						\
														\
		");

		if (sbrFlags)			 componentSpecs.Append("<option alias=\"High Efficiency\">HE</option>");
		if (sbrFlags & CAPF_SBR_PS_MPEG) componentSpecs.Append("<option alias=\"High Efficiency v2\">HEv2</option>");

		componentSpecs.Append("										\
														\
			<option alias=\"Low Delay\">LD</option>							\
														\
		");

		if (sbrFlags)			 componentSpecs.Append("<option alias=\"Enhanced Low Delay\">ELD</option>");

		componentSpecs.Append("										\
														\
		      </selection>										\
		      <range name=\"Bitrate per channel\" argument=\"-b %VALUE\" default=\"64\">		\
			<min alias=\"min\">8</min>								\
			<max alias=\"max\">256</max>								\
		      </range>											\
		      <switch name=\"Disable Afterburner processing\" argument=\"--no-afterburner\"/>		\
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

		UnsignedInt32	 version = GetEncoderVersion();

		componentSpecs.Replace("%VERSION%", String("v").Append(String::FromInt((version >> 24) & 0xff)).Append(".")
							       .Append(String::FromInt((version >> 16) & 0xff)).Append(".")
							       .Append(String::FromInt((version >>  8) & 0xff)));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadFDKAACDLL();
}

Void smooth::DetachDLL()
{
	FreeFDKAACDLL();
}

BoCA::EncoderFDKAAC::EncoderFDKAAC()
{
	configLayer  = NIL;
	config	     = NIL;

	muxer	     = NIL;

	frameSize    = 0;
	granuleSize  = 0;

	blockSize    = 128;
	overlap	     = 12;

	totalFrames  = 0;
	totalSamples = 0;
	delaySamples = 0;

	nextWorker   = 0;
}

BoCA::EncoderFDKAAC::~EncoderFDKAAC()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

UnsignedInt32 BoCA::EncoderFDKAAC::GetEncoderVersion()
{
	LIB_INFO	 info[FDK_MODULE_LAST];

	FDKinitLibInfo(info);
	ex_aacEncGetLibInfo(info);

	for (Int i = 0; i < FDK_MODULE_LAST; i++)
	{
		if (info[i].module_id != FDK_AACENC) continue;

		return info[i].version;
	}

	return 0;
}

Bool BoCA::EncoderFDKAAC::Activate()
{
	const Format	&format = track.GetFormat();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	AS::Registry	&boca = AS::Registry::Get();

	if (!boca.ComponentExists("mp4-mux")) config->SetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", False);

	Bool	 mp4Container = config->GetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", True);
	Int	 mpegVersion  = config->GetIntValue(ConfigureFDKAAC::ConfigID, "MPEGVersion", 0);
	Int	 aacType      = config->GetIntValue(ConfigureFDKAAC::ConfigID, "AACType", AOT_AAC_LC);
	Int	 mode	      = config->GetIntValue(ConfigureFDKAAC::ConfigID, "Mode", 0);
	Int	 bitrate      = config->GetIntValue(ConfigureFDKAAC::ConfigID, "Bitrate", 64);
	Int	 quality      = config->GetIntValue(ConfigureFDKAAC::ConfigID, "Quality", 4);

	/* Fall back to HE if HEv2 is selected for non-stereo input.
	 */
	if (aacType == AOT_PS && format.channels != 2) aacType = AOT_SBR;

	/* Create and configure FDK AAC encoder.
	 */
	HANDLE_AACENCODER	 handle = NIL;

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

	if (mode == 0) ex_aacEncoder_SetParam(handle, AACENC_BITRATE, bitrate * 1000 * format.channels);
	else	       ex_aacEncoder_SetParam(handle, AACENC_BITRATEMODE, quality);

	ex_aacEncoder_SetParam(handle, AACENC_AOT, mpegVersion + aacType);
	ex_aacEncoder_SetParam(handle, AACENC_TRANSMUX, mp4Container ? TT_MP4_RAW : TT_MP4_ADTS);

	if ( mp4Container && (aacType == AOT_SBR       || aacType == AOT_PS	   )) ex_aacEncoder_SetParam(handle, AACENC_SIGNALING_MODE, 1);
	if (!mp4Container && (aacType == AOT_ER_AAC_LD || aacType == AOT_ER_AAC_ELD)) ex_aacEncoder_SetParam(handle, AACENC_TRANSMUX, TT_MP4_LOAS);

	AACENC_InfoStruct	 aacInfo;
	AACENC_InfoStruct_Old	&aacInfoOld = *(AACENC_InfoStruct_Old *) &aacInfo;

	if (ex_aacEncEncode(handle, NULL, NULL, NULL, NULL) != AACENC_OK)
	{
		ex_aacEncClose(&handle);
		return False;
	}

	ex_aacEncInfo(handle, &aacInfo);

	frameSize    = aacInfo.frameLength;
	granuleSize  = ex_aacEncoder_GetParam(handle, AACENC_GRANULE_LENGTH);

#if AACENCODER_LIB_VL0 >= 4
	if (GetEncoderVersion() >= LIB_VERSION(4, 0, 0))
	{
		delaySamples = aacInfo.nDelayCore;
	}
	else
#endif
	{
		delaySamples = aacInfoOld.encoderDelay;

		/* Adjust delay for LD/ELD object types.
		 */
		if (aacType == AOT_ER_AAC_LD)  delaySamples = frameSize;
		if (aacType == AOT_ER_AAC_ELD) delaySamples = frameSize / 4;
	}

	/* Check whether to use MP4 container.
	 */
	if (mp4Container)
	{
		/* Get codec configuration.
		 */
		Buffer<UnsignedByte>	 codecConfig;

		if (GetEncoderVersion() >= LIB_VERSION(4, 0, 0))
		{
			codecConfig.Resize(aacInfo.confSize);

			memcpy(codecConfig, aacInfo.confBuf, aacInfo.confSize);
		}
		else
		{
			codecConfig.Resize(aacInfoOld.confSize);

			memcpy(codecConfig, aacInfoOld.confBuf, aacInfoOld.confSize);
		}

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
			muxer->SetTimeScale(format.rate * granuleSize / frameSize);
		}

		if (muxer == NIL || !muxer->Open(MUXER_MODE_WRITE))
		{
			if (muxer != NIL) boca.DeleteComponent(muxer);

			muxer = NIL;

			ex_aacEncClose(&handle);

			return False;
		}

		totalFrames  = 0;
		totalSamples = 0;
	}

	ex_aacEncClose(&handle);

	/* Write ID3v2 tag if requested.
	 */
	if (muxer == NIL && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureFDKAAC::ConfigID, "AllowID3v2", False))
	{
		const Info	&info = track.GetInfo();

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

	/* Get number of threads to use.
	 */
	Bool	 enableParallel	 = config->GetIntValue("Resources", "EnableParallelConversions", True);
	Bool	 enableSuperFast = config->GetIntValue("Resources", "EnableSuperFastMode", True);
	Int	 numberOfThreads = enableParallel && enableSuperFast ? config->GetIntValue("Resources", "NumberOfConversionThreads", 0) : 1;

	if (enableParallel && enableSuperFast && numberOfThreads <= 1) numberOfThreads = CPU().GetNumCores() + (CPU().GetNumLogicalCPUs() - CPU().GetNumCores()) / 2;

	/* Disable overlap if we use only one thread.
	 */
	if (numberOfThreads == 1) overlap = 0;
	else			  overlap = 12;

	/* Start up worker threads.
	 */
	for (Int i = 0; i < numberOfThreads; i++) workers.Add(new SuperWorker(config, format));

	foreach (SuperWorker *worker, workers) worker->Start();

	return True;
}

Bool BoCA::EncoderFDKAAC::Deactivate()
{
	/* Output remaining samples to encoder.
	 */
	EncodeFrames(True);

	/* Tear down worker threads.
	 */
	foreach (SuperWorker *worker, workers) worker->Quit();
	foreach (SuperWorker *worker, workers) worker->Wait();
	foreach (SuperWorker *worker, workers) delete worker;

	workers.RemoveAll();

	/* Finish MP4 writing.
	 */
	Bool	 mp4Container = (muxer != NIL);

	if (muxer != NIL)
	{
		AS::Registry	&boca = AS::Registry::Get();

		/* Write gapless information.
		 */
		Float	 sbrRatio   = Float(frameSize) / granuleSize;
		Int64	 delayValue = Math::Floor(delaySamples / sbrRatio);
		Int64	 totalValue = Math::Ceil(totalSamples / sbrRatio);

		muxer->SetGaplessInfo(delayValue, totalFrames * granuleSize - totalValue - delayValue, totalValue);
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
	if (!mp4Container && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureFDKAAC::ConfigID, "AllowID3v2", False))
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
	const Format	&format = track.GetFormat();

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

	/* Pass samples to workers.
	 */
	Int	 framesToProcess = blockSize;
	Int	 framesProcessed = 0;
	Int	 dataLength	 = 0;

	Int	 samplesPerFrame = frameSize * format.channels;

	if (flush) framesToProcess = Math::Floor(samplesBuffer.Size() / samplesPerFrame);

	while (samplesBuffer.Size() - framesProcessed * samplesPerFrame >= samplesPerFrame * framesToProcess)
	{
		SuperWorker	*workerToUse = workers.GetNth(nextWorker % workers.Length());

		workerToUse->WaitUntilReady();

		/* See if the worker has some packets for us.
		 */
		if (workerToUse->GetPacketSizes().Length() != 0) dataLength += ProcessPackets(workerToUse->GetPackets(), workerToUse->GetPacketSizes(), nextWorker == workers.Length());

		/* Pass new frames to worker.
		 */
		workerToUse->Encode(samplesBuffer, framesProcessed * samplesPerFrame, flush ? samplesBuffer.Size() : samplesPerFrame * framesToProcess, flush);

		framesProcessed += framesToProcess - (flush ? 0 : overlap);

		nextWorker++;

		if (flush) break;
	}

	memmove(samplesBuffer, samplesBuffer + framesProcessed * samplesPerFrame, sizeof(int16_t) * (samplesBuffer.Size() - framesProcessed * samplesPerFrame));

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * samplesPerFrame);

	if (!flush) return dataLength;

	/* Wait for workers to finish and process packets.
	 */
	for (Int i = 0; i < workers.Length(); i++)
	{
		SuperWorker	*workerToUse = workers.GetNth(nextWorker % workers.Length());

		workerToUse->WaitUntilReady();

		/* See if the worker has some packets for us.
		 */
		if (workerToUse->GetPacketSizes().Length() != 0) dataLength += ProcessPackets(workerToUse->GetPackets(), workerToUse->GetPacketSizes(), nextWorker == workers.Length());

		nextWorker++;
	}

	return dataLength;
}

Int BoCA::EncoderFDKAAC::ProcessPackets(const Buffer<unsigned char> &packets, const Array<Int> &packetSizes, Bool first)
{
	Int	 offset	    = 0;
	Int	 dataLength = 0;

	if (!first) for (Int i = 0; i < overlap; i++) offset += packetSizes.GetNth(i);

	for (Int i = 0; i < packetSizes.Length(); i++)
	{
		if (i <	overlap && !first)	continue;
		if (packetSizes.GetNth(i) == 0) continue;

		totalFrames++;

		if (muxer != NIL) muxer->WritePacket(packets + offset, packetSizes.GetNth(i), totalFrames * granuleSize, PACKET_FLAG_NONE);
		else		  driver->WriteData(packets + offset, packetSizes.GetNth(i));

		offset	   += packetSizes.GetNth(i);
		dataLength += packetSizes.GetNth(i);
	}

	return dataLength;
}

Bool BoCA::EncoderFDKAAC::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	if (n == 0 && AS::Registry::Get().ComponentExists("mp4-mux"))
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
	const Config	*config = GetConfiguration();

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

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureFDKAAC::ConfigID, "MPEGVersion", 0);
		config->SetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", True);

		config->SetIntValue(ConfigureFDKAAC::ConfigID, "AACType", AOT_AAC_LC);
		config->SetIntValue(ConfigureFDKAAC::ConfigID, "Bitrate", 64);
		config->SetIntValue(ConfigureFDKAAC::ConfigID, "Afterburner", True);
	}

	/* Get command line settings.
	 */
	Bool	 rawAAC	       = config->GetIntValue(encoderID, "Create ADTS AAC files (no MP4 container)", !config->GetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", True));
	Bool	 afterburner   = !config->GetIntValue(encoderID, "Disable Afterburner processing", False);

	Int	 bitrate       = config->GetIntValue(ConfigureFDKAAC::ConfigID, "Bitrate", 64);
	Int	 aacType       = config->GetIntValue(ConfigureFDKAAC::ConfigID, "AACType", AOT_AAC_LC);
	String	 aacTypeString = "LC";

	if	(aacType == AOT_SBR)	    aacTypeString = "HE";
	else if	(aacType == AOT_PS)	    aacTypeString = "HEv2";
	else if	(aacType == AOT_ER_AAC_LD)  aacTypeString = "LD";
	else if	(aacType == AOT_ER_AAC_ELD) aacTypeString = "ELD";

	if (config->GetIntValue(encoderID, "Set Bitrate per channel", False)) bitrate	    = config->GetIntValue(encoderID, "Bitrate per channel", bitrate);
	if (config->GetIntValue(encoderID, "Set AAC encoding mode", False))   aacTypeString = config->GetStringValue(encoderID, "AAC encoding mode", aacTypeString).ToUpper();

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureFDKAAC::ConfigID, "MP4Container", !rawAAC);

	if	(aacTypeString == "LC"  ) aacType = AOT_AAC_LC;
	else if (aacTypeString == "HE"  ) aacType = AOT_SBR;
	else if (aacTypeString == "HEV2") aacType = AOT_PS;
	else if (aacTypeString == "LD"  ) aacType = AOT_ER_AAC_LD;
	else if (aacTypeString == "ELD" ) aacType = AOT_ER_AAC_ELD;

	config->SetIntValue(ConfigureFDKAAC::ConfigID, "AACType", aacType);
	config->SetIntValue(ConfigureFDKAAC::ConfigID, "Bitrate", Math::Max(8, Math::Min(256, bitrate)));

	if (!afterburner) config->SetIntValue(ConfigureFDKAAC::ConfigID, "Afterburner", False);

	return True;
}

ConfigLayer *BoCA::EncoderFDKAAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureFDKAAC();

	return configLayer;
}
