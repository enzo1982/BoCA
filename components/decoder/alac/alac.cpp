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

#ifdef __WIN32__
#	define WIN32_LEAN_AND_MEAN
#endif

#include <smooth.h>

#include <smooth/io/drivers/driver_ansi.h>

#include "alac.h"

#include "alac/ALACBitUtilities.h"

using namespace smooth::IO;

const String &BoCA::DecoderALAC::GetComponentSpecs()
{
	static String	 componentSpecs = "				\
									\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
	  <component>							\
	    <name>Apple Lossless Decoder</name>				\
	    <version>1.0</version>					\
	    <id>alac-dec</id>						\
	    <type>decoder</type>					\
	    <require>mp4-mux</require>					\
	    <replace>alac-decoder-dec</replace>				\
	    <replace>ffmpeg-alac-dec</replace>				\
	    <format>							\
	      <name>Apple Lossless Files</name>				\
	      <lossless>true</lossless>					\
	      <extension>m4a</extension>				\
	      <extension>m4b</extension>				\
	      <extension>mp4</extension>				\
	      <tag id=\"mp4-tag\" mode=\"other\">MP4 Metadata</tag>	\
	    </format>							\
	  </component>							\
									\
	";

	return componentSpecs;
}

Bool BoCA::DecoderALAC::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if ((in.InputNumberRaw(8) & 0xFFFFFFFF) != 'ftyp') return False;

	/* Open MP4 file and check for an ALAC track.
	 */
	DriverANSI		 ioDriver(File(streamURI), IS_READ);
	AS::MuxerComponent	*muxer = OpenMuxer(&ioDriver);

	if (muxer == NIL) return False;

	Bool	 isValidFile = (muxer->GetCodec() == MUXER_CODEC_ALAC);

	CloseMuxer(muxer);

	return isValidFile;
}

Error BoCA::DecoderALAC::GetStreamInfo(const String &streamURI, Track &track)
{
	Format		 format = track.GetFormat();
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if ((in.InputNumberRaw(8) & 0xFFFFFFFF) != 'ftyp') return Error();

	track.fileSize	= File(streamURI).GetFileSize();
	track.length	= -1;

	/* Open MP4 file.
	 */
	DriverANSI		 ioDriver(File(streamURI), IS_READ);
	AS::MuxerComponent	*muxer = OpenMuxer(&ioDriver);

	if (muxer != NIL)
	{
		/* Get codec configuration.
		 */
		Buffer<UnsignedByte>	 codecConfig;

		muxer->GetCodecConfig(codecConfig);

		/* Init decoder to get format info.
		 */
		if (decoder.Init(codecConfig, codecConfig.Size()) == ALAC_noErr)
		{
			format.rate	= decoder.mConfig.sampleRate;
			format.channels = decoder.mConfig.numChannels;

			track.length	= muxer->GetDuration();

			format.bits	= decoder.mConfig.bitDepth;
		}
		else
		{
			errorState  = True;
			errorString = "Unsupported audio format";
		}

		track.SetFormat(format);

		/* Read gapless information.
		 */
		Int	 delay	= 0, padding = 0;
		Int64	 length	= 0;

		if (muxer->GetGaplessInfo(delay, padding, length) && delay == 0 && padding + length == muxer->GetNumberOfPackets() * decoder.mConfig.frameLength)
		{
			track.length = length;
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

	track.SetFormat(format);

	if (!errorState) return Success();
	else		 return Error();
}

BoCA::DecoderALAC::DecoderALAC()
{
	muxer		 = NIL;

	skipSamples	 = 0;
	samplesLeft	 = 0;
}

BoCA::DecoderALAC::~DecoderALAC()
{
}

Bool BoCA::DecoderALAC::Activate()
{
	/* Open MP4 file.
	 */
	muxer = OpenMuxer(driver);

	if (muxer == NIL) return False;

	samplesLeft = track.length;

	/* Get codec configuration.
	 */
	Buffer<UnsignedByte>	 codecConfig;

	muxer->GetCodecConfig(codecConfig);

	/* Init decoder.
	 */
	if (decoder.Init(codecConfig, codecConfig.Size()) != ALAC_noErr) errorState = True;

	return !errorState;
}

Bool BoCA::DecoderALAC::Deactivate()
{
	/* Close MP4 file.
	 */
	CloseMuxer(muxer);

	muxer = NIL;

	return True;
}

Bool BoCA::DecoderALAC::Seek(Int64 samplePosition)
{
	Int64	 time	  = Math::Round(Float(samplePosition) / track.GetFormat().rate * muxer->GetTimeScale());
	Int64	 position = muxer->SeekPacket(time);

	if (position < 0) return False;

	skipSamples = time - position;
	samplesLeft = track.sampleOffset + track.length - samplePosition;

	return True;
}

Int BoCA::DecoderALAC::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	/* Read MP4 sample.
	 */
	Int64	 position   = 0;
	Int	 flags	    = 0;
	Int	 bufferSize = muxer->ReadPacket(dataBuffer, position, flags);

	if (bufferSize <= 0) return -1;

	/* Allocate 4 extra bytes for the buffer as the ALAC decoder always reads
	 * 32 bits ahead, even at the end of the sample buffer. The extra bytes
	 * prevent this from crossing page boundaries, causing access violations.
	 */
	dataBuffer.Resize(bufferSize + 4);

	/* Fill ALAC decoder bit buffer.
	 */
	BitBuffer	 bits;

	BitBufferInit(&bits, dataBuffer, bufferSize);

	/* Prepare output buffer and decode samples.
	 */
	Int		 bytesPerSample = format.channels * (format.bits / 8);
	uint32_t	 samplesRead	= 0;

	data.Resize(decoder.mConfig.frameLength * bytesPerSample);

	if (decoder.Decode(&bits, data, decoder.mConfig.frameLength, format.channels, &samplesRead) != ALAC_noErr) return -1;

	/* Adjust buffer for samples to skip.
	 */
	if (skipSamples > 0)
	{
		Int	 samplesToSkip = Math::Min(samplesRead, skipSamples);

		memcpy(data, data + samplesToSkip * bytesPerSample, (samplesRead - samplesToSkip) * bytesPerSample);

		data.Resize((samplesRead - samplesToSkip) * bytesPerSample);

		skipSamples -= samplesToSkip;
	}

	/* Cut data buffer on last frame.
	 */
	data.Resize(Math::Min(data.Size(), samplesLeft * bytesPerSample));

	samplesLeft -= data.Size() / bytesPerSample;

	/* Change to default channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::AAC_3_0, Channel::Default_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::AAC_5_0, Channel::Default_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::AAC_5_1, Channel::Default_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::AAC_6_1, Channel::Default_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::AAC_7_1, Channel::Default_7_1);

	return data.Size();
}

BoCA::AS::MuxerComponent *BoCA::DecoderALAC::OpenMuxer(Driver *ioDriver) const
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

Void BoCA::DecoderALAC::CloseMuxer(AS::MuxerComponent *muxer) const
{
	if (muxer == NIL) return;

	muxer->Close();

	AS::Registry::Get().DeleteComponent(muxer);
}
