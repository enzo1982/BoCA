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

#include <smooth/io/drivers/driver_ansi.h>

#include "fdkaac.h"

#ifndef CAPF_AAC_USAC
#	define CAPF_AAC_USAC 0x00200000
#endif

using namespace smooth::IO;

const String &BoCA::DecoderFDKAAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (fdkaacdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>FDK-AAC Decoder %VERSION%</name>				\
		    <version>1.0</version>						\
		    <id>fdkaac-dec</id>							\
		    <type>decoder</type>						\
		    <precede>faad2-dec</precede>					\
											\
		";

		if (AS::Registry::Get().ComponentExists("mp4-mux"))
		{
			componentSpecs.Append("						\
											\
			    <format>							\
			      <name>MPEG-4 AAC Files</name>				\
			      <extension>m4a</extension>				\
			      <extension>m4b</extension>				\
			      <extension>m4r</extension>				\
			      <extension>mp4</extension>				\
			      <extension>3gp</extension>				\
			      <extension>3gpp</extension>				\
			      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
			    </format>							\
											\
			");
		}

		componentSpecs.Append("							\
											\
		    <format>								\
		      <name>Raw AAC Files</name>					\
		      <extension>aac</extension>					\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>		\
		    </format>								\
		  </component>								\
											\
		");

		UnsignedInt32	 version = GetDecoderVersion();

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

Bool BoCA::DecoderFDKAAC::CanOpenStream(const String &streamURI)
{
	Bool		 isValidFile = False;
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if ((in.InputNumberRaw(8) & 0xFFFFFFFF) == 'ftyp')
	{
		/* Open MP4 file and check for a supported AAC track.
		 */
		DriverANSI		 ioDriver(File(streamURI), IS_READ);
		AS::MuxerComponent	*muxer = OpenMuxer(&ioDriver);

		if (muxer != NIL)
		{
			if (muxer->GetCodec() == MUXER_CODEC_AAC)
			{
				LIB_INFO	 info[FDK_MODULE_LAST];

				FDKinitLibInfo(info);
				ex_aacDecoder_GetLibInfo(info);

				UINT	 aacFlags = FDKlibInfo_getCapabilities(info, FDK_AACDEC);
				UINT	 sbrFlags = FDKlibInfo_getCapabilities(info, FDK_SBRDEC);

				Buffer<UnsignedByte>	 codecConfig;

				muxer->GetCodecConfig(codecConfig);

				Int	 type	  = GetAudioObjectType(codecConfig);

				if ((type == AOT_AAC_LC	     && aacFlags & CAPF_AAC_LC	   ) ||
				    (type == AOT_SBR	     && sbrFlags		   ) ||
				    (type == AOT_PS	     && sbrFlags & CAPF_SBR_PS_MPEG) ||
				    (type == AOT_USAC	     && aacFlags & CAPF_AAC_USAC   ) ||

				    (type == AOT_ER_AAC_LC   && aacFlags & CAPF_ER_AAC_LC  ) ||
				    (type == AOT_ER_AAC_LD   && aacFlags & CAPF_ER_AAC_LD  ) ||
				    (type == AOT_ER_AAC_ELD  && aacFlags & CAPF_ER_AAC_ELD ) ||
				    (type == AOT_ER_AAC_SCAL && aacFlags & CAPF_ER_AAC_SCAL) ||
				    (type == AOT_ER_BSAC     && aacFlags & CAPF_ER_AAC_BSAC)) isValidFile = True;
			}

			CloseMuxer(muxer);
		}
	}
	else
	{
		Track	 track;

		if (GetStreamInfo(streamURI, track) == Success()) isValidFile = True;
	}

	return isValidFile;
}

Error BoCA::DecoderFDKAAC::GetStreamInfo(const String &streamURI, Track &track)
{
	Format		 format = track.GetFormat();
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if ((in.InputNumberRaw(8) & 0xFFFFFFFF) == 'ftyp')
	{
		track.fileSize	= File(streamURI).GetFileSize();
		track.length	= -1;

		DriverANSI		 ioDriver(File(streamURI), IS_READ);
		AS::MuxerComponent	*muxer = OpenMuxer(&ioDriver);

		if (muxer != NIL)
		{
			HANDLE_AACDECODER	 handle = ex_aacDecoder_Open(TT_MP4_RAW, 1);

			/* Get codec configuration.
			 */
			Buffer<UnsignedByte>	 codecConfig;

			muxer->GetCodecConfig(codecConfig);

			/* The FDK bitstream reader reads up to 4 bytes behind the buffer,
			 * so allocate a larger buffer to prevent access violations.
			 */
			unsigned int	 escBufferSize = codecConfig.Size();
			unsigned char	*escBuffer     = new unsigned char [escBufferSize + 4];

			memcpy(escBuffer, codecConfig, escBufferSize);

			ex_aacDecoder_ConfigRaw(handle, &escBuffer, &escBufferSize);

			delete [] escBuffer;

			/* Decode one frame to initialize decoder.
			 */
			Buffer<UnsignedByte>	 packet;

			Int64		 position   = 0;
			Int		 flags	    = 0;
			unsigned int	 bufferSize = Math::Max(0, muxer->ReadPacket(packet, position, flags));

			packet.Resize(bufferSize + 4);

			unsigned char	*buffer	    = packet;
			unsigned int	 bytesValid = bufferSize;

			ex_aacDecoder_Fill(handle, &buffer, &bufferSize, &bytesValid);

			short		*outputBuffer = new short [16384];

			if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, 16384, 0) != AAC_DEC_OK)
			{
				errorState  = True;
				errorString = "Unsupported audio format";
			}

			delete [] outputBuffer;

			/* Get sample rate and number of channels.
			 */
			if (!errorState)
			{
				CStreamInfo	*streamInfo = ex_aacDecoder_GetStreamInfo(handle);

				frameSize	= streamInfo->frameSize;
				sbrRatio	= streamInfo->frameSize / streamInfo->aacSamplesPerFrame;

				format.rate	= streamInfo->sampleRate;
				format.channels = streamInfo->numChannels;

				if (format.channels == 2 && muxer->GetFormatInfo().channels == 1) format.channels = 1;

				track.length	= muxer->GetNumberOfPackets() * streamInfo->frameSize;
				track.length   -= streamInfo->frameSize; // To account for encoder delay.

				format.bits	= 16;
			}

			ex_aacDecoder_Close(handle);

			track.SetFormat(format);

			/* Read gapless information.
			 */
			Int	 delay	= 0, padding = 0;
			Int64	 length	= 0;

			if (muxer->GetGaplessInfo(delay, padding, length) && (delay + padding + length) * sbrRatio == muxer->GetNumberOfPackets() * frameSize)
			{
				track.length = length * sbrRatio;
			}

			CloseMuxer(muxer);
		}

		/* Read MP4 metadata.
		 */
		if (!errorState)
		{
			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("mp4-tag");

			if (tagger != NIL)
			{
				tagger->SetConfiguration(GetConfiguration());
				tagger->ParseStreamInfo(streamURI, track);

				boca.DeleteComponent(tagger);
			}
		}
	}
	else
	{
		in.Seek(0);

		format.bits	= 16;

		track.fileSize	= in.Size();
		track.length	= -1;

		SkipID3v2Tag(in);

		if (!SyncOnAACHeader(in))
		{
			errorState  = True;
			errorString = "Invalid file format";

			return Error();
		}

		HANDLE_AACDECODER	 handle = ex_aacDecoder_Open( adifFound ? TT_MP4_ADIF :
								     (adtsFound ? TT_MP4_ADTS : TT_MP4_LOAS), 1);

		/* Decode one frame to initialize decoder.
		 */
		unsigned int	 size = Math::Min((Int64) 32768, track.fileSize - in.GetPos());
		unsigned char	*data = new unsigned char [size];

		in.InputData((void *) data, size);

		unsigned int	 bytesValid = size;

		ex_aacDecoder_Fill(handle, &data, &size, &bytesValid);

		short	*outputBuffer = new short [16384];

		if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, 16384, 0) == AAC_DEC_OK)
		{
			/* Get sample rate and number of channels.
			 */
			CStreamInfo	*streamInfo = ex_aacDecoder_GetStreamInfo(handle);

			format.rate	= streamInfo->sampleRate;
			format.channels = streamInfo->numChannels;

			/* Compute approximate length of stream.
			 */
			Int	 samplesRead = streamInfo->frameSize;

			while (True)
			{
				if (bytesValid > 0)
				{
					unsigned char	*inputBuffer	 = data + size - bytesValid;
					unsigned int	 inputBufferSize = bytesValid;

					ex_aacDecoder_Fill(handle, &inputBuffer, &inputBufferSize, &bytesValid);
				}

				if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, 16384, 0) != AAC_DEC_OK) break;

				samplesRead += streamInfo->frameSize;
			}

			if (samplesRead > 0) track.approxLength = samplesRead * (track.fileSize / size);
		}
		else
		{
			errorState  = True;
			errorString = "Unsupported audio format";
		}

		/* Close handles.
		 */
		delete [] outputBuffer;
		delete [] data;

		ex_aacDecoder_Close(handle);

		/* Read ID3v2 tag if any.
		 */
		if (!errorState)
		{
			Bool			 foundTag = False;

			AS::Registry		&boca = AS::Registry::Get();
			AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

			if (tagger != NIL)
			{
				tagger->SetConfiguration(GetConfiguration());

				if (tagger->ParseStreamInfo(streamURI, track) == Success()) foundTag = True;

				boca.DeleteComponent(tagger);
			}

			if (!foundTag)
			{
				tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

				if (tagger != NIL)
				{
					tagger->SetConfiguration(GetConfiguration());
					tagger->ParseStreamInfo(streamURI, track);

					boca.DeleteComponent(tagger);
				}
			}
		}
	}

	track.SetFormat(format);

	if (!errorState) return Success();
	else		 return Error();
}

BoCA::DecoderFDKAAC::DecoderFDKAAC()
{
	muxer		 = NIL;
	handle		 = NIL;

	finished	 = False;

	adifFound	 = False;
	adtsFound	 = False;
	loasFound	 = False;

	frameSize	 = 0;
	sbrRatio	 = 1;

	delaySamples	 = 0;
	delaySamplesLeft = 0;
}

BoCA::DecoderFDKAAC::~DecoderFDKAAC()
{
}

UnsignedInt32 BoCA::DecoderFDKAAC::GetDecoderVersion()
{
	LIB_INFO	 info[FDK_MODULE_LAST];

	FDKinitLibInfo(info);
	ex_aacDecoder_GetLibInfo(info);

	for (Int i = 0; i < FDK_MODULE_LAST; i++)
	{
		if (info[i].module_id != FDK_AACDEC) continue;

		return info[i].version;
	}

	return 0;
}

Bool BoCA::DecoderFDKAAC::Activate()
{
	InStream	 in(STREAM_DRIVER, driver);

	if ((in.InputNumberRaw(8) & 0xFFFFFFFF) == 'ftyp')
	{
		muxer = OpenMuxer(driver);

		if (muxer == NIL) return False;

		handle = ex_aacDecoder_Open(TT_MP4_RAW, 1);

		/* Get codec configuration.
		 */
		Buffer<UnsignedByte>	 codecConfig;

		muxer->GetCodecConfig(codecConfig);

		/* The FDK bitstream reader reads up to 4 bytes behind the buffer,
		 * so allocate a larger buffer to prevent access violations.
		 */
		unsigned int	 escBufferSize = codecConfig.Size();
		unsigned char	*escBuffer     = new unsigned char [escBufferSize + 4];

		memcpy(escBuffer, codecConfig, escBufferSize);

		ex_aacDecoder_ConfigRaw(handle, &escBuffer, &escBufferSize);

		delete [] escBuffer;
	}
	else
	{
		in.Seek(0);

		SkipID3v2Tag(in);
		SyncOnAACHeader(in);

		driver->Seek(in.GetPos());

		handle = ex_aacDecoder_Open( adifFound ? TT_MP4_ADIF :
					    (adtsFound ? TT_MP4_ADTS : TT_MP4_LOAS), 1);
	}

	return True;
}

Bool BoCA::DecoderFDKAAC::Deactivate()
{
	/* Close decoder.
	 */
	ex_aacDecoder_Close(handle);

	/* Close MP4 file.
	 */
	CloseMuxer(muxer);

	muxer = NIL;

	return True;
}

Bool BoCA::DecoderFDKAAC::Seek(Int64 samplePosition)
{
	if (muxer == NIL) return False;

	Int64	 time	  = Math::Round(Float(samplePosition) / track.GetFormat().rate * muxer->GetTimeScale());
	Int64	 position = muxer->SeekPacket(time);

	if (position < 0) return False;

	delaySamplesLeft = delaySamples + time - position;

	return True;
}

Int BoCA::DecoderFDKAAC::ReadData(Buffer<UnsignedByte> &data)
{
	static Int	 maxFrameSize = 2048;

	if (finished) return -1;

	const Format	&format = track.GetFormat();

	Int	 samplesRead = 0;

	samplesBuffer.Resize(0);

	if (muxer != NIL)
	{
		Int64	 numberOfPackets = muxer->GetNumberOfPackets();

		Int64	 position   = 0;
		Int	 flags	    = 0;
		Int	 packetSize = muxer->ReadPacket(dataBuffer, position, flags);

		if (packetSize >= 0)
		{
			unsigned int	 bufferSize = packetSize;

			dataBuffer.Resize(bufferSize + 4); // + 4 to account for FDK bitstream implementation overreading.

			unsigned char	*buffer	    = dataBuffer;
			unsigned int	 bytesValid = bufferSize;

			if (bytesValid > 0)
			{
				unsigned char	*inputBuffer	 = buffer + bufferSize - bytesValid;
				unsigned int	 inputBufferSize = bytesValid;

				ex_aacDecoder_Fill(handle, &inputBuffer, &inputBufferSize, &bytesValid);
			}

			Int	 bufferChannels = Math::Max(2, format.channels);

			if (frameSize == 0) samplesBuffer.Resize(samplesRead * format.channels + maxFrameSize * bufferChannels);
			else		    samplesBuffer.Resize(samplesRead * format.channels + frameSize * bufferChannels);

			short	*outputBuffer = samplesBuffer + samplesRead * format.channels;

			if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, samplesBuffer.Size() - samplesRead * format.channels, 0) == AAC_DEC_OK)
			{
				CStreamInfo	*streamInfo = ex_aacDecoder_GetStreamInfo(handle);

				if (frameSize == 0)
				{
					frameSize = streamInfo->frameSize;
					sbrRatio  = streamInfo->frameSize / streamInfo->aacSamplesPerFrame;

					/* Get delay from gapless information.
					 */
					Int	 delay	= 0, padding = 0;
					Int64	 length	= 0;

					if (muxer->GetGaplessInfo(delay, padding, length) && (delay + padding + length) * sbrRatio == numberOfPackets * frameSize)
					{
						delaySamples	 = delay * sbrRatio;
						delaySamplesLeft = delaySamples;
					}

					/* Set delay samples to minimum encoder delay.
					 */
					if (delaySamples == 0)
					{
						delaySamples	 = frameSize;
						delaySamplesLeft = delaySamples;
					}

					/* Add FDK decoder delay.
					 */
					delaySamplesLeft += streamInfo->outputDelay;
				}

				if (format.channels == 1 && streamInfo->numChannels == 2)
				{
					for (Int i = 0; i < frameSize; i++) samplesBuffer[samplesRead + i] = samplesBuffer[samplesRead + i * 2];
				}

				samplesBuffer.Resize((samplesRead + frameSize) * format.channels);

				samplesRead += frameSize;
			}
		}
		else
		{
			/* Flush the decoder reading up to two more frames (some decoders
			 * keep returning AAC_DEC_OK even after the end is reached, so we
			 * have to limit the number of read attempts).
			 */
			for (Int i = 0; i < 2; i++)
			{
				Int	 bufferChannels = Math::Max(2, format.channels);

				samplesBuffer.Resize(samplesRead * format.channels + frameSize * bufferChannels);

				short	*outputBuffer = samplesBuffer + samplesRead * format.channels;

				if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, samplesBuffer.Size() - samplesRead * format.channels, AACDEC_FLUSH) != AAC_DEC_OK) break;

				CStreamInfo	*streamInfo = ex_aacDecoder_GetStreamInfo(handle);

				if (format.channels == 1 && streamInfo->numChannels == 2)
				{
					for (Int i = 0; i < frameSize; i++) samplesBuffer[samplesRead + i] = samplesBuffer[samplesRead + i * 2];
				}

				samplesBuffer.Resize((samplesRead + frameSize) * format.channels);
	
				samplesRead += frameSize;
			}

			finished = True;
		}
	}
	else
	{
		dataBuffer.Resize(data.Size() + 4); // + 4 to account for FDK bitstream implementation overreading.

		Int	 size = driver->ReadData(dataBuffer, data.Size());

		if (size <= 0) return -1;

		inBytes += size;

		UnsignedInt	 bytesValid = size;

		while (bytesValid || driver->GetPos() == driver->GetSize())
		{
			if (bytesValid > 0)
			{
				unsigned char	*inputBuffer	 = dataBuffer + size - bytesValid;
				unsigned int	 inputBufferSize = bytesValid;

				ex_aacDecoder_Fill(handle, &inputBuffer, &inputBufferSize, &bytesValid);
			}

			if (frameSize == 0) samplesBuffer.Resize((samplesRead + maxFrameSize) * format.channels);
			else		    samplesBuffer.Resize((samplesRead + frameSize) * format.channels);

			short	*outputBuffer = samplesBuffer + samplesRead * format.channels;

			if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, samplesBuffer.Size() - samplesRead * format.channels, 0) != AAC_DEC_OK) break;

			if (frameSize == 0)
			{
				CStreamInfo	*streamInfo = ex_aacDecoder_GetStreamInfo(handle);

				frameSize = streamInfo->frameSize;

				/* Set delay samples to minimum encoder delay.
				 */
				delaySamples	 = frameSize;
				delaySamplesLeft = delaySamples;

				/* Add FDK decoder delay.
				 */
				delaySamplesLeft += streamInfo->outputDelay;

				samplesBuffer.Resize((samplesRead + frameSize) * format.channels);
			}

			samplesRead += frameSize;
		}

		if (driver->GetPos() == driver->GetSize())
		{
			/* Flush the decoder reading up to two more frames (some decoders
			 * keep returning AAC_DEC_OK even after the end is reached, so we
			 * have to limit the number of read attempts).
			 */
			for (Int i = 0; i < 2; i++)
			{
				samplesBuffer.Resize((samplesRead + frameSize) * format.channels);

				short	*outputBuffer = samplesBuffer + samplesRead * format.channels;

				if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, samplesBuffer.Size() - samplesRead * format.channels, AACDEC_FLUSH) != AAC_DEC_OK) break;

				samplesRead += frameSize;
			}

			finished = True;
		}
	}

	data.Resize(0);

	if (samplesRead > delaySamplesLeft)
	{
		data.Resize((samplesRead - delaySamplesLeft) * format.channels * (format.bits / 8));

		memcpy(data, samplesBuffer + delaySamplesLeft * format.channels, data.Size());
	}

	delaySamplesLeft = Math::Max(0, delaySamplesLeft - samplesRead);

	return data.Size();
}

BoCA::AS::MuxerComponent *BoCA::DecoderFDKAAC::OpenMuxer(Driver *ioDriver) const
{
	/* Create MP4 muxer and open stream.
	 */
	AS::Registry		&boca  = AS::Registry::Get();
	AS::MuxerComponent	*muxer = (AS::MuxerComponent *) boca.CreateComponentByID("mp4-mux");

	if (muxer == NIL) return NIL;

	muxer->SetConfiguration(GetConfiguration());
	muxer->SetDriver(ioDriver);

	if (!muxer->Open(MUXER_MODE_READ))
	{
		boca.DeleteComponent(muxer);

		return NIL;
	}

	return muxer;
}

Void BoCA::DecoderFDKAAC::CloseMuxer(AS::MuxerComponent *muxer) const
{
	if (muxer == NIL) return;

	muxer->Close();

	AS::Registry::Get().DeleteComponent(muxer);
}

Int BoCA::DecoderFDKAAC::GetAudioObjectType(const Buffer<UnsignedByte> &codecConfig) const
{
	/* Parse audio object type from MPEG-4 audio specific config.
	 */
	if (codecConfig.Size() < 1) return 0;

	Int	 type = (codecConfig[0] >> 3) & 0x1F;

	if (type == 0x1F)
	{
		if (codecConfig.Size() < 2) return 0;

		type = 32 + (((codecConfig[0] & 0x07) << 3) | ((codecConfig[1] >> 5) & 0x07));
	}

	return type;
}

Bool BoCA::DecoderFDKAAC::SkipID3v2Tag(InStream &in)
{
	/* Check for an ID3v2 tag at the beginning
	 * of the file and skip it if it exists.
	 */
	if (in.InputString(3) == "ID3")
	{
		in.InputNumber(2); // ID3 version
		in.InputNumber(1); // Flags

		/* Read tag size as a 4 byte unsynchronized integer.
		 */
		Int	 tagSize = (in.InputNumber(1) << 21) +
				   (in.InputNumber(1) << 14) +
				   (in.InputNumber(1) <<  7) +
				   (in.InputNumber(1)      );

		in.RelSeek(tagSize);

		inBytes += (tagSize + 10);
	}
	else
	{
		in.Seek(0);
	}

	return True;
}

Bool BoCA::DecoderFDKAAC::SyncOnAACHeader(InStream &in)
{
	const Int	 startPos     = in.GetPos();
	const Int	 maxFrameSize = 8192;

	/* Try to sync on ADIF header.
	 */
	for (Int n = 0; n < maxFrameSize; n++)
	{
		if (in.InputNumber(1) != 'A') continue;
		if (in.InputNumber(1) != 'D') continue;
		if (in.InputNumber(1) != 'I') continue;
		if (in.InputNumber(1) != 'F') continue;

		/* ADIF magic word found.
		 */
		in.RelSeek(-4);

		inBytes += n;

		adifFound = True;

		return True;
	}

	in.Seek(startPos);

	/* Try to sync on ADTS header.
	 */
	for (Int n = 0; n < maxFrameSize; n++)
	{
		if (  in.InputNumber(1)		      != 0xFF) continue;
		if ( (in.InputNumber(1) & 0xF6)       != 0xF0) continue;
		if (((in.InputNumber(1) & 0x3C) >> 2) >=   12) continue;

		/* ADTS sync found.
		 */
		in.RelSeek(-3);

		inBytes += n;

		adtsFound = True;

		return True;
	}

	in.Seek(startPos);

	/* Try to sync on LOAS/LATM header.
	 */
	for (Int n = 0; n < maxFrameSize; n++)
	{
		if (  in.InputNumber(1)		!= 0x56) continue;
		if ( (in.InputNumber(1) & 0xE0) != 0xE0) continue;

		/* LOAS sync.
		 */
		in.RelSeek(-2);

		inBytes += n;

		loasFound = True;

		return True;
	}

	/* No sync. Probably not an AAC file.
	 */
	return False;
}
