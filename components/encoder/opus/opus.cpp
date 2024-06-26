 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2022 Robert Kausch <robert.kausch@freac.org>
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

#include <stdint.h>

#include "opus.h"
#include "config.h"

const String &BoCA::EncoderOpus::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && opusdll != NIL)
	{
		componentSpecs = "										\
														\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>							\
		  <component>											\
		    <name>Opus Audio Encoder %VERSION%</name>							\
		    <version>1.0</version>									\
		    <id>opus-enc</id>										\
		    <type>encoder</type>									\
		    <format>											\
		      <name>Opus Audio</name>									\
		      <extension>opus</extension>								\
		      <extension>oga</extension>								\
		      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>				\
		    </format>											\
		    <input bits=\"16\" channels=\"1-8\"/>							\
		    <parameters>										\
		      <range name=\"Bitrate\" argument=\"--bitrate %VALUE\" default=\"128\">			\
			<min>6</min>										\
			<max>510</max>										\
		      </range>											\
		      <range name=\"Encoding complexity\" argument=\"--comp %VALUE\" default=\"10\">		\
			<min alias=\"fastest\">0</min>								\
			<max alias=\"slowest\">10</max>								\
		      </range>											\
		      <selection name=\"Frame size\" argument=\"--framesize %VALUE\" default=\"20\">		\
			<option alias=\"5ms\">5</option>							\
			<option alias=\"10ms\">10</option>							\
			<option alias=\"20ms\">20</option>							\
			<option alias=\"40ms\">40</option>							\
			<option alias=\"60ms\">60</option>							\
			<option alias=\"80ms\">80</option>							\
			<option alias=\"100ms\">100</option>							\
			<option alias=\"120ms\">120</option>							\
		      </selection>										\
 		      <switch name=\"Use constrained VBR encoding\" argument=\"--cvbr\"/>			\
 		      <switch name=\"Use hard CBR encoding\" argument=\"--hard-cbr\"/>				\
 		      <switch name=\"Disable intensity stereo phase inversion\" argument=\"--no-phase-inv\"/>	\
		    </parameters>										\
		  </component>											\
														\
		";

		componentSpecs.Replace("%VERSION%", String("v").Append(String(ex_opus_get_version_string()).Replace("libopus ", NIL)));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadOggDLL();
	LoadOpusDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
	FreeOpusDLL();
}

namespace BoCA
{
	/* Opus header definition.
	 */
	struct OpusHeader
	{
		char		 codec_id[8];	  /**< MUST be "OpusHead" */
		uint8_t		 version_id;	  /**< Version number */
		uint8_t		 nb_channels;	  /**< Number of channels */
		uint16_t	 preskip;	  /**< Pre-skip */
		uint32_t	 sample_rate;	  /**< Input sample rate; informational only */
		int16_t		 output_gain;	  /**< Output gain to apply when decoding */
		uint8_t		 channel_mapping; /**< Channel mapping family */

		uint8_t		 nb_streams;	  /**< Stream count */
		uint8_t		 nb_coupled;	  /**< Two-channel stream count */
		uint8_t		 stream_map[255]; /**< Channel mapping */
	};
};

BoCA::EncoderOpus::EncoderOpus()
{
	configLayer	= NIL;
	config		= NIL;

	converter	= NIL;

	frameSize	= 0;
	preSkip		= 0;

	blockSize	= 256;
	overlap		= 24;

	totalSamples	= 0;

	nextWorker	= 0;

	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));
}

BoCA::EncoderOpus::~EncoderOpus()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderOpus::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();
	Info		 info	= track.GetInfo();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	/* Get target format.
	 */
	targetFormat = format;

	if	(format.rate <=  8000) targetFormat.rate =  8000;
	else if (format.rate <= 12000) targetFormat.rate = 12000;
	else if (format.rate <= 16000) targetFormat.rate = 16000;
	else if (format.rate <= 24000) targetFormat.rate = 24000;
	else			       targetFormat.rate = 48000;

	/* Create and init format converter component.
	 */
	converter = new FormatConverter(format, targetFormat);

	if (converter->GetErrorState() == True)
	{
		errorState  = True;
		errorString = converter->GetErrorString();

		delete converter;

		return False;
	}

	/* Init Ogg stream.
	 */
	Math::RandomSeed();

	ex_ogg_stream_init(&os, Math::Random());

	/* Create Opus header.
	 */
	OpusHeader	 setup;

	memcpy(setup.codec_id, "OpusHead", 8);

	setup.version_id  = 1;
	setup.nb_channels = format.channels;
	setup.sample_rate = format.rate;
	setup.output_gain = 0;

	if (format.channels <= 2) setup.channel_mapping = 0;
	else			  setup.channel_mapping = 1;

	/* Init Opus encoder.
	 */
	int	 error	 = 0;
	int	 streams = 0;
	int	 coupled = 0;

	OpusMSEncoder	*encoder = ex_opus_multistream_surround_encoder_create(targetFormat.rate, setup.nb_channels, setup.channel_mapping, &streams, &coupled, setup.stream_map, OPUS_APPLICATION_AUDIO, &error);

	setup.nb_streams = streams;
	setup.nb_coupled = coupled;

	/* Get number of pre-skip samples.
	 */
	ex_opus_multistream_encoder_ctl(encoder, OPUS_GET_LOOKAHEAD(&preSkip));

	setup.preskip = preSkip * (48000 / targetFormat.rate);

	frameSize     = Math::Round(Float(targetFormat.rate) / (1000000.0 / config->GetIntValue(ConfigureOpus::ConfigID, "FrameSize", 20000)));
	totalSamples  = 0;

	ex_opus_multistream_encoder_destroy(encoder);

	/* Adjust endianness of header fields.
	 */
	if (endianness != EndianLittle)
	{
		BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.preskip, sizeof(setup.preskip));
		BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.sample_rate, sizeof(setup.sample_rate));
		BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup.output_gain, sizeof(setup.output_gain));
	}

	/* Write header packet.
	 */
	ogg_packet	 header = { (unsigned char *) &setup, 19 + (setup.channel_mapping == 0 ? 0 : 2 + setup.nb_channels), 1, 0, 0, 0 };

	ex_ogg_stream_packetin(&os, &header);

	/* Write Vorbis Comment header
	 */
	{
		Buffer<unsigned char>	 vcBuffer;

		/* Remove ReplayGain information as per Opus comment spec.
		 */
		info.track_gain = NIL;
		info.track_peak = NIL;
		info.album_gain = NIL;
		info.album_peak = NIL;

		/* Render actual Vorbis Comment tag.
		 *
		 * An empty tag containing only the vendor string
		 * is rendered if Vorbis Comment tags are disabled.
		 */
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			const char	*opusVersion = ex_opus_get_version_string();

			tagger->SetConfiguration(config);
			tagger->SetVendorString(opusVersion);

			if (config->GetIntValue("Tags", "EnableVorbisComment", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))) tagger->RenderBuffer(vcBuffer, track);
			else																					    tagger->RenderBuffer(vcBuffer, Track());

			boca.DeleteComponent(tagger);
		}

		vcBuffer.Resize(vcBuffer.Size() + 8);

		memmove(vcBuffer + 8, vcBuffer, vcBuffer.Size() - 8);
		memcpy(vcBuffer, "OpusTags", 8);

		ogg_packet	 header_comm = { vcBuffer, vcBuffer.Size(), 0, 0, 0, 0 };

		ex_ogg_stream_packetin(&os, &header_comm);
	}

	WriteOggPackets(True);

	/* Get number of threads to use.
	 */
	Bool	 enableParallel	 = config->GetIntValue("Resources", "EnableParallelConversions", True);
	Bool	 enableSuperFast = config->GetIntValue("Resources", "EnableSuperFastMode", True);
	Int	 numberOfThreads = enableParallel && enableSuperFast ? config->GetIntValue("Resources", "NumberOfConversionThreads", 0) : 1;

	if (enableParallel && enableSuperFast && numberOfThreads <= 1) numberOfThreads = CPU().GetNumCores() + (CPU().GetNumLogicalCPUs() - CPU().GetNumCores()) / 2;

	/* Disable overlap if we use only one thread.
	 */
	if (numberOfThreads == 1) overlap = 0;
	else			  overlap = Math::Max(24, (Int) Math::Ceil(24.0 * 960 / frameSize));

	/* Start up worker threads.
	 */
	for (Int i = 0; i < numberOfThreads; i++) workers.Add(new SuperWorker(config, targetFormat));

	foreach (SuperWorker *worker, workers) worker->Start();

	return True;
}

Bool BoCA::EncoderOpus::Deactivate()
{
	/* Flush and clean up format converter.
	 */
	Buffer<UnsignedByte>	 buffer;

	converter->Finish(buffer);

	delete converter;

	/* Append remaining samples to samples buffer.
	 */
	Int	 samples = buffer.Size() / 2;

	samplesBuffer.Resize(samplesBuffer.Size() + samples);

	memcpy(samplesBuffer + samplesBuffer.Size() - samples, buffer, buffer.Size());

	/* Output remaining samples to encoder.
	 */
	EncodeFrames(True);

	/* Write any remaining Ogg packets.
	 */
	WriteOggPackets(True);

	ex_ogg_stream_clear(&os);

	/* Tear down worker threads.
	 */
	foreach (SuperWorker *worker, workers) worker->Quit();
	foreach (SuperWorker *worker, workers) worker->Wait();
	foreach (SuperWorker *worker, workers) delete worker;

	workers.RemoveAll();

	/* Fix chapter marks in Vorbis Comments.
	 */
	if (config->GetIntValue("Tags", "EnableVorbisComment", True) && track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
	{
		driver->Close();

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			tagger->UpdateStreamInfo(track.outputFile, track);

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Int BoCA::EncoderOpus::WriteData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	/* Change to Vorbis channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Default_3_0, Channel::Vorbis_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_0, Channel::Vorbis_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::Vorbis_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Default_6_1, Channel::Vorbis_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Default_7_1, Channel::Vorbis_7_1);

	/* Perform sample rate conversion.
	 */
	converter->Transform(data);

	/* Copy data to samples buffer.
	 */
	Int	 samples = data.Size() / 2;

	samplesBuffer.Resize(samplesBuffer.Size() + samples);

	memcpy(samplesBuffer + samplesBuffer.Size() - samples, data, data.Size());

	/* Output samples to encoder.
	 */
	return EncodeFrames(False);
}

Int BoCA::EncoderOpus::EncodeFrames(Bool flush)
{
	/* Pad end of stream with empty samples.
	 */
	Int	 nullSamples = 0;

	if (flush)
	{
		nullSamples = preSkip;

		if ((samplesBuffer.Size() / targetFormat.channels + preSkip) % frameSize > 0) nullSamples += frameSize - (samplesBuffer.Size() / targetFormat.channels + preSkip) % frameSize;

		samplesBuffer.Resize(samplesBuffer.Size() + nullSamples * targetFormat.channels);

		memset(((signed short *) samplesBuffer) + samplesBuffer.Size() - nullSamples * targetFormat.channels, 0, sizeof(short) * nullSamples * targetFormat.channels);
	}

	/* Pass samples to workers.
	 */
	Int	 framesToProcess = blockSize;
	Int	 framesProcessed = 0;
	Int	 dataLength	 = 0;

	Int	 samplesPerFrame = frameSize * targetFormat.channels;

	if (flush) framesToProcess = Math::Floor(samplesBuffer.Size() / samplesPerFrame);

	while (samplesBuffer.Size() - framesProcessed * samplesPerFrame >= samplesPerFrame * framesToProcess)
	{
		SuperWorker	*workerToUse = workers.GetNth(nextWorker % workers.Length());

		workerToUse->WaitUntilReady();

		/* See if the worker has some packets for us.
		 */
		if (workerToUse->GetPacketSizes().Length() != 0) dataLength += ProcessPackets(workerToUse->GetPackets(), workerToUse->GetPacketSizes(), nextWorker == workers.Length(), False, 0);

		/* Pass new frames to worker.
		 */
		workerToUse->Encode(samplesBuffer, framesProcessed * samplesPerFrame, samplesPerFrame * framesToProcess);

		framesProcessed += framesToProcess - (flush ? 0 : overlap);

		nextWorker++;

		if (flush) break;
	}

	memmove((signed short *) samplesBuffer, ((signed short *) samplesBuffer) + framesProcessed * samplesPerFrame, sizeof(short) * (samplesBuffer.Size() - framesProcessed * samplesPerFrame));

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
		if (workerToUse->GetPacketSizes().Length() != 0) dataLength += ProcessPackets(workerToUse->GetPackets(), workerToUse->GetPacketSizes(), nextWorker == workers.Length(), i == workers.Length() - 1, i == workers.Length() - 1 ? nullSamples : 0);

		nextWorker++;
	}

	return dataLength;
}

Int BoCA::EncoderOpus::ProcessPackets(const Buffer<unsigned char> &packets, const Array<Int> &packetSizes, Bool first, Bool flush, Int nullSamples)
{
	Int	 offset = 0;

	if (!first) for (Int i = 0; i < overlap; i++) offset += packetSizes.GetNth(i);

	for (Int i = 0; i < packetSizes.Length(); i++)
	{
		if (i <	overlap && !first)	continue;
		if (packetSizes.GetNth(i) == 0) continue;

		totalSamples += frameSize;

		op.packet     = packets + offset;
		op.bytes      = packetSizes.GetNth(i);
		op.b_o_s      = first && i == 0;
		op.e_o_s      = flush && i == packetSizes.Length() - 1;
		op.granulepos = (op.e_o_s ? totalSamples + preSkip - nullSamples : totalSamples) * (48000 / targetFormat.rate);
		op.packetno   = 0;

		ex_ogg_stream_packetin(&os, &op);	

		offset += packetSizes.GetNth(i);
	}

	return WriteOggPackets(flush);
}

Int BoCA::EncoderOpus::WriteOggPackets(Bool flush)
{
	Int	 bytes = 0;

	do
	{
		int	 result = 0;

		if (flush) result = ex_ogg_stream_flush(&os, &og);
		else	   result = ex_ogg_stream_pageout(&os, &og);

		if (result == 0) break;

		bytes += driver->WriteData(og.header, og.header_len);
		bytes += driver->WriteData(og.body, og.body_len);
	}
	while (true);

	return bytes;
}

String BoCA::EncoderOpus::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	switch (config->GetIntValue(ConfigureOpus::ConfigID, "FileExtension", 0))
	{
		default:
		case  0: return "opus";
		case  1: return "oga";
	}
}

Bool BoCA::EncoderOpus::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "opus-enc";

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureOpus::ConfigID, "Mode", 0);
		config->SetIntValue(ConfigureOpus::ConfigID, "Bandwidth", 0);
		config->SetIntValue(ConfigureOpus::ConfigID, "PacketLoss", 0);
		config->SetIntValue(ConfigureOpus::ConfigID, "EnableDTX", False);

		config->SetIntValue(ConfigureOpus::ConfigID, "EnableVBR", True);
		config->SetIntValue(ConfigureOpus::ConfigID, "EnableConstrainedVBR", False);

		config->SetIntValue(ConfigureOpus::ConfigID, "Bitrate", 128);
		config->SetIntValue(ConfigureOpus::ConfigID, "Complexity", 10);
		config->SetIntValue(ConfigureOpus::ConfigID, "FrameSize", 20000);

		config->SetIntValue(ConfigureOpus::ConfigID, "DisablePhaseInversion", False);
	}

	/* Get command line settings.
	 */
	Bool	 hardCBR	 = config->GetIntValue(encoderID, "Use hard CBR encoding", !config->GetIntValue(ConfigureOpus::ConfigID, "EnableVBR", True));
	Bool	 constrainedVBR  = config->GetIntValue(encoderID, "Use constrained VBR encoding", config->GetIntValue(ConfigureOpus::ConfigID, "EnableConstrainedVBR", False));

	Int	 bitrate	 = config->GetIntValue(ConfigureOpus::ConfigID, "Bitrate", 128);
	Int	 complexity	 = config->GetIntValue(ConfigureOpus::ConfigID, "Complexity", 10);
	Int	 framesize	 = config->GetIntValue(ConfigureOpus::ConfigID, "FrameSize", 20000) / 1000;

	if (config->GetIntValue(encoderID, "Set Bitrate", False))	      bitrate	 = config->GetIntValue(encoderID, "Bitrate", bitrate);
	if (config->GetIntValue(encoderID, "Set Encoding complexity", False)) complexity = config->GetIntValue(encoderID, "Encoding complexity", complexity);
	if (config->GetIntValue(encoderID, "Set Frame size", False))	      framesize	 = config->GetIntValue(encoderID, "Frame size", framesize);

	Bool	 disablePhaseInv = config->GetIntValue(encoderID, "Disable intensity stereo phase inversion", config->GetIntValue(ConfigureOpus::ConfigID, "DisablePhaseInversion", False));

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureOpus::ConfigID, "EnableVBR", !hardCBR);
	config->SetIntValue(ConfigureOpus::ConfigID, "EnableConstrainedVBR", constrainedVBR);

	config->SetIntValue(ConfigureOpus::ConfigID, "Bitrate", Math::Max(6, Math::Min(510, bitrate)));
	config->SetIntValue(ConfigureOpus::ConfigID, "Complexity", Math::Max(0, Math::Min(10, complexity)));
	config->SetIntValue(ConfigureOpus::ConfigID, "FrameSize", Math::Max(5, Math::Min(120, framesize)) * 1000);

	config->SetIntValue(ConfigureOpus::ConfigID, "DisablePhaseInversion", disablePhaseInv);

	return True;
}

ConfigLayer *BoCA::EncoderOpus::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureOpus();

	return configLayer;
}
