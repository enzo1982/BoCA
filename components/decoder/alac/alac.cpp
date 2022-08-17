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

#ifdef __WIN32__
#	define WIN32_LEAN_AND_MEAN
#endif

#include <smooth.h>
#include <smooth/dll.h>

#include "alac.h"

#include "alac/ALACBitUtilities.h"

using namespace smooth::IO;

const String &BoCA::DecoderALAC::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (mp4v2dll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>Apple Lossless Decoder</name>				\
		    <version>1.0</version>					\
		    <id>alac-dec</id>						\
		    <type>decoder</type>					\
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
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeMP4v2DLL();
}

namespace BoCA
{
	int64_t	 MP4IO_size(void *);
	int	 MP4IO_seek(void *, int64_t);
	int	 MP4IO_read(void *, void *, int64_t, int64_t *);

	static MP4IOCallbacks	 mp4Callbacks = { MP4IO_size, MP4IO_seek, MP4IO_read, NIL, NIL };
};

Bool BoCA::DecoderALAC::CanOpenStream(const String &streamURI)
{
	Bool		 isValidFile = False;
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if ((in.InputNumberRaw(8) & 0xFFFFFFFF) != 'ftyp') return False;

	MP4FileHandle	 mp4File  = ex_MP4Read(streamURI.ConvertTo("UTF-8"));
	MP4TrackId	 mp4Track = ex_MP4FindTrackId(mp4File, 0, MP4_AUDIO_TRACK_TYPE, 0);

	if (mp4Track != MP4_INVALID_TRACK_ID && ex_MP4HaveTrackAtom(mp4File, mp4Track, "mdia.minf.stbl.stsd.alac") &&
						ex_MP4GetSampleSize(mp4File, mp4Track, 1) > 0) isValidFile = True;

	ex_MP4Close(mp4File, 0);

	return isValidFile;
}

Error BoCA::DecoderALAC::GetStreamInfo(const String &streamURI, Track &track)
{
	Format		 format = track.GetFormat();
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if ((in.InputNumberRaw(8) & 0xFFFFFFFF) != 'ftyp') return Error();

	track.fileSize	= File(streamURI).GetFileSize();
	track.length	= -1;

	MP4FileHandle	 mp4File  = ex_MP4Read(streamURI.ConvertTo("UTF-8"));
	MP4TrackId	 mp4Track = ex_MP4FindTrackId(mp4File, 0, MP4_AUDIO_TRACK_TYPE, 0);

	if (mp4Track != MP4_INVALID_TRACK_ID && ex_MP4GetSampleSize(mp4File, mp4Track, 1) > 0)
	{
		/* Get codec configuration.
		 */
		unsigned char	*ascBuffer     = NIL;
		unsigned int	 ascBufferSize = 0;

		ex_MP4GetTrackBytesProperty(mp4File, mp4Track, "mdia.minf.stbl.stsd.alac.alac.decoderConfig", (uint8_t **) &ascBuffer, (uint32_t *) &ascBufferSize);

		/* Work around a bug in MP4v2 that ignores the version
		 * and flags bytes at the start of the alac atom.
		 */
		Int	 offset = 0;

		if (ascBufferSize == 28 || ascBufferSize == 52) offset = 4;

		/* Init decoder to get format info.
		 */
		if (decoder.Init(ascBuffer + offset, ascBufferSize - offset) == ALAC_noErr)
		{
			format.rate	= decoder.mConfig.sampleRate;
			format.channels = decoder.mConfig.numChannels;

			track.length	= ex_MP4GetTrackDuration(mp4File, mp4Track);

			format.bits	= decoder.mConfig.bitDepth;
		}
		else
		{
			errorState  = True;
			errorString = "Unsupported audio format";
		}

		ex_MP4Free(ascBuffer);

		track.SetFormat(format);

		/* Read gapless information.
		 */
		Int	 delay	= 0, padding = 0;
		Int64	 length	= 0;

		if (ReadGaplessInfo(mp4File, delay, padding, length) && delay == 0 && padding + length == Int64(ex_MP4GetTrackNumberOfSamples(mp4File, mp4Track)) * decoder.mConfig.frameLength)
		{
			track.length = length;
		}
	}

	ex_MP4Close(mp4File, 0);

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
	mp4File		 = NIL;

	mp4Track	 = MP4_INVALID_TRACK_ID;
	sampleId	 = 1;

	skipSamples	 = 0;
	samplesLeft	 = 0;
}

BoCA::DecoderALAC::~DecoderALAC()
{
}

Bool BoCA::DecoderALAC::Activate()
{
	InStream	 in(STREAM_DRIVER, driver);

	mp4File	 = ex_MP4ReadCallbacks(&mp4Callbacks, driver);
	mp4Track = ex_MP4FindTrackId(mp4File, 0, MP4_AUDIO_TRACK_TYPE, 0);

	if (mp4Track == MP4_INVALID_TRACK_ID)
	{
		ex_MP4Close(mp4File, 0);

		return False;
	}

	samplesLeft = track.length;

	/* Get codec configuration.
	 */
	unsigned char	*ascBuffer     = NIL;
	unsigned int	 ascBufferSize = 0;

	ex_MP4GetTrackBytesProperty(mp4File, mp4Track, "mdia.minf.stbl.stsd.alac.alac.decoderConfig", (uint8_t **) &ascBuffer, (uint32_t *) &ascBufferSize);

	/* Work around a bug in MP4v2 that ignores the version
	 * and flags bytes at the start of the alac atom.
	 */
	Int	 offset = 0;

	if (ascBufferSize == 28 || ascBufferSize == 52) offset = 4;

	/* Init decoder.
	 */
	if (decoder.Init(ascBuffer + offset, ascBufferSize - offset) != ALAC_noErr) errorState = True;

	ex_MP4Free(ascBuffer);

	return !errorState;
}

Bool BoCA::DecoderALAC::Deactivate()
{
	/* Close MP4 file.
	 */
	ex_MP4Close(mp4File, 0);

	return True;
}

Bool BoCA::DecoderALAC::Seek(Int64 samplePosition)
{
	MP4Timestamp	 time = Math::Round(Float(samplePosition) / track.GetFormat().rate * ex_MP4GetTrackTimeScale(mp4File, mp4Track));

	sampleId    = ex_MP4GetSampleIdFromTime(mp4File, mp4Track, time, true);
	skipSamples = time - ex_MP4GetSampleTime(mp4File, mp4Track, sampleId);
	samplesLeft = track.sampleOffset + track.length - samplePosition;

	return True;
}

Int BoCA::DecoderALAC::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	/* Read MP4 sample.
	 *
	 * Allocate 4 extra bytes for the buffer as the ALAC decoder always reads
	 * 32 bits ahead, even at the end of the sample buffer. The extra bytes
	 * prevent this from crossing page boundaries, causing access violations.
	 */
	unsigned int	 bufferSize = ex_MP4GetSampleSize(mp4File, mp4Track, sampleId);

	dataBuffer.Resize(bufferSize + 4);

	unsigned char	*buffer	    = dataBuffer;

	if (!ex_MP4ReadSample(mp4File, mp4Track, sampleId++, (uint8_t **) &buffer, (uint32_t *) &bufferSize, NIL, NIL, NIL, NIL) || bufferSize == 0) return -1;

	/* Fill ALAC decoder bit buffer.
	 */
	BitBuffer	 bits;

	BitBufferInit(&bits, buffer, bufferSize);

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

Bool BoCA::DecoderALAC::ReadGaplessInfo(MP4FileHandle mp4File, Int &delay, Int &padding, Int64 &length) const
{
	Bool	 result = False;

	/* Look for iTunes metadata with gapless information.
	 */
	MP4ItmfItemList	*items = ex_MP4ItmfGetItemsByMeaning(mp4File, "com.apple.iTunes", "iTunSMPB");

	if (items != NIL)
	{
		if (items->size == 1)
		{
			/* Read value as string.
			 */
			Buffer<char>	 value(items->elements[0].dataList.elements[0].valueSize + 1);

			memset(value, 0, items->elements[0].dataList.elements[0].valueSize + 1);
			memcpy(value, items->elements[0].dataList.elements[0].value, items->elements[0].dataList.elements[0].valueSize);

			/* Parse value string.
			 */
			const Array<String>	&values = String(value).Trim().Explode(" ");

			delay	= (Int64) Number::FromHexString(values.GetNth(1));
			padding	= (Int64) Number::FromHexString(values.GetNth(2));
			length	= (Int64) Number::FromHexString(values.GetNth(3));

			result = True;
		}

		ex_MP4ItmfItemListFree(items);
	}

	return result;
}

int64_t BoCA::MP4IO_size(void *handle)
{
	Driver	*driver = (Driver *) handle;

	return driver->GetSize();
}

int BoCA::MP4IO_seek(void *handle, int64_t pos)
{
	Driver	*driver = (Driver *) handle;

	if (driver->Seek(pos) == -1) return 1;

	return 0;
}

int BoCA::MP4IO_read(void *handle, void *buffer, int64_t size, int64_t *nout)
{
	Driver	*driver = (Driver *) handle;

	*nout = driver->ReadData((UnsignedByte *) buffer, size);

	if (*nout == 0) return 1;

	return 0;
}
