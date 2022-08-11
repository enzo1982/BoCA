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

#include "faad2.h"

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

		if (mp4v2dll != NIL)
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
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeFAAD2DLL();
	FreeMP4v2DLL();
}

namespace BoCA
{
	int64_t	 MP4IO_size(void *);
	int	 MP4IO_seek(void *, int64_t);
	int	 MP4IO_read(void *, void *, int64_t, int64_t *);

	static MP4IOCallbacks	 mp4Callbacks = { MP4IO_size, MP4IO_seek, MP4IO_read, NIL, NIL };
};

Bool BoCA::DecoderFAAD2::CanOpenStream(const String &streamURI)
{
	Bool		 isValidFile = False;
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (mp4v2dll != NIL && (in.InputNumberRaw(8) & 0xFFFFFFFF) == 'ftyp')
	{
		MP4FileHandle	 mp4File  = ex_MP4Read(streamURI.ConvertTo("UTF-8"));
		MP4TrackId	 mp4Track = ex_MP4FindTrackId(mp4File, 0, MP4_AUDIO_TRACK_TYPE, 0);

		if (mp4Track != MP4_INVALID_TRACK_ID && ex_MP4GetSampleSize(mp4File, mp4Track, 1) > 0)
		{
			Int	 type = ex_MP4GetTrackAudioMpeg4Type(mp4File, mp4Track);

			if (type == MAIN   ||
			    type == LC	   ||
			    type == LTP	   ||
			    type == HE_AAC ||
			    type == LD	   ||
			    type == PS	   ||

			    type == ER_LC  ||
			    type == ER_LTP) isValidFile = True;
		}

		ex_MP4Close(mp4File, 0);
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

		MP4FileHandle	 mp4File  = ex_MP4Read(streamURI.ConvertTo("UTF-8"));
		MP4TrackId	 mp4Track = ex_MP4FindTrackId(mp4File, 0, MP4_AUDIO_TRACK_TYPE, 0);

		if (mp4Track != MP4_INVALID_TRACK_ID && ex_MP4GetSampleSize(mp4File, mp4Track, 1) > 0)
		{
			NeAACDecHandle			 handle	 = ex_NeAACDecOpen();
			NeAACDecConfigurationPtr	 fConfig = ex_NeAACDecGetCurrentConfiguration(handle);

			fConfig->defSampleRate	= 44100;
			fConfig->defObjectType	= LC;
			fConfig->outputFormat	= FAAD_FMT_16BIT;

			ex_NeAACDecSetConfiguration(handle, fConfig);

			/* Get codec configuration.
			 */
			unsigned char	*escBuffer     = NIL;
			unsigned long	 escBufferSize = 0;

			ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (uint8_t **) &escBuffer, (uint32_t *) &escBufferSize);

			if (ex_NeAACDecInit2(handle, escBuffer, escBufferSize, (unsigned long *) &format.rate, (unsigned char *) &format.channels) < 0)
			{
				errorState  = True;
				errorString = "Unsupported audio format";
			}

			ex_MP4Free(escBuffer);

			/* Decode frames to get frame size.
			 */
			while (frameSize == 0 && !errorState)
			{
				unsigned int	 bufferSize = ex_MP4GetSampleSize(mp4File, mp4Track, sampleId);
				unsigned char	*buffer	    = new unsigned char [bufferSize];

				ex_MP4ReadSample(mp4File, mp4Track, sampleId++, (uint8_t **) &buffer, (uint32_t *) &bufferSize, NIL, NIL, NIL, NIL);

				NeAACDecFrameInfo frameInfo;

				ex_NeAACDecDecode(handle, &frameInfo, buffer, bufferSize);

				if (frameInfo.error)
				{
					errorState  = True;
					errorString = "Unsupported audio format";
				}

				frameSize = frameInfo.samples / format.channels;

				if (frameInfo.sbr == SBR_UPSAMPLED ||
				    frameInfo.sbr == NO_SBR_UPSAMPLED) sbrRatio = 2;

				delete [] buffer;
			}

			/* Get track length.
			 */
			track.length	= Int64(ex_MP4GetTrackNumberOfSamples(mp4File, mp4Track)) * frameSize;
			track.length   -= frameSize; // To account for encoder delay.

			format.bits	= 16;

			ex_NeAACDecClose(handle);

			track.SetFormat(format);

			/* Read gapless information.
			 */
			Int	 delay	= 0, padding = 0;
			Int64	 length	= 0;

			if (ReadGaplessInfo(mp4File, delay, padding, length) && (delay + padding + length) * sbrRatio == Int64(ex_MP4GetTrackNumberOfSamples(mp4File, mp4Track)) * frameSize)
			{
				track.length = length * sbrRatio;
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
	mp4File		 = NIL;
	handle		 = NIL;
	fConfig		 = NIL;

	mp4Track	 = MP4_INVALID_TRACK_ID;
	sampleId	 = 1;

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
		mp4File	 = ex_MP4ReadCallbacks(&mp4Callbacks, driver);
		mp4Track = ex_MP4FindTrackId(mp4File, 0, MP4_AUDIO_TRACK_TYPE, 0);

		if (mp4Track == MP4_INVALID_TRACK_ID)
		{
			ex_MP4Close(mp4File, 0);

			return False;
		}
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

	if (mp4File != NIL)
	{
		/* Get codec configuration.
		 */
		unsigned char	*escBuffer     = NIL;
		unsigned long	 escBufferSize = 0;

		ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (uint8_t **) &escBuffer, (uint32_t *) &escBufferSize);

		unsigned long	 rate;
		unsigned char	 channels;

		if (ex_NeAACDecInit2(handle, escBuffer, escBufferSize, &rate, &channels) < 0)
		{
			errorState  = True;
			errorString = "Unsupported audio format";
		}

		ex_MP4Free(escBuffer);
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

		if (mp4File != NIL) ex_MP4Close(mp4File, 0);

		return False;
	}

	return True;
}

Bool BoCA::DecoderFAAD2::Deactivate()
{
	/* Close decoder.
	 */
	ex_NeAACDecClose(handle);

	if (mp4File == NIL) return True;

	/* Close MP4 file.
	 */
	ex_MP4Close(mp4File, 0);

	return True;
}

Bool BoCA::DecoderFAAD2::Seek(Int64 samplePosition)
{
	if (mp4File == NIL) return False;

	MP4Timestamp	 time = Math::Round(Float(samplePosition) / track.GetFormat().rate * ex_MP4GetTrackTimeScale(mp4File, mp4Track));

	sampleId	 = ex_MP4GetSampleIdFromTime(mp4File, mp4Track, time, true);
	delaySamplesLeft = delaySamples + time - ex_MP4GetSampleTime(mp4File, mp4Track, sampleId);

	return True;
}

Int BoCA::DecoderFAAD2::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	Void	*samples = NIL;
	Int	 samplesRead = 0;

	samplesBuffer.Resize(0);

	if (mp4File != NIL)
	{
		MP4SampleId	 numberOfSamples = ex_MP4GetTrackNumberOfSamples(mp4File, mp4Track);

		if (sampleId > numberOfSamples) return -1;

		unsigned int	 bufferSize = ex_MP4GetSampleSize(mp4File, mp4Track, sampleId);

		dataBuffer.Resize(bufferSize);

		unsigned char	*buffer	    = dataBuffer;

		if (!ex_MP4ReadSample(mp4File, mp4Track, sampleId++, (uint8_t **) &buffer, (uint32_t *) &bufferSize, NIL, NIL, NIL, NIL)) return -1;

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

				if (ReadGaplessInfo(mp4File, delay, padding, length) && (delay + padding + length) * sbrRatio == Int64(numberOfSamples) * frameSize)
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

			memcpy(samplesBuffer + samplesRead * format.channels, samples, frameInfo.samples * (format.bits / 8));

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

Bool BoCA::DecoderFAAD2::ReadGaplessInfo(MP4FileHandle mp4File, Int &delay, Int &padding, Int64 &length) const
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
