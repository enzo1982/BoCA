 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

#include "coreaudio.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderCoreAudio::GetComponentSpecs()
{
	static String	 componentSpecs;

#ifndef __APPLE__
	if (coreaudiodll != NIL)
#endif
	{
		componentSpecs = "									\
													\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
		  <component>										\
		    <name>Core Audio AAC/ALAC Encoder</name>						\
		    <version>1.0</version>								\
		    <id>coreaudio-enc</id>								\
		    <type>encoder</type>								\
		    <replace>faac-enc</replace>								\
		    <replace>voaacenc-enc</replace>							\
		    <replace>ffmpeg-alac-enc</replace>							\
		    <format>										\
		      <name>MPEG-4 AAC Files</name>							\
		      <extension>m4a</extension>							\
		      <extension>m4b</extension>							\
		      <extension>m4r</extension>							\
		      <extension>mp4</extension>							\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>				\
		    </format>										\
		    <format>										\
		      <name>Raw AAC Files</name>							\
		      <extension>aac</extension>							\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>				\
		    </format>										\
		    <format>										\
		      <name>Apple Lossless Files</name>							\
		      <lossless>true</lossless>								\
		      <extension>m4a</extension>							\
		      <extension>m4b</extension>							\
		      <extension>mp4</extension>							\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>				\
		    </format>										\
		    <parameters>									\
		      <selection name=\"Output format\" argument=\"-f %VALUE\" default=\"AAC\">		\
			<option alias=\"Advanced Audio Coding\">AAC</option>				\
			<option alias=\"Apple Lossless Audio Codec\">ALAC</option>			\
		      </selection>									\
		      <range name=\"Bitrate per channel\" argument=\"-b %VALUE\" default=\"64\">	\
			<min alias=\"min\">8</min>							\
			<max alias=\"max\">192</max>							\
		      </range>										\
		      <switch name=\"Write raw AAC files\" argument=\"--raw\"/>				\
		    </parameters>									\
		  </component>										\
													\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadCoreAudioDLL();
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeCoreAudioDLL();
	FreeMP4v2DLL();
}

namespace BoCA
{
	CA::OSStatus	 AudioFileReadProc(void *, CA::SInt64, CA::UInt32, void *, CA::UInt32 *);
	CA::OSStatus	 AudioFileWriteProc(void *, CA::SInt64, CA::UInt32, const void *, CA::UInt32 *);
	CA::SInt64	 AudioFileGetSizeProc(void *);
	CA::OSStatus	 AudioFileSetSizeProc(void *, CA::SInt64);
};

BoCA::EncoderCoreAudio::EncoderCoreAudio()
{
	configLayer    = NIL;
	config	       = NIL;

	audioFile      = NIL;

	fileType       = 0;

	dataOffset     = 0;

	frameSize      = 0;

	blockSize      = 256;
	overlap	       = 24;

	nextWorker     = 0;

	packetsWritten = 0;
	packetsMissing = 0;

	totalSamples   = 0;
}

BoCA::EncoderCoreAudio::~EncoderCoreAudio()
{
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderCoreAudio::IsLossless() const
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	CA::UInt32	 codec = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);

	/* Signal lossless for ALAC.
	 */
	if (codec == CA::kAudioFormatAppleLossless) return True;

	return False;
}

Bool BoCA::EncoderCoreAudio::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();

	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	CA::UInt32	 codec	      = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	Int		 kbps	      = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Bitrate", 64);
	Bool		 mp4Container = config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True);

	/* Fill out source format description.
	 */
	CA::AudioStreamBasicDescription	 sourceFormat = { 0 };

	sourceFormat.mFormatID		    = CA::kAudioFormatLinearPCM;
	sourceFormat.mFormatFlags	    = CA::kLinearPCMFormatFlagIsPacked;
	sourceFormat.mFormatFlags	   |= format.fp			? CA::kLinearPCMFormatFlagIsFloat	  : 0;
	sourceFormat.mFormatFlags	   |= format.sign && !format.fp ? CA::kLinearPCMFormatFlagIsSignedInteger : 0;
	sourceFormat.mFormatFlags	   |= endianness == EndianBig	? CA::kLinearPCMFormatFlagIsBigEndian     : 0;
	sourceFormat.mSampleRate	    = format.rate;
	sourceFormat.mChannelsPerFrame	    = format.channels;
	sourceFormat.mBitsPerChannel	    = format.bits;
	sourceFormat.mFramesPerPacket	    = 1;
	sourceFormat.mBytesPerFrame	    = sourceFormat.mChannelsPerFrame * sourceFormat.mBitsPerChannel / 8;
	sourceFormat.mBytesPerPacket	    = sourceFormat.mFramesPerPacket * sourceFormat.mBytesPerFrame;

	/* Fill out destination format description.
	 */
	CA::AudioStreamBasicDescription	 destinationFormat = { 0 };

	destinationFormat.mFormatID	    = codec;
	destinationFormat.mSampleRate	    = SuperWorker::GetOutputSampleRate(destinationFormat.mFormatID, format.rate);
	destinationFormat.mChannelsPerFrame = format.channels;

	CA::UInt32	 formatSize = sizeof(destinationFormat);

	CA::AudioFormatGetProperty(CA::kAudioFormatProperty_FormatInfo, 0, NIL, &formatSize, &destinationFormat);

	/* Create audio converter object.
	 */
	CA::AudioConverterRef	 converter = NIL;

	if (CA::AudioConverterNew(&sourceFormat, &destinationFormat, &converter) != 0)
	{
		errorString = "Could not create converter component!";
		errorState  = True;

		return False;
	}

	frameSize = destinationFormat.mFramesPerPacket;

	/* Set bitrate if format does support bitrates.
	 */
	CA::UInt32	 size = 0;

	if (CA::AudioConverterGetPropertyInfo(converter, CA::kAudioConverterApplicableEncodeBitRates, &size, NIL) == 0)
	{
		/* Get applicable bitrate values.
		 */
		CA::UInt32		 bitrate       = kbps * 1000 * format.channels;
		CA::AudioValueRange	*bitrateValues = new CA::AudioValueRange [size / sizeof(CA::AudioValueRange)];

		CA::AudioConverterGetProperty(converter, CA::kAudioConverterApplicableEncodeBitRates, &size, bitrateValues);

		/* Find best supported bitrate.
		 */
		CA::Float64	 nearest = 0xFFFFFFFF;

		for (UnsignedInt i = 0; i < size / sizeof(CA::AudioValueRange); i++)
		{
			if (bitrate >= bitrateValues[i].mMinimum && bitrate <= bitrateValues[i].mMaximum)  nearest = bitrate;

			if (Math::Abs(bitrate - bitrateValues[i].mMinimum) < Math::Abs(bitrate - nearest)) nearest = bitrateValues[i].mMinimum;
			if (Math::Abs(bitrate - bitrateValues[i].mMaximum) < Math::Abs(bitrate - nearest)) nearest = bitrateValues[i].mMaximum;
		}

		bitrate = nearest;

		delete [] bitrateValues;

		/* Set bitrate on converter.
		 */
		CA::AudioConverterSetProperty(converter, CA::kAudioConverterEncodeBitRate, sizeof(CA::UInt32), &bitrate);
	}

	/* Get file type of output file.
	 */
	fileType = mp4Container ? CA::kAudioFileM4AType : CA::kAudioFileAAC_ADTSType;

	/* Write ID3v2 tag if requested.
	 */
	if (fileType == CA::kAudioFileAAC_ADTSType && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureCoreAudio::ConfigID, "AllowID3v2", False))
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

				dataOffset = id3Buffer.Size();

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Create audio file object for output file.
	 */
	CA::AudioFileInitializeWithCallbacks(this, AudioFileReadProc, AudioFileWriteProc, AudioFileGetSizeProc, AudioFileSetSizeProc, fileType, &destinationFormat, 0, &audioFile);

	/* Get magic cookie and supply it to audio file.
	 */
	CA::UInt32	 cookieSize = 0;

	if (CA::AudioConverterGetPropertyInfo(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, NIL) == 0)
	{
		unsigned char	*cookie = new unsigned char [cookieSize];

		CA::AudioConverterGetProperty(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, cookie);
		CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyMagicCookieData, cookieSize, cookie);

		delete [] cookie;
	}

	CA::AudioConverterDispose(converter);

	packetsWritten = 0;
	packetsMissing = 0;

	totalSamples   = 0;

	/* Get number of threads to use.
	 */
	Bool	 enableParallel	 = config->GetIntValue("Resources", "EnableParallelConversions", True);
	Bool	 enableSuperFast = config->GetIntValue("Resources", "EnableSuperFastMode", True) && format.rate == destinationFormat.mSampleRate;
	Int	 numberOfThreads = enableParallel && enableSuperFast ? config->GetIntValue("Resources", "NumberOfConversionThreads", 0) : 1;

	if (enableParallel && enableSuperFast && numberOfThreads <= 1) numberOfThreads = CPU().GetNumCores() + (CPU().GetNumLogicalCPUs() - CPU().GetNumCores()) / 2;

	/* Disable overlap if we use only one thread.
	 */
	if (numberOfThreads == 1) overlap = 0;
	else			  overlap = 24;

	/* Start up worker threads.
	 */
	for (Int i = 0; i < numberOfThreads; i++) workers.Add(new SuperWorker(config, format));

	foreach (SuperWorker *worker, workers) worker->Start();

	return True;
}

Bool BoCA::EncoderCoreAudio::Deactivate()
{
	/* Get configuration.
	 */
	CA::UInt32	 codec = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);

	/* Convert final frames.
	 */
	EncodeFrames(True);

	/* Calculate frame size divider.
	 */
	const Format	&format = track.GetFormat();

	Int	 rate	  = SuperWorker::GetOutputSampleRate(codec, format.rate);

	if (rate == 0) rate = format.rate;

	Float	 divider  = Float(format.rate) / rate;

	if (codec == CA::kAudioFormatMPEG4AAC_HE ||
	    codec == CA::kAudioFormatMPEG4AAC_HE_V2) divider *= 2.0;

	/* Write priming and remainder info.
	 */
	CA::AudioConverterPrimeInfo	 initialInfo = workers.GetNth(0					 )->GetPrimeInfo();
	CA::AudioConverterPrimeInfo	 finalInfo   = workers.GetNth((nextWorker - 1) % workers.Length())->GetPrimeInfo();

	if (initialInfo.leadingFrames != 0xFFFFFFFF && finalInfo.trailingFrames != 0xFFFFFFFF)
	{
		CA::AudioFilePacketTableInfo	 pti;

		pti.mPrimingFrames     = initialInfo.leadingFrames;
		pti.mRemainderFrames   = finalInfo.trailingFrames;
		pti.mNumberValidFrames = Math::Ceil(totalSamples / divider);

		CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyPacketTableInfo, sizeof(pti), &pti);
	}

	/* Get and set magic cookie again as some
	 * encoders may change it during encoding.
	 */
	CA::UInt32	 cookieSize = 4;
	unsigned char	*cookie	    = workers.GetFirst()->GetMagicCookie(&cookieSize);

	if (cookie != NIL)
	{
		CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyMagicCookieData, cookieSize, cookie);

		delete [] cookie;
	}

	/* Tear down worker threads.
	 */
	foreach (SuperWorker *worker, workers) worker->Quit();
	foreach (SuperWorker *worker, workers) worker->Wait();
	foreach (SuperWorker *worker, workers) delete worker;

	workers.RemoveAll();

	/* Close audio file.
	 */
	CA::AudioFileClose(audioFile);

	/* Finish MP4 writing.
	 */
	if (fileType == CA::kAudioFileM4AType)
	{
		driver->Close();

		/* Fix mhdr atom for long running tracks.
		 */
		Int64	 duration = Int64(packetsWritten) * frameSize / divider;

		if (duration > 0xFFFFFFFF)
		{
			String		 tempFile = String(track.outputFile).Append(".temp");

			InStream	 in(STREAM_FILE, track.outputFile, IS_READ);
			OutStream	 out(STREAM_FILE, tempFile, OS_REPLACE);

			Bool		 result = FixupDurationAtoms(duration, in, out, in.Size());

			in.Close();
			out.Close();

			if (result)
			{
				File(track.outputFile).Delete();
				File(tempFile).Move(track.outputFile);
			}

			File(tempFile).Delete();
		}

		/* Write metadata to file.
		 */
		const Info	&info = track.GetInfo();

		if (config->GetIntValue("Tags", "EnableMP4Metadata", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))))
		{
			AS::Registry		&boca = AS::Registry::Get();
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
			String	 tempFile = String(track.outputFile).Append(".temp");

			ex_MP4Optimize(track.outputFile.ConvertTo("UTF-8"), tempFile.ConvertTo("UTF-8"));

			File(track.outputFile).Delete();
			File(tempFile).Move(track.outputFile);
		}
	}

	/* Write ID3v1 tag if requested.
	 */
	if (fileType == CA::kAudioFileAAC_ADTSType && config->GetIntValue("Tags", "EnableID3v1", False))
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
	if (fileType == CA::kAudioFileAAC_ADTSType && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureCoreAudio::ConfigID, "AllowID3v2", False))
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

Int BoCA::EncoderCoreAudio::WriteData(Buffer<UnsignedByte> &data)
{
	const Format	&format	= track.GetFormat();

	/* Change to AAC channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Default_3_0, Channel::AAC_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_0, Channel::AAC_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::AAC_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Default_6_1, Channel::AAC_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Default_7_1, Channel::AAC_7_1);

	/* Copy data to samples buffer.
	 */
	samplesBuffer.Resize(samplesBuffer.Size() + data.Size());

	memcpy(samplesBuffer + samplesBuffer.Size() - data.Size(), data, data.Size());

	/* Output samples to encoder.
	 */
	totalSamples += data.Size() / format.channels / (format.bits / 8);

	return EncodeFrames(False);
}

Int BoCA::EncoderCoreAudio::EncodeFrames(Bool flush)
{
	const Format	&format = track.GetFormat();

	/* Pass samples to workers.
	 */
	Int	 framesToProcess = blockSize;
	Int	 framesProcessed = 0;
	Int	 dataLength	 = 0;

	Int	 bytesPerFrame = frameSize * format.channels * (format.bits / 8);

	if (flush) framesToProcess = Math::Floor(samplesBuffer.Size() / bytesPerFrame);

	while (samplesBuffer.Size() - framesProcessed * bytesPerFrame >= bytesPerFrame * framesToProcess)
	{
		SuperWorker	*workerToUse = workers.GetNth(nextWorker % workers.Length());

		workerToUse->WaitUntilReady();

		/* See if the worker has some packets for us.
		 */
		if (workerToUse->GetPacketSizes().Length() != 0) dataLength += ProcessPackets(workerToUse->GetPackets(), workerToUse->GetPacketSizes(), workerToUse->GetPacketInfos(), nextWorker == workers.Length(), nextWorker >= 2 * workers.Length());

		/* Pass new frames to worker.
		 */
		workerToUse->Encode(samplesBuffer, framesProcessed * bytesPerFrame, flush ? samplesBuffer.Size() : bytesPerFrame * framesToProcess, flush);

		framesProcessed += framesToProcess - (flush ? 0 : overlap);

		nextWorker++;

		if (flush) break;
	}

	memmove(samplesBuffer, samplesBuffer + framesProcessed * bytesPerFrame, samplesBuffer.Size() - framesProcessed * bytesPerFrame);

	samplesBuffer.Resize(samplesBuffer.Size() - framesProcessed * bytesPerFrame);

	if (!flush) return dataLength;

	/* Wait for workers to finish and process packets.
	 */
	for (Int i = 0; i < workers.Length(); i++)
	{
		SuperWorker	*workerToUse = workers.GetNth(nextWorker % workers.Length());

		workerToUse->WaitUntilReady();

		/* See if the worker has some packets for us.
		 */
		if (workerToUse->GetPacketSizes().Length() != 0) dataLength += ProcessPackets(workerToUse->GetPackets(), workerToUse->GetPacketSizes(), workerToUse->GetPacketInfos(), nextWorker == workers.Length(), nextWorker >= 2 * workers.Length());

		nextWorker++;
	}

	return dataLength;
}

Int BoCA::EncoderCoreAudio::ProcessPackets(const Buffer<unsigned char> &packetData, const Array<Int> &packetSizes, const Array<CA::AudioStreamPacketDescription *> &packetInfos, Bool first, Bool discardMissing)
{
	Int	 offset	    = 0;
	Int	 dataLength = 0;

	if (discardMissing) packetsMissing = 0;

	if (!first) for (Int i = 0; i < overlap - packetsMissing; i++) offset += packetSizes.GetNth(i);

	for (Int i = 0; i < packetSizes.Length(); i++)
	{
		if (i <	overlap - packetsMissing && !first) continue;
		if (packetSizes.GetNth(i) == 0)		    continue;

		CA::UInt32				 packets = 1;
		CA::AudioStreamPacketDescription	*packet	 = packetInfos.GetNth(i);

		CA::AudioFileWritePackets(audioFile, False, packetSizes.GetNth(i), packet, packetsWritten, &packets, (unsigned char *) packetData + offset);

		offset	   += packetSizes.GetNth(i);
		dataLength += packetSizes.GetNth(i);

		packetsWritten += packets;
	}

	packetsMissing = blockSize - packetSizes.Length();

	return dataLength;
}

Bool BoCA::EncoderCoreAudio::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	if (n != 1) config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True);
	else	    config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", False);

	if	(n != 2 && (CA::UInt32) config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC) == CA::kAudioFormatAppleLossless) config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	else if (n == 2)																       config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatAppleLossless);

	return True;
}

String BoCA::EncoderCoreAudio::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if (config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True))
	{
		switch (config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4FileExtension", 0))
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

Bool BoCA::EncoderCoreAudio::FixupDurationAtoms(Int64 duration, InStream &in, OutStream &out, Int64 size, Bool seenMdat)
{
	Int64			 bytesLeft = size;
	Buffer<UnsignedByte>	 buffer(65536);

	while (bytesLeft > 0)
	{
		UnsignedInt64	 bytes	  = UnsignedInt32(in.InputNumberRaw(4));
		UnsignedInt32	 id	  = in.InputNumberRaw(4);
		Bool		 use64bit = (bytes == 1);
		Int		 grow	  = 0;

		if (use64bit == 1) bytes = in.InputNumberRaw(8);

		if	(id == 'moov') grow = 36;
		else if (id == 'trak') grow = 24;
		else if (id == 'mdia' ||
			 id == 'tkhd' ||
			 id == 'mdhd' ||
			 id == 'mvhd') grow = 12;

		out.OutputNumberRaw(use64bit ? 1 : bytes + grow, 4);
		out.OutputNumberRaw(id, 4);

		if (use64bit)
		{
			out.OutputNumberRaw(bytes + grow, 8);

			bytes	  -= 8;
			bytesLeft -= 8;
		}

		bytes	  -= 8;
		bytesLeft -= 8;

		/* Recurse into moov, trak, mdia, minf and stbl boxes.
		 */
		if (id == 'moov' ||
		    id == 'trak' ||
		    id == 'mdia' ||
		    id == 'minf' ||
		    id == 'stbl')
		{
			if (!FixupDurationAtoms(duration, in, out, bytes, seenMdat)) return False;

			bytesLeft -= bytes;

			continue;
		}

		/* Fixup duration atoms.
		 */
		if (id == 'tkhd' ||
		    id == 'mdhd' ||
		    id == 'mvhd')
		{
			Int	 size = 0;

			// Version.
			if (in.InputNumberRaw(1) >= 1) return False;

			out.OutputNumberRaw(1, 1);
			size += 1;

			// Flags.
			out.OutputNumberRaw(in.InputNumberRaw(3), 3);
			size += 3;

			// Creation time.
			out.OutputNumberRaw(0, 4);
			out.OutputNumberRaw(in.InputNumberRaw(4), 4);
			size += 4;

			// Modification time.
			out.OutputNumberRaw(0, 4);
			out.OutputNumberRaw(in.InputNumberRaw(4), 4);
			size += 4;

			// Time scale.
			out.OutputNumberRaw(in.InputNumberRaw(4), 4);
			size += 4;

			// Reserved.
			if (id == 'tkhd')
			{
				out.OutputNumberRaw(in.InputNumberRaw(4), 4);
				size += 4;
			}

			// Duration.
			in.RelSeek(4);
			out.OutputNumberRaw(duration, 8);
			size += 4;

			// Rest of atom.
			in.InputData(buffer, bytes - size);
			out.OutputData(buffer, bytes - size);

			bytesLeft -= bytes;

			continue;
		}

		/* Fixup chunk offset atoms.
		 */
		if (!seenMdat && (id == 'stco' ||
				  id == 'co64'))
		{
			// Version.
			out.OutputNumberRaw(in.InputNumberRaw(1), 1);

			// Flags.
			out.OutputNumberRaw(in.InputNumberRaw(3), 3);

			// Number of entries.
			UnsignedInt32	 numEntries = in.InputNumberRaw(4);

			out.OutputNumberRaw(numEntries, 4);

			// Entries.
			if (id == 'stco') for (UnsignedInt32 i = 0; i < numEntries; i++) out.OutputNumberRaw(in.InputNumberRaw(4) + 36, 4);
			else		  for (UnsignedInt32 i = 0; i < numEntries; i++) out.OutputNumberRaw(in.InputNumberRaw(8) + 36, 8);

			bytesLeft -= bytes;

			continue;
		}

		/* Copy other boxes/atoms.
		 */
		if (id == 'mdat') seenMdat = True;

		while (bytes > 0)
		{
			Int	 size = Math::Min(bytes, buffer.Size());

			in.InputData(buffer, size);
			out.OutputData(buffer, size);

			bytes	  -= size;
			bytesLeft -= size;
		}
	}

	return True;
}

Bool BoCA::EncoderCoreAudio::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "coreaudio-enc";

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True);

		config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
		config->SetIntValue(ConfigureCoreAudio::ConfigID, "Bitrate", 64);
	}

	/* Get command line settings.
	 */
	Bool	 rawAAC	    = config->GetIntValue(encoderID, "Write raw AAC files", !config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True));

	Int	 bitrate    = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Bitrate", 64);
	Int	 format	    = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	String	 formatName = "AAC";

	if (format == CA::kAudioFormatAppleLossless) formatName = "ALAC";

	if (config->GetIntValue(encoderID, "Set Bitrate per channel", False)) bitrate	 = config->GetIntValue(encoderID, "Bitrate per channel", bitrate);
	if (config->GetIntValue(encoderID, "Set Output format", False))	      formatName = config->GetStringValue(encoderID, "Output format", formatName).ToUpper();

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", !rawAAC || formatName == "ALAC");

	if	(formatName == "AAC" ) format = CA::kAudioFormatMPEG4AAC;
	else if (formatName == "ALAC") format = CA::kAudioFormatAppleLossless;

	config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", format);
	config->SetIntValue(ConfigureCoreAudio::ConfigID, "Bitrate", Math::Max(8, Math::Min(256, bitrate)));

	return True;
}

ConfigLayer *BoCA::EncoderCoreAudio::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureCoreAudio();

	return configLayer;
}

CA::OSStatus BoCA::AudioFileReadProc(void *inClientData, CA::SInt64 inPosition, CA::UInt32 requestCount, void *buffer, CA::UInt32 *actualCount)
{
	EncoderCoreAudio	*filter = (EncoderCoreAudio *) inClientData;

	filter->driver->Seek(inPosition + filter->dataOffset);

	*actualCount = filter->driver->ReadData((UnsignedByte *) buffer, requestCount);

	return 0;
}

CA::OSStatus BoCA::AudioFileWriteProc(void *inClientData, CA::SInt64 inPosition, CA::UInt32 requestCount, const void *buffer, CA::UInt32 *actualCount)
{
	EncoderCoreAudio	*filter = (EncoderCoreAudio *) inClientData;

	filter->driver->Seek(inPosition + filter->dataOffset);

	*actualCount = filter->driver->WriteData((UnsignedByte *) buffer, requestCount);

	return 0;
}

CA::SInt64 BoCA::AudioFileGetSizeProc(void *inClientData)
{
	EncoderCoreAudio	*filter = (EncoderCoreAudio *) inClientData;

	return filter->driver->GetSize() - filter->dataOffset;
}

CA::OSStatus BoCA::AudioFileSetSizeProc(void *inClientData, CA::SInt64 inSize)
{
	EncoderCoreAudio	*filter = (EncoderCoreAudio *) inClientData;

	filter->driver->Truncate(inSize + filter->dataOffset);

	return 0;
}
