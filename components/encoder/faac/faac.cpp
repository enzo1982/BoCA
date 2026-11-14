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

#include "faac.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderFAAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (faacdll != NIL)
	{
		componentSpecs = "										\
														\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>							\
		  <component>											\
		    <name>FAAC MP4/AAC Encoder %VERSION%</name>							\
		    <version>1.0</version>									\
		    <id>faac-enc</id>										\
		    <type>encoder</type>									\
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
		    <input bits=\"16\" channels=\"1-6\"								\
			   rate=\"8000,11025,12000,16000,22050,24000,32000,44100,48000,64000,88200,96000\"/>	\
		    <parameters>										\
		      <range name=\"VBR quality\" argument=\"-q %VALUE\" default=\"150\">			\
			<min alias=\"worst\">10</min>								\
			<max alias=\"best\">500</max>								\
		      </range>											\
		      <range name=\"ABR bitrate per channel\" argument=\"-b %VALUE\" default=\"96\">		\
			<min alias=\"min\">8</min>								\
			<max alias=\"max\">256</max>								\
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

		unsigned long	 samplesSize;
		unsigned long	 bufferSize;

		faacEncHandle	 faac = ex_faacEncOpen(44100, 2, &samplesSize, &bufferSize);

		componentSpecs.Replace("%VERSION%", String("v").Append(ex_faacEncGetCurrentConfiguration(faac)->name));

		ex_faacEncClose(faac);
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadFAACDLL();
}

Void smooth::DetachDLL()
{
	FreeFAACDLL();
}

BoCA::EncoderFAAC::EncoderFAAC()
{
	configLayer  = NIL;
	config	     = NIL;

	muxer	     = NIL;

	frameSize    = 0;

	blockSize    = 128;
	overlap	     = 8;

	totalSamples = 0;

	nextWorker   = 0;
}

BoCA::EncoderFAAC::~EncoderFAAC()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderFAAC::Activate()
{
	const Format	&format = track.GetFormat();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	AS::Registry	&boca = AS::Registry::Get();

	if (!boca.ComponentExists("mp4-mux")) config->SetIntValue(ConfigureFAAC::ConfigID, "MP4Container", False);

	Bool	 mp4Container = config->GetIntValue(ConfigureFAAC::ConfigID, "MP4Container", True);
	Int	 mpegVersion  = config->GetIntValue(ConfigureFAAC::ConfigID, "MPEGVersion", 0);

	/* Create FAAC encoder.
	 */
	unsigned long	 samplesSize = 0;
	unsigned long	 bufferSize  = 0;

	faacEncHandle	 handle = ex_faacEncOpen(format.rate, format.channels, &samplesSize, &bufferSize);

	/* Set encoder parameters.
	 */
	faacEncConfigurationPtr	 fConfig = ex_faacEncGetCurrentConfiguration(handle);

	fConfig->mpegVersion	= mp4Container ? MPEG4 : mpegVersion;
	fConfig->aacObjectType	= LOW;

	ex_faacEncSetConfiguration(handle, fConfig);

	frameSize = samplesSize / format.channels;

	/* Check whether to use MP4 container.
	 */
	if (mp4Container)
	{
		/* Get codec configuration.
		 */
		unsigned char		*buffer = NIL;

		ex_faacEncGetDecoderSpecificInfo(handle, &buffer, &bufferSize);

		Buffer<UnsignedByte>	 codecConfig(bufferSize);

		memcpy(codecConfig, buffer, bufferSize);

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

			ex_faacEncClose(handle);

			return False;
		}

		totalSamples = 0;
	}

	ex_faacEncClose(handle);

	/* Write ID3v2 tag if requested.
	 */
	if (muxer == NIL && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureFAAC::ConfigID, "AllowID3v2", False))
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
	else			  overlap = 8;

	/* Start up worker threads.
	 */
	for (Int i = 0; i < numberOfThreads; i++) workers.Add(new SuperWorker(config, format));

	foreach (SuperWorker *worker, workers) worker->Start();

	return True;
}

Bool BoCA::EncoderFAAC::Deactivate()
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
		muxer->SetGaplessInfo(frameSize, frameSize - totalSamples % frameSize, totalSamples);
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
	if (!mp4Container && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureFAAC::ConfigID, "AllowID3v2", False))
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

Int BoCA::EncoderFAAC::WriteData(Buffer<UnsignedByte> &data)
{
	const Format	&format	= track.GetFormat();

	/* Change to AAC channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Default_3_0, Channel::AAC_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_0, Channel::AAC_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::AAC_5_1);

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

Int BoCA::EncoderFAAC::EncodeFrames(Bool flush)
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

Int BoCA::EncoderFAAC::ProcessPackets(const Buffer<unsigned char> &packets, const Array<Int> &packetSizes, Bool first)
{
	Int	 offset	    = 0;
	Int	 dataLength = 0;

	if (!first) for (Int i = 0; i < overlap; i++) offset += packetSizes.GetNth(i);

	for (Int i = 0; i < packetSizes.Length(); i++)
	{
		if (i <	overlap && !first)	continue;
		if (packetSizes.GetNth(i) == 0) continue;

		if (muxer != NIL) muxer->WritePacket(packets + offset, packetSizes.GetNth(i), muxer->GetDuration() + frameSize, PACKET_FLAG_NONE);
		else		  driver->WriteData(packets + offset, packetSizes.GetNth(i));

		offset	   += packetSizes.GetNth(i);
		dataLength += packetSizes.GetNth(i);
	}

	return dataLength;
}

Bool BoCA::EncoderFAAC::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	if (n == 0 && AS::Registry::Get().ComponentExists("mp4-mux"))
	{
		config->SetIntValue(ConfigureFAAC::ConfigID, "MP4Container", True);
		config->SetIntValue(ConfigureFAAC::ConfigID, "MPEGVersion", 0);
	}
	else
	{
		config->SetIntValue(ConfigureFAAC::ConfigID, "MP4Container", False);
	}

	return True;
}

String BoCA::EncoderFAAC::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (config->GetIntValue(ConfigureFAAC::ConfigID, "MP4Container", True))
	{
		switch (config->GetIntValue(ConfigureFAAC::ConfigID, "MP4FileExtension", 0))
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

Bool BoCA::EncoderFAAC::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "faac-enc";

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureFAAC::ConfigID, "MPEGVersion", 0);

		config->SetIntValue(ConfigureFAAC::ConfigID, "MP4Container", True);
		config->SetIntValue(ConfigureFAAC::ConfigID, "SetQuality", True);

		config->SetIntValue(ConfigureFAAC::ConfigID, "AACQuality", 150);
		config->SetIntValue(ConfigureFAAC::ConfigID, "Bitrate", 96);
	}

	/* Get command line settings.
	 */
	Bool	 rawAAC	 = config->GetIntValue(encoderID, "Create ADTS AAC files (no MP4 container)", !config->GetIntValue(ConfigureFAAC::ConfigID, "MP4Container", True));
	Bool	 useABR	 = config->GetIntValue(encoderID, "Set ABR bitrate per channel", !config->GetIntValue(ConfigureFAAC::ConfigID, "SetQuality", True));

	Int	 quality = config->GetIntValue(ConfigureFAAC::ConfigID, "AACQuality", 150);
	Int	 bitrate = config->GetIntValue(ConfigureFAAC::ConfigID, "Bitrate", 96);

	if (config->GetIntValue(encoderID, "Set VBR quality", False))		  quality = config->GetIntValue(encoderID, "VBR quality", quality);
	if (config->GetIntValue(encoderID, "Set ABR bitrate per channel", False)) bitrate = config->GetIntValue(encoderID, "ABR bitrate per channel", bitrate);

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureFAAC::ConfigID, "MP4Container", !rawAAC);
	config->SetIntValue(ConfigureFAAC::ConfigID, "SetQuality", !useABR);

	config->SetIntValue(ConfigureFAAC::ConfigID, "AACQuality", Math::Max(10, Math::Min(500, quality)));
	config->SetIntValue(ConfigureFAAC::ConfigID, "Bitrate", Math::Max(8, Math::Min(256, bitrate)));

	return True;
}

ConfigLayer *BoCA::EncoderFAAC::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureFAAC();

	return configLayer;
}
