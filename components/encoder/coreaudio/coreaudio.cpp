 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2017 Robert Kausch <robert.kausch@freac.org>
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
	if (corefoundationdll != NIL && coreaudiodll != NIL)
#endif
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>Core Audio AAC/ALAC Encoder</name>			\
		    <version>1.0</version>					\
		    <id>coreaudio-enc</id>					\
		    <type>encoder</type>					\
		    <replace>faac-enc</replace>					\
		    <replace>voaacenc-enc</replace>				\
		    <replace>avconv-alac-enc</replace>				\
		    <format>							\
		      <name>MPEG-4 AAC Files</name>				\
		      <extension>m4a</extension>				\
		      <extension>m4b</extension>				\
		      <extension>m4r</extension>				\
		      <extension>mp4</extension>				\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
		    </format>							\
		    <format>							\
		      <name>Raw AAC Files</name>				\
		      <extension>aac</extension>				\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>	\
		    </format>							\
		    <format>							\
		      <name>Apple Lossless Files</name>				\
		      <lossless>true</lossless>					\
		      <extension>m4a</extension>				\
		      <extension>m4b</extension>				\
		      <extension>mp4</extension>				\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
		    </format>							\
		    <format>							\
		      <name>FLAC Files</name>					\
		      <lossless>true</lossless>					\
		      <extension>flac</extension>				\
		      <tag id=\"flac-tag\" mode=\"other\">FLAC Metadata</tag>	\
		    </format>							\
		  </component>							\
										\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
#ifndef __APPLE__
	LoadCoreFoundationDLL();
	LoadCoreAudioDLL();
#endif
}

Void smooth::DetachDLL()
{
#ifndef __APPLE__
	FreeCoreFoundationDLL();
	FreeCoreAudioDLL();
#endif
}

namespace BoCA
{
	CA::OSStatus	 AudioConverterComplexInputDataProc(CA::AudioConverterRef, CA::UInt32 *, CA::AudioBufferList *, CA::AudioStreamPacketDescription **, void *);
};

BoCA::EncoderCoreAudio::EncoderCoreAudio()
{
	configLayer    = NIL;

	audioFile      = NIL;
	converter      = NIL;

	buffers	       = NIL;
	bufferSize     = 0;
	bytesConsumed  = 0;

	packetsWritten = 0;
	totalSamples   = 0;
}

BoCA::EncoderCoreAudio::~EncoderCoreAudio()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderCoreAudio::IsLossless() const
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	CA::UInt32	 codec = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);

	/* Signal lossless for ALAC and FLAC.
	 */
	if (codec == CA::kAudioFormatAppleLossless ||
	    codec == CA::kAudioFormatFLAC) return True;

	return False;
}

Bool BoCA::EncoderCoreAudio::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();

	if (format.channels > 8)
	{
		errorString = "This encoder does not support more than 8 channels!";
		errorState  = True;

		return False;
	}

	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	CA::UInt32	 codec	      = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	Int		 kbps	      = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Bitrate", 64);
	Bool		 mp4Container = config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True);

	/* Fill out source format description.
	 */
	CA::AudioStreamBasicDescription	 sourceFormat = { 0 };

	sourceFormat.mFormatID		    = CA::kAudioFormatLinearPCM;
	sourceFormat.mFormatFlags	    = CA::kLinearPCMFormatFlagIsPacked | (format.bits > 8	  ? CA::kLinearPCMFormatFlagIsSignedInteger : 0) |
										 (endianness == EndianBig ? CA::kLinearPCMFormatFlagIsBigEndian	    : 0);
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
	destinationFormat.mSampleRate	    = GetOutputSampleRate(format.rate);
	destinationFormat.mChannelsPerFrame = format.channels;

	CA::UInt32	 formatSize = sizeof(destinationFormat);

	CA::AudioFormatGetProperty(CA::kAudioFormatProperty_FormatInfo, 0, NIL, &formatSize, &destinationFormat);

	/* Create audio converter object.
	 */
	if (CA::AudioConverterNew(&sourceFormat, &destinationFormat, &converter) != 0)
	{
		errorString = "Could not create converter component!";
		errorState  = True;

		return False;
	}

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

	/* Create audio file object for output file.
	 */
	String		 fileName	= Utilities::GetNonUnicodeTempFileName(track.outfile).Append(".out");
	CA::CFStringRef	 fileNameString	= CA::CFStringCreateWithCString(NULL, fileName.ConvertTo("UTF-8"), CA::kCFStringEncodingUTF8);

#ifdef __WIN32__
	CA::CFURLRef	 fileNameURL	= CA::CFURLCreateWithFileSystemPath(NULL, fileNameString, CA::kCFURLWindowsPathStyle, False);
#else
	CA::CFURLRef	 fileNameURL	= CA::CFURLCreateWithFileSystemPath(NULL, fileNameString, CA::kCFURLPOSIXPathStyle, False);
#endif

	CA::UInt32	 fileType	= (codec == CA::kAudioFormatFLAC) ? CA::kAudioFileFLACType : 
					  (mp4Container			  ? CA::kAudioFileM4AType  :
									    CA::kAudioFileAAC_ADTSType);

	CA::AudioFileCreateWithURL(fileNameURL, fileType, &destinationFormat, CA::kAudioFileFlags_EraseFile, &audioFile);

	CA::CFRelease(fileNameURL);
	CA::CFRelease(fileNameString);

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

	/* Get maximum output packet size.
	 */
	CA::UInt32	 valueSize = 4;

	CA::AudioConverterGetProperty(converter, CA::kAudioConverterPropertyMaximumOutputPacketSize, &valueSize, &bufferSize);

	/* Set up buffer for Core Audio.
	 */
	buffers = (CA::AudioBufferList	*) new unsigned char [sizeof(CA::AudioBufferList) + sizeof(CA::AudioBuffer)];

	buffers->mNumberBuffers = 1;

	buffers->mBuffers[0].mData	     = new unsigned char [bufferSize];
	buffers->mBuffers[0].mDataByteSize   = bufferSize;
	buffers->mBuffers[0].mNumberChannels = format.channels;

	packetsWritten = 0;
	totalSamples   = 0;

	/* Write ID3v2 tag if requested.
	 */
	if (codec != CA::kAudioFormatFLAC && !mp4Container && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureCoreAudio::ConfigID, "AllowID3v2", False))
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

Bool BoCA::EncoderCoreAudio::Deactivate()
{
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	CA::UInt32	 codec	      = config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	Bool		 mp4Container = config->GetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True);

	/* Convert final frames.
	 */
	EncodeFrames(True);

	/* Free buffers.
	 */
	delete [] (unsigned char *) buffers->mBuffers[0].mData;
	delete [] (unsigned char *) buffers;

	/* Write priming and remainder info.
	 */
	CA::AudioConverterPrimeInfo	 primeInfo;
	CA::UInt32			 size = sizeof(primeInfo);

	if (CA::AudioConverterGetProperty(converter, CA::kAudioConverterPrimeInfo, &size, &primeInfo) == 0)
	{
		const Format	&format = track.GetFormat();

		Float	 divider = Float(format.rate) / GetOutputSampleRate(format.rate);
		Int	 extra	 = 0;

		if (codec == CA::kAudioFormatMPEG4AAC_HE ||
		    codec == CA::kAudioFormatMPEG4AAC_HE_V2) { divider *= 2.0; extra = 480; }

		CA::AudioFilePacketTableInfo	 pti;

		pti.mPrimingFrames     = primeInfo.leadingFrames + extra;
		pti.mRemainderFrames   = primeInfo.trailingFrames;
		pti.mNumberValidFrames = totalSamples / divider;

		CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyPacketTableInfo, sizeof(pti), &pti);
	}

	/* Get and set magic cookie again as some
	 * encoders may change it during encoding.
	 */
	CA::UInt32	 cookieSize = 4;

	if (CA::AudioConverterGetPropertyInfo(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, NIL) == 0)
	{
		unsigned char	*cookie = new unsigned char [cookieSize];

		CA::AudioConverterGetProperty(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, cookie);
		CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyMagicCookieData, cookieSize, cookie);

		delete [] cookie;
	}

	/* Close converter and audio file.
	 */
	CA::AudioConverterDispose(converter);
	CA::AudioFileClose(audioFile);

	/* Write metadata to file
	 */
	if (codec != CA::kAudioFormatFLAC && mp4Container && config->GetIntValue("Tags", "EnableMP4Metadata", True))
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

	/* Write ID3v1 tag if requested.
	 */
	if (codec != CA::kAudioFormatFLAC && !mp4Container && config->GetIntValue("Tags", "EnableID3v1", False))
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
	if (codec != CA::kAudioFormatFLAC && !mp4Container && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue(ConfigureCoreAudio::ConfigID, "AllowID3v2", False))
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

	/* Configure buffer.
	 */
	buffer.Resize(buffer.Size() + data.Size());

	memmove(buffer, buffer + bytesConsumed, buffer.Size() - bytesConsumed - data.Size());
	memcpy(buffer + buffer.Size() - bytesConsumed - data.Size(), data, data.Size());

	buffer.Resize(buffer.Size() - bytesConsumed);

	bytesConsumed = 0;

	/* Convert frames.
	 */
	return EncodeFrames(False);
}

Int BoCA::EncoderCoreAudio::EncodeFrames(Bool flush)
{
	/* Encode samples.
	 */
	Int	 totalOutBytes = 0;

	CA::UInt32				 packets = 1;
	CA::AudioStreamPacketDescription	 packet;

	buffers->mBuffers[0].mDataByteSize = bufferSize;

	while (CA::AudioConverterFillComplexBuffer(converter, &AudioConverterComplexInputDataProc, this, &packets, buffers, &packet) == 0)
	{
		if (buffers->mBuffers[0].mDataByteSize == 0) break;

		CA::AudioFileWritePackets(audioFile, False, buffers->mBuffers[0].mDataByteSize, &packet, packetsWritten, &packets, buffers->mBuffers[0].mData);

		packetsWritten += packets;
		totalOutBytes  += buffers->mBuffers[0].mDataByteSize;

		if (!flush && buffer.Size() - bytesConsumed < 65536) break;

		buffers->mBuffers[0].mDataByteSize = bufferSize;
	}

	return totalOutBytes;
}

Int BoCA::EncoderCoreAudio::GetOutputSampleRate(Int inputRate) const
{
	const Config	*config = GetConfiguration();

	/* Get supported sample rate ranges for selected codec.
	 */
	CA::UInt32	 format	= config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	CA::UInt32	 size	= 0;

	CA::AudioFormatGetPropertyInfo(CA::kAudioFormatProperty_AvailableEncodeSampleRates, sizeof(format), &format, &size);

	if (size == 0) return inputRate;

	CA::AudioValueRange	*sampleRates = new CA::AudioValueRange [size / sizeof(CA::AudioValueRange)];

	CA::AudioFormatGetProperty(CA::kAudioFormatProperty_AvailableEncodeSampleRates, sizeof(format), &format, &size, sampleRates);

	/* Find best fit output sample rate.
	 */
	Int	 outputRate = 0;

	for (UnsignedInt i = 0; i < size / sizeof(CA::AudioValueRange); i++)
	{
		/* Check if encoder supports arbitrary sample rate.
		 */
		if (sampleRates[i].mMinimum == 0 &&
		    sampleRates[i].mMaximum == 0) { outputRate = inputRate; break; }

		/* Check if input rate falls into current sample rate range.
		 */
		if (inputRate >= sampleRates[i].mMinimum &&
		    inputRate <= sampleRates[i].mMaximum) { outputRate = inputRate; break; }

		/* Check if current sample rate range fits better than previous best.
		 */
		if (Math::Abs(inputRate - sampleRates[i].mMinimum) < Math::Abs(inputRate - outputRate)) outputRate = sampleRates[i].mMinimum;
		if (Math::Abs(inputRate - sampleRates[i].mMaximum) < Math::Abs(inputRate - outputRate)) outputRate = sampleRates[i].mMaximum;
	}

	delete [] sampleRates;

	return outputRate;
}

Bool BoCA::EncoderCoreAudio::SetOutputFormat(Int n)
{
	Config	*config = Config::Get();

	if (n != 1) config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", True);
	else	    config->SetIntValue(ConfigureCoreAudio::ConfigID, "MP4Container", False);

	if	(n != 2 && (CA::UInt32) config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC) == CA::kAudioFormatAppleLossless) config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);
	else if	(n != 3 && (CA::UInt32) config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC) == CA::kAudioFormatFLAC)	       config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC);

	if	(n == 2) config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatAppleLossless);
	else if (n == 3) config->SetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatFLAC);

	return True;
}

String BoCA::EncoderCoreAudio::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	if ((CA::UInt32) config->GetIntValue(ConfigureCoreAudio::ConfigID, "Codec", CA::kAudioFormatMPEG4AAC) == CA::kAudioFormatFLAC) return "flac";

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

ConfigLayer *BoCA::EncoderCoreAudio::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureCoreAudio();

	return configLayer;
}

CA::OSStatus BoCA::AudioConverterComplexInputDataProc(CA::AudioConverterRef inAudioConverter, CA::UInt32 *ioNumberDataPackets, CA::AudioBufferList *ioData, CA::AudioStreamPacketDescription **outDataPacketDescription, void *inUserData)
{
	EncoderCoreAudio	*filter = (EncoderCoreAudio *) inUserData;
	const Format		&format = filter->track.GetFormat();

	filter->suppliedData.Resize(Math::Min(filter->buffer.Size() - filter->bytesConsumed, *ioNumberDataPackets * format.channels * (format.bits / 8)));

	memcpy(filter->suppliedData, filter->buffer + filter->bytesConsumed, filter->suppliedData.Size());

	*ioNumberDataPackets = filter->suppliedData.Size() / format.channels / (format.bits / 8);

	ioData->mBuffers[0].mData           = filter->suppliedData;
	ioData->mBuffers[0].mDataByteSize   = filter->suppliedData.Size();
	ioData->mBuffers[0].mNumberChannels = format.channels;

	filter->totalSamples  += *ioNumberDataPackets;
	filter->bytesConsumed += ioData->mBuffers[0].mDataByteSize;

	return 0;
}
