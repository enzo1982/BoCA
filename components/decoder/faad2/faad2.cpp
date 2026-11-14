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

#include "faad2.h"

#include <stdint.h>

#ifndef PS
#	define PS 29
#endif

using namespace smooth::IO;

const String &BoCA::DecoderFAAD2::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (faad2dll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>FAAD2 MP4/AAC Decoder %VERSION%</name>			\
		    <version>1.0</version>						\
		    <id>faad2-dec</id>							\
		    <type>decoder</type>						\
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

		char	*faad2Version = NIL;

		ex_NeAACDecGetVersion(&faad2Version, NIL);

		componentSpecs.Replace("%VERSION%", String("v").Append(faad2Version));
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadFAAD2DLL();
}

Void smooth::DetachDLL()
{
	FreeFAAD2DLL();
}

Bool BoCA::DecoderFAAD2::CanOpenStream(const String &streamURI)
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
				Buffer<UnsignedByte>	 codecConfig;

				muxer->GetCodecConfig(codecConfig);

				Int	 type = GetAudioObjectType(codecConfig);

				if (type == MAIN   ||
				    type == LC	   ||
				    type == LTP	   ||
				    type == HE_AAC ||
				    type == LD	   ||
				    type == PS	   ||

				    type == ER_LC  ||
				    type == ER_LTP) isValidFile = True;
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

Error BoCA::DecoderFAAD2::GetStreamInfo(const String &streamURI, Track &track)
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
			NeAACDecHandle			 handle	 = ex_NeAACDecOpen();
			NeAACDecConfigurationPtr	 fConfig = ex_NeAACDecGetCurrentConfiguration(handle);

			fConfig->defSampleRate	= 44100;
			fConfig->defObjectType	= LC;
			fConfig->outputFormat	= FAAD_FMT_16BIT;

			ex_NeAACDecSetConfiguration(handle, fConfig);

			/* Get codec configuration.
			 */
			Buffer<UnsignedByte>	 codecConfig;

			muxer->GetCodecConfig(codecConfig);

			if (ex_NeAACDecInit2(handle, codecConfig, codecConfig.Size(), (unsigned long *) &format.rate, (unsigned char *) &format.channels) < 0)
			{
				errorState  = True;
				errorString = "Unsupported audio format";
			}

			if (format.channels == 2 && muxer->GetFormatInfo().channels == 1) format.channels = 1;

			/* Decode frames to get frame size.
			 */
			Buffer<UnsignedByte>	 buffer;

			while (frameSize == 0 && !errorState)
			{
				Int64	 position   = 0;
				Int	 flags	    = 0;
				Int	 bufferSize = muxer->ReadPacket(buffer, position, flags);

				if (bufferSize < 0)
				{
					errorState  = True;
					errorString = "Unsupported audio format";

					break;
				}

				NeAACDecFrameInfo frameInfo;

				ex_NeAACDecDecode(handle, &frameInfo, buffer, bufferSize);

				if (frameInfo.error)
				{
					errorState  = True;
					errorString = "Unsupported audio format";
				}

				frameSize = frameInfo.samples / frameInfo.channels;

				if (frameInfo.sbr == SBR_UPSAMPLED ||
				    frameInfo.sbr == NO_SBR_UPSAMPLED) sbrRatio = 2;
			}

			/* Get track length.
			 */
			track.length	= muxer->GetNumberOfPackets() * frameSize;
			track.length   -= frameSize; // To account for encoder delay.

			format.bits	= 16;

			ex_NeAACDecClose(handle);

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

		NeAACDecHandle			 handle	 = ex_NeAACDecOpen();
		NeAACDecConfigurationPtr	 fConfig = ex_NeAACDecGetCurrentConfiguration(handle);

		fConfig->defSampleRate	= 44100;
		fConfig->defObjectType	= LC;
		fConfig->outputFormat	= FAAD_FMT_16BIT;

		ex_NeAACDecSetConfiguration(handle, fConfig);

		/* Get sample rate and number of channels.
		 */
		Int		 size = Math::Min((Int64) 32768, track.fileSize - in.GetPos());
		unsigned char	*data = new unsigned char [size];

		in.InputData((void *) data, size);

		if (ex_NeAACDecInit(handle, data, size, (unsigned long *) &format.rate, (unsigned char *) &format.channels) < 0)
		{
			errorState  = True;
			errorString = "Unsupported audio format";
		}

		/* Compute approximate length of stream.
		 */
		if (!errorState)
		{
			NeAACDecFrameInfo	 frameInfo;

			ex_NeAACDecDecode(handle, &frameInfo, data, size);

			if (!frameInfo.error)
			{
				Int	 bytesConsumed = 0;
				Int	 samplesRead   = 0;

				while (!frameInfo.error)
				{
					bytesConsumed += frameInfo.bytesconsumed;
					samplesRead   += frameInfo.samples;

					ex_NeAACDecDecode(handle, &frameInfo, data + bytesConsumed, size - bytesConsumed);
				}

				if (samplesRead > 0) track.approxLength = samplesRead / format.channels * (track.fileSize / bytesConsumed);
			}
			else
			{
				errorState  = True;
				errorString = "Unsupported audio format";
			}
		}

		/* Close handles.
		 */
		delete [] data;

		ex_NeAACDecClose(handle);

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

BoCA::DecoderFAAD2::DecoderFAAD2()
{
	muxer		 = NIL;
	handle		 = NIL;
	fConfig		 = NIL;

	frameSize	 = 0;
	sbrRatio	 = 1;

	delaySamples	 = 0;
	delaySamplesLeft = 0;
}

BoCA::DecoderFAAD2::~DecoderFAAD2()
{
}

Bool BoCA::DecoderFAAD2::Activate()
{
	InStream	 in(STREAM_DRIVER, driver);

	if ((in.InputNumberRaw(8) & 0xFFFFFFFF) == 'ftyp')
	{
		muxer = OpenMuxer(driver);

		if (muxer == NIL) return False;
	}
	else
	{
		in.Seek(0);

		SkipID3v2Tag(in);
		SyncOnAACHeader(in);

		driver->Seek(in.GetPos());
	}

	handle	= ex_NeAACDecOpen();
	fConfig	= ex_NeAACDecGetCurrentConfiguration(handle);

	fConfig->defSampleRate	= 44100;
	fConfig->defObjectType	= LC;
	fConfig->outputFormat	= FAAD_FMT_16BIT;

	ex_NeAACDecSetConfiguration(handle, fConfig);

	if (muxer != NIL)
	{
		/* Get codec configuration.
		 */
		Buffer<UnsignedByte>	 codecConfig;

		muxer->GetCodecConfig(codecConfig);

		unsigned long	 rate;
		unsigned char	 channels;

		if (ex_NeAACDecInit2(handle, codecConfig, codecConfig.Size(), &rate, &channels) < 0)
		{
			errorState  = True;
			errorString = "Unsupported audio format";
		}
	}
	else
	{
		Int		 size = 4096;
		unsigned char	*data = new unsigned char [size];

		size = driver->ReadData(data, size);

		unsigned long	 rate;
		unsigned char	 channels;

		if (ex_NeAACDecInit(handle, data, size, &rate, &channels) < 0)
		{
			errorState  = True;
			errorString = "Unsupported audio format";
		}

		delete [] data;

		driver->Seek(driver->GetPos() - size);
	}

	/* Check for error.
	 */
	if (errorState)
	{
		ex_NeAACDecClose(handle);

		CloseMuxer(muxer);

		muxer = NIL;

		return False;
	}

	return True;
}

Bool BoCA::DecoderFAAD2::Deactivate()
{
	/* Close decoder.
	 */
	ex_NeAACDecClose(handle);

	/* Close MP4 file.
	 */
	CloseMuxer(muxer);

	muxer = NIL;

	return True;
}

Bool BoCA::DecoderFAAD2::Seek(Int64 samplePosition)
{
	if (muxer == NIL) return False;

	Int64	 time	  = Math::Round(Float(samplePosition) / track.GetFormat().rate * muxer->GetTimeScale());
	Int64	 position = muxer->SeekPacket(time);

	if (position < 0) return False;

	delaySamplesLeft = delaySamples + time - position;

	return True;
}

Int BoCA::DecoderFAAD2::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	Void	*samples = NIL;
	Int	 samplesRead = 0;

	samplesBuffer.Resize(0);

	if (muxer != NIL)
	{
		Int64		 numberOfPackets = muxer->GetNumberOfPackets();

		Int64		 position   = 0;
		Int		 flags	    = 0;
		Int		 bufferSize = muxer->ReadPacket(dataBuffer, position, flags);

		if (bufferSize < 0) return -1;

		unsigned char	*buffer	    = dataBuffer;

		NeAACDecFrameInfo frameInfo;

		samples = ex_NeAACDecDecode(handle, &frameInfo, buffer, bufferSize);

		if (!frameInfo.error && frameInfo.samples > 0 && samples != NIL)
		{
			if (frameSize == 0)
			{
				frameSize = frameInfo.samples / frameInfo.channels;

				if (frameInfo.sbr == SBR_UPSAMPLED ||
				    frameInfo.sbr == NO_SBR_UPSAMPLED) sbrRatio = 2;

				/* Get delay from gapless information.
				 */
				Int	 delay	= 0, padding = 0;
				Int64	 length	= 0;

				if (muxer->GetGaplessInfo(delay, padding, length) && (delay + padding + length) * sbrRatio == numberOfPackets * frameSize)
				{
					if (sbrRatio > 1) delay += 480;

					delaySamples	 = delay * sbrRatio;
					delaySamplesLeft = delaySamples;
				}

				/* Set delay samples to minimum encoder delay.
				 */
				if (delaySamples == 0)
				{
					delaySamples	 = frameSize + (sbrRatio == 1 ? 0 : 480) * sbrRatio;
					delaySamplesLeft = delaySamples;
				}

				/* FAAD2 automatically skips the first frame, so
				 * subtract it from the delay sample count.
				 */
				delaySamplesLeft -= frameSize;

				/* Fix delay for LD/ELD object types.
				 */
				if (frameInfo.object_type == LD) delaySamplesLeft += frameSize;
			}

			samplesBuffer.Resize(samplesRead * format.channels + frameInfo.samples);

			if (format.channels == 1 && frameInfo.channels == 2)
			{
				for (UnsignedInt i = 0; i < frameInfo.samples; i++) samplesBuffer[samplesRead + i] = ((int16_t *) samples)[i * 2];
			}
			else
			{
				memcpy(samplesBuffer + samplesRead * format.channels, samples, frameInfo.samples * (format.bits / 8));
			}

			samplesRead += frameSize;
		}
	}
	else
	{
		dataBuffer.Resize(data.Size() + backBuffer.Size());

		Int	 size = driver->ReadData(dataBuffer + backBuffer.Size(), data.Size());

		if (size <= 0) return -1;

		inBytes += size;

		if (backBuffer.Size() > 0)
		{
			memcpy(dataBuffer, backBuffer, backBuffer.Size());

			size += backBuffer.Size();

			backBuffer.Resize(0);
		}

		Int	 bytesConsumed = 0;

		do
		{
			NeAACDecFrameInfo	 frameInfo;

			samples = ex_NeAACDecDecode(handle, &frameInfo, dataBuffer + bytesConsumed, size - bytesConsumed);

		        if (!frameInfo.error && frameInfo.samples > 0 && samples != NIL)
			{
				if (frameSize == 0)
				{
					frameSize	 = frameInfo.samples / frameInfo.channels;

					if (frameInfo.sbr == SBR_UPSAMPLED ||
					    frameInfo.sbr == NO_SBR_UPSAMPLED) sbrRatio = 2;

					/* Set delay samples to minimum encoder delay.
					 */
					delaySamples	 = frameSize + (sbrRatio == 1 ? 0 : 480) * sbrRatio;
					delaySamplesLeft = delaySamples;

					/* FAAD2 automatically skips the first frame, so
					 * subtract it from the delay sample count.
					 */
					delaySamplesLeft -= frameSize;

					/* Fix delay for LD/ELD object types.
					 */
					if (frameInfo.object_type == LD) delaySamplesLeft += frameSize;
				}

				samplesBuffer.Resize(samplesRead * format.channels + frameInfo.samples);

				memcpy(samplesBuffer + samplesRead * format.channels, samples, frameInfo.samples * (format.bits / 8));

				samplesRead += frameSize;
			}

			bytesConsumed += frameInfo.bytesconsumed;

			if ((size - bytesConsumed < bytesConsumed) && (driver->GetPos() < driver->GetSize())) samples = NIL;
		}
		while (samples != NIL);

		if ((size - bytesConsumed) > 0)
		{
			backBuffer.Resize(size - bytesConsumed);

			memcpy(backBuffer, dataBuffer + bytesConsumed, backBuffer.Size());
		}
	}

	data.Resize(0);

	if (samplesRead > delaySamplesLeft)
	{
		data.Resize((samplesRead - delaySamplesLeft) * format.channels * (format.bits / 8));

		memcpy(data, samplesBuffer + delaySamplesLeft * format.channels, data.Size());
	}

	delaySamplesLeft = Math::Max(0, delaySamplesLeft - samplesRead);

	/* Change to default channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::AAC_3_0, Channel::Default_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::AAC_5_0, Channel::Default_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::AAC_5_1, Channel::Default_5_1);

	return data.Size();
}

BoCA::AS::MuxerComponent *BoCA::DecoderFAAD2::OpenMuxer(Driver *ioDriver) const
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

Void BoCA::DecoderFAAD2::CloseMuxer(AS::MuxerComponent *muxer) const
{
	if (muxer == NIL) return;

	muxer->Close();

	AS::Registry::Get().DeleteComponent(muxer);
}

Int BoCA::DecoderFAAD2::GetAudioObjectType(const Buffer<UnsignedByte> &codecConfig) const
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

Bool BoCA::DecoderFAAD2::SkipID3v2Tag(InStream &in)
{
	/* Check for an ID3v2 tag at the beginning of the
	 * file and skip it if it exists as FAAD2 may crash
	 * on unsynchronized tags.
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

Bool BoCA::DecoderFAAD2::SyncOnAACHeader(InStream &in)
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

		return True;
	}

	/* No sync. Probably not an AAC file.
	 */
	return False;
}
