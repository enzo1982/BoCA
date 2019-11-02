 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License as
  * published by the Free Software Foundation, either version 2 of
  * the License, or (at your option) any later version.
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include "dllinterface.h"
#include "communication.h"

#ifdef __WINE__
#	include <sys/mman.h>
#	include <unistd.h>
#	include <fcntl.h>
#else
#	include <io.h>

#	define ftruncate _chsize
#endif

#include <stdio.h>
#include <math.h>

#ifndef min
#	define min(n, m) ((n) < (m) ? (n) : (m))
#endif

static CoreAudioCommSetup	 setup		= { 0 };

static unsigned int		 dataOffset	= 0;

static unsigned char		*buffer		= NULL;
static unsigned int		 bufferSize	= 0;
static unsigned int		 bytesConsumed	= 0;

static CA::SInt64		 totalSamples	= 0;

void QueryCoreAudioCodecs(CoreAudioCommCodecs *comm)
{
	ZeroMemory(comm, sizeof(CoreAudioCommCodecs));

	/* Get supported codecs.
	 */
	CA::UInt32	 size = 0;

	CA::AudioFormatGetPropertyInfo(CA::kAudioFormatProperty_EncodeFormatIDs, 0, NULL, &size);

	CA::UInt32	*formats = (CA::UInt32 *) malloc(size);

	CA::AudioFormatGetProperty(CA::kAudioFormatProperty_EncodeFormatIDs, 0, NULL, &size, formats);

	for (CA::UInt32 i = 0; i < size / sizeof(CA::UInt32) && i < 32; i++)
	{
		comm->codecs[i] = formats[i];

		/* Get bitrate ranges for each codec.
		 */
		CA::UInt32	 brSize = 0;

		CA::AudioFormatGetPropertyInfo(CA::kAudioFormatProperty_AvailableEncodeBitRates, sizeof(CA::UInt32), &formats[i], &brSize);

		CA::AudioValueRange	*bitrateValues = (CA::AudioValueRange *) malloc(brSize);

		CA::AudioFormatGetProperty(CA::kAudioFormatProperty_AvailableEncodeBitRates, sizeof(CA::UInt32), &formats[i], &brSize, bitrateValues);

		for (CA::UInt32 j = 0; j < brSize / sizeof(CA::AudioValueRange) && j < 64; j++)
		{
			if (bitrateValues[j].mMinimum / 1000 > 192 && bitrateValues[j].mMaximum / 1000 > 192) continue;
			if (					      bitrateValues[j].mMaximum / 1000 > 192) bitrateValues[j].mMaximum = 192 * 1000;

			comm->bitrates[i][j * 2    ] = bitrateValues[j].mMinimum;
			comm->bitrates[i][j * 2 + 1] = bitrateValues[j].mMaximum;
		}

		free(bitrateValues);
	}

	free(formats);
}

int GetOutputSampleRate(const CoreAudioCommSetup &setup)
{
	/* Get supported sample rate ranges for selected codec.
	 */
	CA::UInt32	 format	= setup.codec;
	CA::UInt32	 size	= 0;

	CA::AudioFormatGetPropertyInfo(CA::kAudioFormatProperty_AvailableEncodeSampleRates, sizeof(format), &format, &size);

	if (size == 0) return setup.rate;

	CA::AudioValueRange	*sampleRates = (CA::AudioValueRange *) malloc(size);

	CA::AudioFormatGetProperty(CA::kAudioFormatProperty_AvailableEncodeSampleRates, sizeof(format), &format, &size, sampleRates);

	/* Find best fit output sample rate.
	 */
	int	 outputRate = 0;

	for (CA::UInt32 i = 0; i < size / sizeof(CA::AudioValueRange); i++)
	{
		/* Check if encoder supports arbitrary sample rate.
		 */
		if (sampleRates[i].mMinimum == 0 &&
		    sampleRates[i].mMaximum == 0) { outputRate = setup.rate; break; }

		/* Check if input rate falls into current sample rate range.
		 */
		if (setup.rate >= sampleRates[i].mMinimum &&
		    setup.rate <= sampleRates[i].mMaximum) { outputRate = setup.rate; break; }

		/* Check if current sample rate range fits better than previous best.
		 */
		if (abs(setup.rate - (int) sampleRates[i].mMinimum) < abs(setup.rate - outputRate)) outputRate = sampleRates[i].mMinimum;
		if (abs(setup.rate - (int) sampleRates[i].mMaximum) < abs(setup.rate - outputRate)) outputRate = sampleRates[i].mMaximum;
	}

	free(sampleRates);

	return outputRate;
}

CA::OSStatus AudioFileReadProc(void *inClientData, CA::SInt64 inPosition, CA::UInt32 requestCount, void *buffer, CA::UInt32 *actualCount)
{
	FILE	*file = (FILE *) inClientData;

	fseek(file, inPosition + dataOffset, SEEK_SET);

	*actualCount = fread(buffer, 1, requestCount, file);

	return 0;
}

CA::OSStatus AudioFileWriteProc(void *inClientData, CA::SInt64 inPosition, CA::UInt32 requestCount, const void *buffer, CA::UInt32 *actualCount)
{
	FILE	*file = (FILE *) inClientData;

	fseek(file, inPosition + dataOffset, SEEK_SET);

	*actualCount = fwrite(buffer, 1, requestCount, file);

	return 0;
}

CA::SInt64 AudioFileGetSizeProc(void *inClientData)
{
	FILE	*file = (FILE *) inClientData;

	unsigned int	 pos  = ftell(file);

	fseek(file,   0, SEEK_END);

	unsigned int	 size = ftell(file);

	fseek(file, pos, SEEK_SET);

	return size - dataOffset;
}

CA::OSStatus AudioFileSetSizeProc(void *inClientData, CA::SInt64 inSize)
{
	FILE	*file = (FILE *) inClientData;

	fflush(file);
	ftruncate(fileno(file), inSize + dataOffset);

	return 0;
}

CA::OSStatus AudioConverterComplexInputDataProc(CA::AudioConverterRef inAudioConverter, CA::UInt32 *ioNumberDataPackets, CA::AudioBufferList *ioData, CA::AudioStreamPacketDescription **outDataPacketDescription, void *inUserData)
{
	static unsigned char	*suppliedData	  = NULL;
	static unsigned int	 suppliedDataSize = 0;

	suppliedDataSize = min(bufferSize - bytesConsumed, (unsigned int) *ioNumberDataPackets * setup.channels * (setup.bits / 8));
	suppliedData	 = (unsigned char *) realloc(suppliedData, suppliedDataSize);

	memcpy(suppliedData, buffer + bytesConsumed, suppliedDataSize);

	*ioNumberDataPackets = suppliedDataSize / setup.channels / (setup.bits / 8);

	ioData->mBuffers[0].mData           = suppliedData;
	ioData->mBuffers[0].mDataByteSize   = suppliedDataSize;
	ioData->mBuffers[0].mNumberChannels = setup.channels;

	totalSamples  += *ioNumberDataPackets;
	bytesConsumed += ioData->mBuffers[0].mDataByteSize;

	return 0;
}

int main(int argc, char *argv[])
{
	/* Open shared memory object and map view to communication buffer.
	 */
#ifndef __WINE__
	HANDLE	 mapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, false, argv[1]);

	if (mapping == NULL) return 1;

	CoreAudioCommBuffer	*comm = (CoreAudioCommBuffer *) MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
#else
	int	 mapping = shm_open(argv[1], O_RDWR, 0666);

	if (mapping == -1) return 1;

	CoreAudioCommBuffer	*comm = (CoreAudioCommBuffer *) mmap(NULL, sizeof(CoreAudioCommBuffer), PROT_READ | PROT_WRITE, MAP_SHARED, mapping, 0);
#endif

	/* Load Core Audio DLLs.
	 */
	LoadCoreAudioDLL();

	/* Main processing loop.
	 */
	FILE			*file		= NULL;

	CA::AudioFileID		 audioFile	= NULL;
	CA::AudioConverterRef	 converter	= NULL;
	CA::AudioBufferList	*buffers	= NULL;

	int			 frameSize	= 0;
	int64_t			 duration	= 0;

	CA::UInt32		 dataSize	= 0;
	CA::UInt32		 packetsWritten	= 0;

	while (comm->command != CommCommandQuit)
	{
		while (comm->status != CommStatusIssued) Sleep(1);

		switch (comm->command)
		{
			case CommCommandHello:
				if (((CoreAudioCommHello *) &comm->data)->version == 1 && coreaudiodll != NULL) comm->status = CommStatusReady;
				else										comm->status = CommStatusError;

				break;
			case CommCommandCodecs:
				QueryCoreAudioCodecs((CoreAudioCommCodecs *) &comm->data);

				comm->length = sizeof(CoreAudioCommCodecs);
				comm->status = CommStatusReady;

				break;
			case CommCommandSetup:
				memcpy(&setup, comm->data, sizeof(CoreAudioCommSetup));

				/* Setup encoder for first packet.
				 */
				{
					/* Fill out source format description.
					 */
					CA::AudioStreamBasicDescription	 sourceFormat = { 0 };

					sourceFormat.mFormatID		    = CA::kAudioFormatLinearPCM;
					sourceFormat.mFormatFlags	    = CA::kLinearPCMFormatFlagIsPacked;
					sourceFormat.mFormatFlags	   |= setup.fp		      ? CA::kLinearPCMFormatFlagIsFloat		: 0;
					sourceFormat.mFormatFlags	   |= setup.sign && !setup.fp ? CA::kLinearPCMFormatFlagIsSignedInteger : 0;
					sourceFormat.mSampleRate	    = setup.rate;
					sourceFormat.mChannelsPerFrame	    = setup.channels;
					sourceFormat.mBitsPerChannel	    = setup.bits;
					sourceFormat.mFramesPerPacket	    = 1;
					sourceFormat.mBytesPerFrame	    = sourceFormat.mChannelsPerFrame * sourceFormat.mBitsPerChannel / 8;
					sourceFormat.mBytesPerPacket	    = sourceFormat.mFramesPerPacket * sourceFormat.mBytesPerFrame;

					/* Fill out destination format description.
					 */
					CA::AudioStreamBasicDescription	 destinationFormat = { 0 };

					destinationFormat.mFormatID	    = setup.codec;
					destinationFormat.mSampleRate	    = GetOutputSampleRate(setup);
					destinationFormat.mChannelsPerFrame = setup.channels;

					CA::UInt32	 formatSize = sizeof(destinationFormat);

					CA::AudioFormatGetProperty(CA::kAudioFormatProperty_FormatInfo, 0, NULL, &formatSize, &destinationFormat);

					/* Create audio converter object.
					 */
					if (CA::AudioConverterNew(&sourceFormat, &destinationFormat, &converter) != 0) { comm->status = CommStatusError; break; }

					frameSize = destinationFormat.mFramesPerPacket;

					/* Set bitrate if format does support bitrates.
					 */
					CA::UInt32	 size = 0;

					if (CA::AudioConverterGetPropertyInfo(converter, CA::kAudioConverterApplicableEncodeBitRates, &size, NULL) == 0)
					{
						/* Get applicable bitrate values.
						 */
						CA::UInt32		 bitrate       = setup.bitrate;
						CA::AudioValueRange	*bitrateValues = (CA::AudioValueRange *) malloc(size);

						CA::AudioConverterGetProperty(converter, CA::kAudioConverterApplicableEncodeBitRates, &size, bitrateValues);

						/* Find best supported bitrate.
						 */
						CA::Float64	 nearest = 0xFFFFFFFF;

						for (unsigned int i = 0; i < size / sizeof(CA::AudioValueRange); i++)
						{
							if (bitrate >= bitrateValues[i].mMinimum && bitrate <= bitrateValues[i].mMaximum)  nearest = bitrate;

							if (fabs(bitrate - bitrateValues[i].mMinimum) < fabs(bitrate - nearest)) nearest = bitrateValues[i].mMinimum;
							if (fabs(bitrate - bitrateValues[i].mMaximum) < fabs(bitrate - nearest)) nearest = bitrateValues[i].mMaximum;
						}

						bitrate = nearest;

						free(bitrateValues);

						/* Set bitrate on converter.
						 */
						CA::AudioConverterSetProperty(converter, CA::kAudioConverterEncodeBitRate, sizeof(CA::UInt32), &bitrate);
					}

					/* Create audio file object for output file.
					 */
					CA::UInt32	 fileType = setup.format ? CA::kAudioFileM4AType : CA::kAudioFileAAC_ADTSType;

#ifdef __WINE__
					file	   = fopen(setup.file, "r+b");
#else
					wchar_t	 fileName[32768];

					MultiByteToWideChar(CP_UTF8, 0, setup.file, -1, fileName, sizeof(fileName) / sizeof(wchar_t));

					file	   = _wfopen(fileName, L"r+b");
#endif

					fseek(file, 0, SEEK_END);
					dataOffset = ftell(file);

					CA::AudioFileInitializeWithCallbacks(file, AudioFileReadProc, AudioFileWriteProc, AudioFileGetSizeProc, AudioFileSetSizeProc, fileType, &destinationFormat, 0, &audioFile);

					/* Get magic cookie and supply it to audio file.
					 */
					CA::UInt32	 cookieSize = 0;

					if (CA::AudioConverterGetPropertyInfo(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, NULL) == 0)
					{
						unsigned char	*cookie = (unsigned char *) malloc(cookieSize);

						CA::AudioConverterGetProperty(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, cookie);
						CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyMagicCookieData, cookieSize, cookie);

						free(cookie);
					}

					/* Get maximum output packet size.
					 */
					CA::UInt32	 valueSize = 4;

					CA::AudioConverterGetProperty(converter, CA::kAudioConverterPropertyMaximumOutputPacketSize, &valueSize, &dataSize);

					/* Set up buffer for Core Audio.
					 */
					buffers = (CA::AudioBufferList	*) malloc(sizeof(CA::AudioBufferList) + sizeof(CA::AudioBuffer));

					buffers->mNumberBuffers = 1;

					buffers->mBuffers[0].mData	     = (unsigned char *) malloc(dataSize);
					buffers->mBuffers[0].mDataByteSize   = dataSize;
					buffers->mBuffers[0].mNumberChannels = setup.channels;
				}

				packetsWritten = 0;
				totalSamples   = 0;

				comm->status = CommStatusReady;

				break;
			case CommCommandEncode:
				if (converter == NULL) { comm->status = CommStatusError; break; }

				/* Encode supplied packet.
				 */
				{
					/* Configure buffer.
					 */
					bufferSize += comm->length;
					buffer	    = (unsigned char *) realloc(buffer, bufferSize);

					memmove(buffer, buffer + bytesConsumed, bufferSize - bytesConsumed - comm->length);
					memcpy(buffer + bufferSize - bytesConsumed - comm->length, comm->data, comm->length);

					bufferSize -= bytesConsumed;
					buffer	    = (unsigned char *) realloc(buffer, bufferSize);

					bytesConsumed = 0;

					/* Convert frames.
					 */
					CA::UInt32				 packets = 1;
					CA::AudioStreamPacketDescription	 packet;

					buffers->mBuffers[0].mDataByteSize = dataSize;

					while (CA::AudioConverterFillComplexBuffer(converter, &AudioConverterComplexInputDataProc, NULL, &packets, buffers, &packet) == 0)
					{
						if (buffers->mBuffers[0].mDataByteSize == 0) break;

						CA::AudioFileWritePackets(audioFile, false, buffers->mBuffers[0].mDataByteSize, &packet, packetsWritten, &packets, buffers->mBuffers[0].mData);

						packetsWritten += packets;

						if (bufferSize - bytesConsumed < 65536) break;

						buffers->mBuffers[0].mDataByteSize = dataSize;
					}
				}

				comm->status = CommStatusReady;

				break;
			case CommCommandFinish:
				if (converter == NULL) { comm->status = CommStatusError; break; }

				/* Finish encoding after last packet.
				 */
				{
					/* Convert final frames.
					 */
					CA::UInt32				 packets = 1;
					CA::AudioStreamPacketDescription	 packet;

					buffers->mBuffers[0].mDataByteSize = dataSize;

					while (CA::AudioConverterFillComplexBuffer(converter, &AudioConverterComplexInputDataProc, NULL, &packets, buffers, &packet) == 0)
					{
						if (buffers->mBuffers[0].mDataByteSize == 0) break;

						CA::AudioFileWritePackets(audioFile, false, buffers->mBuffers[0].mDataByteSize, &packet, packetsWritten, &packets, buffers->mBuffers[0].mData);

						packetsWritten += packets;

						buffers->mBuffers[0].mDataByteSize = dataSize;
					}

					free(buffers->mBuffers[0].mData);
					free(buffers);

					/* Calculate frame size divider and duration.
					 */
					int	 rate	 = GetOutputSampleRate(setup);

					if (rate == 0) rate = setup.rate;

					float	 divider = float(setup.rate) / rate;

					if (setup.codec == CA::kAudioFormatMPEG4AAC_HE ||
					    setup.codec == CA::kAudioFormatMPEG4AAC_HE_V2) divider *= 2.0;

					duration = int64_t(packetsWritten) * frameSize / divider;

					/* Write priming and remainder info.
					 */
					CA::AudioConverterPrimeInfo	 primeInfo;
					CA::UInt32			 size = sizeof(primeInfo);

					if (CA::AudioConverterGetProperty(converter, CA::kAudioConverterPrimeInfo, &size, &primeInfo) == 0)
					{
						CA::AudioFilePacketTableInfo	 pti;

						pti.mPrimingFrames     = primeInfo.leadingFrames;
						pti.mRemainderFrames   = primeInfo.trailingFrames;
						pti.mNumberValidFrames = ceil(totalSamples / divider);

						CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyPacketTableInfo, sizeof(pti), &pti);
					}

					/* Get and set magic cookie again as some
					 * encoders may change it during encoding.
					 */
					CA::UInt32	 cookieSize = 4;

					if (CA::AudioConverterGetPropertyInfo(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, NULL) == 0)
					{
						unsigned char	*cookie = (unsigned char *) malloc(cookieSize);

						CA::AudioConverterGetProperty(converter, CA::kAudioConverterCompressionMagicCookie, &cookieSize, cookie);
						CA::AudioFileSetProperty(audioFile, CA::kAudioFilePropertyMagicCookieData, cookieSize, cookie);

						free(cookie);
					}

					/* Close converter and audio file.
					 */
					CA::AudioConverterDispose(converter);
					CA::AudioFileClose(audioFile);

					fclose(file);
				}

				comm->status = CommStatusReady;

				break;
			case CommCommandDuration:
				comm->length = sizeof(int64_t);

				comm->data[0] = duration >> 32;
				comm->data[1] = duration & 0xFFFFFFFF;

				comm->status = CommStatusReady;

				break;
			case CommCommandQuit:
				comm->status = CommStatusReady;

				break;
		}
	}

	/* Free Core Audio DLLs.
	 */
	FreeCoreAudioDLL();

	/* Unmap view and close shared memory object.
	 */
#ifndef __WINE__
	UnmapViewOfFile(comm);
	CloseHandle(mapping);
#else
	munmap(comm, sizeof(CoreAudioCommBuffer));
	close(mapping);
#endif

	return 0;
}
