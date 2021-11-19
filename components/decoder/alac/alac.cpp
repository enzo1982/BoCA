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

Bool BoCA::DecoderALAC::CanOpenStream(const String &streamURI)
{
	Bool		 isValidFile = False;
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if ((in.InputNumberRaw(8) & 0xFFFFFFFF) != 'ftyp') return False;

	MP4FileHandle	 mp4File  = ex_MP4ReadProvider(streamURI.ConvertTo("UTF-8"), NIL);
	Int		 mp4Track = GetAudioTrack(mp4File);

	if (mp4Track >= 0 && ex_MP4HaveTrackAtom(mp4File, mp4Track, "mdia.minf.stbl.stsd.alac") &&
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

	MP4FileHandle	 mp4File  = ex_MP4ReadProvider(streamURI.ConvertTo("UTF-8"), NIL);
	Int		 mp4Track = GetAudioTrack(mp4File);

	if (mp4Track >= 0 && ex_MP4GetSampleSize(mp4File, mp4Track, 1) > 0)
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

	mp4Track	 = -1;
	sampleId	 = 1;

	skipSamples	 = 0;
}

BoCA::DecoderALAC::~DecoderALAC()
{
}

Bool BoCA::DecoderALAC::Activate()
{
	InStream	 in(STREAM_DRIVER, driver);

	mp4File	 = ex_MP4ReadProvider(track.fileName.ConvertTo("UTF-8"), NIL);
	mp4Track = GetAudioTrack(mp4File);

	if (mp4Track == -1) return False;

	driver->Seek(0);

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

	return True;
}

Int BoCA::DecoderALAC::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	/* Read MP4 sample.
	 */
	unsigned int	 bufferSize = ex_MP4GetSampleSize(mp4File, mp4Track, sampleId);

	dataBuffer.Resize(bufferSize);

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

	/* Change to default channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::AAC_3_0, Channel::Default_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::AAC_5_0, Channel::Default_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::AAC_5_1, Channel::Default_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::AAC_6_1, Channel::Default_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::AAC_7_1, Channel::Default_7_1);

	return data.Size();
}

Int BoCA::DecoderALAC::GetAudioTrack(MP4FileHandle mp4File) const
{
	Int	 nOfTracks = ex_MP4GetNumberOfTracks(mp4File, NIL, 0);

	for (Int i = 0; i < nOfTracks; i++)
	{
		MP4TrackId	 trackId   = ex_MP4FindTrackId(mp4File, i, NIL, 0);
		String		 trackType = ex_MP4GetTrackType(mp4File, trackId);

		if (trackType == MP4_AUDIO_TRACK_TYPE) return trackId;
	}

	return -1;
}
