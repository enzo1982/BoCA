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
		      <name>MP4 Audio Files</name>				\
		      <extension>m4a</extension>				\
		      <extension>m4b</extension>				\
		      <extension>m4r</extension>				\
		      <extension>mp4</extension>				\
		      <extension>3gp</extension>				\
		      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
		    </format>							\
		    <format>							\
		      <name>Advanced Audio Files</name>				\
		      <extension>aac</extension>				\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>	\
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

Bool BoCA::EncoderCoreAudio::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format = track.GetFormat();

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	Config	*config = Config::Get();

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

	destinationFormat.mFormatID	    = config->GetIntValue("CoreAudio", "Codec", 'aac ');
	destinationFormat.mSampleRate	    = GetOutputSampleRate(format.rate);
	destinationFormat.mChannelsPerFrame = format.channels;

	CA::UInt32	 formatSize = sizeof(destinationFormat);

	CA::AudioFormatGetProperty(CA::kAudioFormatProperty_FormatInfo, 0, NIL, &formatSize, &destinationFormat);

	/* Create audio converter object.
	 */
	CA::OSStatus	 status = CA::AudioConverterNew(&sourceFormat, &destinationFormat, &converter);

	if (status != 0)
	{
		errorString = "Could not create converter component!";
		errorState  = True;

		return False;
	}

	/* Set bitrate if format does support bitrates.
	 */
	CA::UInt32	 bitratesSize = 0;

	if (CA::AudioFormatGetPropertyInfo(CA::kAudioFormatProperty_AvailableEncodeBitRates, sizeof(destinationFormat.mFormatID), &destinationFormat.mFormatID, &bitratesSize) == 0)
	{
		CA::UInt32	 bitrate = config->GetIntValue("CoreAudio", "Bitrate", 128) * 1000;

		status = CA::AudioConverterSetProperty(converter, CA::kAudioConverterEncodeBitRate, sizeof(CA::UInt32), &bitrate);

		if (status != 0)
		{
			CA::AudioConverterDispose(converter);

			errorString = "Selected bitrate is not supported for current sample format!";
			errorState  = True;

			return False;
		}
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

	CA::UInt32	 fileType = config->GetIntValue("CoreAudio", "MP4Container", 1) ? CA::kAudioFileM4AType : CA::kAudioFileAAC_ADTSType;

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
	if (!config->GetIntValue("CoreAudio", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("CoreAudio", "AllowID3v2", 0))
	{
		const Info	&info = track.GetInfo();

		if ((track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)) ||
		    (info.artist != NIL || info.title != NIL))
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

Bool BoCA::EncoderCoreAudio::Deactivate()
{
	Config	*config = Config::Get();

	/* Convert final frames.
	 */
	CA::UInt32				 packets = 1;
	CA::AudioStreamPacketDescription	 packet;

	buffers->mBuffers[0].mDataByteSize = bufferSize;

	while (CA::AudioConverterFillComplexBuffer(converter, &AudioConverterComplexInputDataProc, this, &packets, buffers, &packet) == 0)
	{
		if (buffers->mBuffers[0].mDataByteSize == 0) break;

		CA::AudioFileWritePackets(audioFile, False, buffers->mBuffers[0].mDataByteSize, &packet, packetsWritten, &packets, buffers->mBuffers[0].mData);

		packetsWritten += packets;

		buffers->mBuffers[0].mDataByteSize = bufferSize;
	}

	delete [] (unsigned char *) buffers->mBuffers[0].mData;
	delete [] (unsigned char *) buffers;

	/* Write priming and remainder info.
	 */
	CA::AudioConverterPrimeInfo	 primeInfo;
	CA::UInt32			 size = sizeof(primeInfo);

	if (CA::AudioConverterGetProperty(converter, CA::kAudioConverterPrimeInfo, &size, &primeInfo) == 0)
	{
		Int	 divider = 1;
		Int	 extra	 = 0;

		if (config->GetIntValue("CoreAudio", "Codec", 'aac ') == 'aach' ||
		    config->GetIntValue("CoreAudio", "Codec", 'aac ') == 'aacp') { divider = 2; extra = 480; }

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
	if (config->GetIntValue("CoreAudio", "MP4Container", 1) && config->GetIntValue("Tags", "EnableMP4Metadata", True))
	{
		const Info	&info = track.GetInfo();

		if ((track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)) ||
		    (info.artist != NIL || info.title != NIL))
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
	if (!config->GetIntValue("CoreAudio", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v1", False))
	{
		const Info	&info = track.GetInfo();

		if (info.artist != NIL || info.title != NIL)
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->RenderBuffer(id3Buffer, track);

				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	/* Update ID3v2 tag with correct chapter marks.
	 */
	if (!config->GetIntValue("CoreAudio", "MP4Container", 1) && config->GetIntValue("Tags", "EnableID3v2", True) && config->GetIntValue("CoreAudio", "AllowID3v2", 0))
	{
		if (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				Buffer<unsigned char>	 id3Buffer;

				tagger->RenderBuffer(id3Buffer, track);

				driver->Seek(0);
				driver->WriteData(id3Buffer, id3Buffer.Size());

				boca.DeleteComponent(tagger);
			}
		}
	}

	return True;
}

Int BoCA::EncoderCoreAudio::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	/* Configure buffer.
	 */
	buffer.Resize(buffer.Size() + size);

	memmove(buffer, buffer + bytesConsumed, buffer.Size() - bytesConsumed - size);
	memcpy(buffer + buffer.Size() - bytesConsumed - size, data, size);

	buffer.Resize(buffer.Size() - bytesConsumed);

	bytesConsumed = 0;

	/* Convert frames.
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

		if (buffer.Size() - bytesConsumed < 65536) break;

		buffers->mBuffers[0].mDataByteSize = bufferSize;
	}

	return totalOutBytes;
}

Int BoCA::EncoderCoreAudio::GetOutputSampleRate(Int inputRate)
{
	Config	*config = Config::Get();

	/* Get supported sample rate ranges for selected codec.
	 */
	CA::UInt32	 format	= config->GetIntValue("CoreAudio", "Codec", 'aac ');
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

String BoCA::EncoderCoreAudio::GetOutputFileExtension()
{
	Config	*config = Config::Get();

	if (config->GetIntValue("CoreAudio", "MP4Container", 1))
	{
		switch (config->GetIntValue("CoreAudio", "MP4FileExtension", 0))
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
