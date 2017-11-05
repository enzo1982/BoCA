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

#include "faad2.h"

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

Bool BoCA::DecoderFAAD2::CanOpenStream(const String &streamURI)
{
	Bool		 isValidFile = False;
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (mp4v2dll != NIL && (in.InputNumberRaw(8) & 0xFFFFFFFF) == 'ftyp')
	{
		MP4FileHandle	 mp4File;

		if (String::IsUnicode(streamURI))
		{
			File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));

			mp4File = ex_MP4Read(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));
		}
		else
		{
			mp4File = ex_MP4Read(streamURI);
		}

		Int	 mp4Track = GetAudioTrack(mp4File);

		if (mp4Track >= 0 && ex_MP4GetSampleSize(mp4File, mp4Track, 1) > 0)
		{
			Int	 type = ex_MP4GetTrackAudioMpeg4Type(mp4File, mp4Track);

			if (type == ER_LC			  ||
			    type == ER_LTP			  ||
			    type == LD				  ||

			    type == MP4_MPEG4_AAC_MAIN_AUDIO_TYPE ||
			    type == MP4_MPEG4_AAC_LC_AUDIO_TYPE	  ||
			    type == MP4_MPEG4_AAC_SSR_AUDIO_TYPE  ||
			    type == MP4_MPEG4_AAC_LTP_AUDIO_TYPE  ||
			    type == MP4_MPEG4_AAC_HE_AUDIO_TYPE) isValidFile = True;
		}

		ex_MP4Close(mp4File, 0);

		if (String::IsUnicode(streamURI))
		{
			File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in")).Delete();
		}
	}
	else
	{
		in.Seek(0);

		SkipID3v2Tag(&in);

		isValidFile = SyncOnAACHeader(&in);
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

		MP4FileHandle	 mp4File;

		if (String::IsUnicode(streamURI))
		{
			File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));

			mp4File = ex_MP4Read(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));
		}
		else
		{
			mp4File = ex_MP4Read(streamURI);
		}

		Int	 mp4Track = GetAudioTrack(mp4File);

		if (mp4Track >= 0 && ex_MP4GetSampleSize(mp4File, mp4Track, 1) > 0)
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

			ex_NeAACDecInit2(handle, escBuffer, escBufferSize, (unsigned long *) &format.rate, (unsigned char *) &format.channels);

			ex_MP4Free(escBuffer);

			track.length	= Math::Round(ex_MP4GetTrackDuration(mp4File, mp4Track) * Float(format.rate / ex_MP4GetTrackTimeScale(mp4File, mp4Track)));

			format.bits	= 16;

			ex_NeAACDecClose(handle);

			track.SetFormat(format);

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

					track.length = Math::Round((Int64) Number::FromHexString(values.GetNth(3)) * Float(format.rate / ex_MP4GetTrackTimeScale(mp4File, mp4Track)));

					String::ExplodeFinish();
				}

				ex_MP4ItmfItemListFree(items);
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

				if (String::IsUnicode(streamURI)) tagger->ParseStreamInfo(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"), track);
				else				  tagger->ParseStreamInfo(streamURI, track);

				boca.DeleteComponent(tagger);
			}
		}

		if (String::IsUnicode(streamURI))
		{
			File(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in")).Delete();
		}
	}
	else
	{
		in.Seek(0);

		format.bits	= 16;

		track.fileSize	= in.Size();
		track.length	= -1;

		SkipID3v2Tag(&in);

		if (!SyncOnAACHeader(&in))
		{
			errorState  = True;
			errorString = "No AAC file.";

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

		ex_NeAACDecInit(handle, data, size, (unsigned long *) &format.rate, (unsigned char *) &format.channels);

		/* Compute approximate length of stream.
		 */
		Void	*samples       = NIL;
		Int	 bytesConsumed = 0;
		Int	 samplesRead   = 0;

		do
		{
			NeAACDecFrameInfo	 frameInfo;

			samples = ex_NeAACDecDecode(handle, &frameInfo, data + bytesConsumed, size - bytesConsumed);

			if (!frameInfo.error)
			{
				bytesConsumed += frameInfo.bytesconsumed;
				samplesRead   += frameInfo.samples;
			}
		}
		while (samples != NIL);

		if (samplesRead > 0) track.approxLength = samplesRead / format.channels * (track.fileSize / bytesConsumed);

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
	packageSize	 = 0;

	mp4File		 = NIL;
	handle		 = NIL;
	fConfig		 = NIL;

	mp4Track	 = -1;
	sampleId	 = 0;

	frameSize	 = 0;

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
		if (String::IsUnicode(track.origFilename))
		{
			File(track.origFilename).Copy(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in"));

			mp4File	= ex_MP4Read(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in"));
		}
		else
		{
			mp4File	= ex_MP4Read(track.origFilename);
		}

		mp4Track = GetAudioTrack(mp4File);

		if (mp4Track == -1) return False;
	}
	else
	{
		in.Seek(0);

		SkipID3v2Tag(&in);
		SyncOnAACHeader(&in);

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

		ex_NeAACDecInit2(handle, escBuffer, escBufferSize, &rate, &channels);

		ex_MP4Free(escBuffer);

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

				delaySamples	 = Math::Round((Int64) Number::FromHexString(values.GetNth(1)) * Float(track.GetFormat().rate / ex_MP4GetTrackTimeScale(mp4File, mp4Track)));
				delaySamplesLeft = delaySamples;

				String::ExplodeFinish();
			}

			ex_MP4ItmfItemListFree(items);
		}

		sampleId = 1;
	}
	else
	{
		Int		 size = 4096;
		unsigned char	*data = new unsigned char [size];

		size = driver->ReadData(data, size);

		unsigned long	 rate;
		unsigned char	 channels;

		ex_NeAACDecInit(handle, data, size, &rate, &channels);

		delete [] data;

		driver->Seek(driver->GetPos() - size);
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

	if (String::IsUnicode(track.origFilename))
	{
		File(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in")).Delete();
	}

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

				if (delaySamples == 0)
				{
					delaySamples	 = frameSize;
					delaySamplesLeft = frameSize;
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

			samplesRead += frameInfo.samples / format.channels;
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

					delaySamples	 = frameSize;
					delaySamplesLeft = frameSize;

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

				samplesRead += frameInfo.samples / format.channels;
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

Int BoCA::DecoderFAAD2::GetAudioTrack(MP4FileHandle mp4File) const
{
	Int	 nOfTracks = ex_MP4GetNumberOfTracks(mp4File, NIL, 0);

	for (Int i = 0; i < nOfTracks; i++)
	{
		MP4TrackId	 trackId	= ex_MP4FindTrackId(mp4File, i, NIL, 0);
		String		 trackType	= ex_MP4GetTrackType(mp4File, trackId);

		if (trackType == MP4_AUDIO_TRACK_TYPE) return trackId;
	}

	return -1;
}

Bool BoCA::DecoderFAAD2::SkipID3v2Tag(InStream *in)
{
	/* Check for an ID3v2 tag at the beginning of the
	 * file and skip it if it exists as FAAD2 may crash
	 * on unsynchronized tags.
	 */
	if (in->InputString(3) == "ID3")
	{
		in->InputNumber(2); // ID3 version
		in->InputNumber(1); // Flags

		/* Read tag size as a 4 byte unsynchronized integer.
		 */
		Int	 tagSize = (in->InputNumber(1) << 21) +
				   (in->InputNumber(1) << 14) +
				   (in->InputNumber(1) <<  7) +
				   (in->InputNumber(1)      );

		in->RelSeek(tagSize);

		inBytes += (tagSize + 10);
	}
	else
	{
		in->Seek(0);
	}

	return True;
}

Bool BoCA::DecoderFAAD2::SyncOnAACHeader(InStream *in)
{
	Int	 startPos = in->GetPos();

	/* Try to sync on ADIF header.
	 */
	for (Int n = 0; n < 1024; n++)
	{
		if (in->InputNumber(1) != 'A') continue;
		if (in->InputNumber(1) != 'D') continue;
		if (in->InputNumber(1) != 'I') continue;
		if (in->InputNumber(1) != 'F') continue;

		/* No ADIF magic word found in the first 1 kB.
		 */
		if (n == 1023) break;

		in->RelSeek(-4);

		inBytes += n;

		return True;
	}

	in->Seek(startPos);

	/* Try to sync on ADTS header.
	 */
	for (Int n = 0; n < 1024; n++)
	{
		if (  in->InputNumber(1)	       != 0xFF) continue;
		if ( (in->InputNumber(1) & 0xF6)       != 0xF0) continue;
		if (((in->InputNumber(1) & 0x3C) >> 2) >=   12) continue;

		/* No ADTS sync found in the first 1 kB.
		 */
		if (n == 1023) break;

		in->RelSeek(-3);

		inBytes += n;

		return True;
	}

	/* No sync; probably not an AAC file.
	 */
	return False;
}
