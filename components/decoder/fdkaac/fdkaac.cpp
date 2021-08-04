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

#include <smooth.h>
#include <smooth/dll.h>

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
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeFDKAACDLL();
	FreeMP4v2DLL();
}

Bool BoCA::DecoderFDKAAC::CanOpenStream(const String &streamURI)
{
	Bool		 isValidFile = False;
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (mp4v2dll != NIL && (in.InputNumberRaw(8) & 0xFFFFFFFF) == 'ftyp')
	{
		MP4FileHandle	 mp4File  = ex_MP4ReadProvider(streamURI.ConvertTo("UTF-8"), NIL);
		Int		 mp4Track = GetAudioTrack(mp4File);

		if (mp4Track >= 0 && ex_MP4GetSampleSize(mp4File, mp4Track, 1) > 0)
		{
			LIB_INFO	 info[FDK_MODULE_LAST];

			FDKinitLibInfo(info);
			ex_aacDecoder_GetLibInfo(info);

			UINT	 aacFlags = FDKlibInfo_getCapabilities(info, FDK_AACDEC);
			UINT	 sbrFlags = FDKlibInfo_getCapabilities(info, FDK_SBRDEC);

			Int	 type	  = ex_MP4GetTrackAudioMpeg4Type(mp4File, mp4Track);

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

		ex_MP4Close(mp4File, 0);
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

		MP4FileHandle	 mp4File  = ex_MP4ReadProvider(streamURI.ConvertTo("UTF-8"), NIL);
		Int		 mp4Track = GetAudioTrack(mp4File);

		if (mp4Track >= 0 && ex_MP4GetSampleSize(mp4File, mp4Track, 1) > 0)
		{
			HANDLE_AACDECODER	 handle = ex_aacDecoder_Open(TT_MP4_RAW, 1);

			/* Get codec configuration.
			 */
			unsigned char	*escBuffer     = NIL;
			unsigned int	 escBufferSize = 0;

			ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (uint8_t **) &escBuffer, (uint32_t *) &escBufferSize);

			/* The FDK bitstream reader reads up to 4 bytes behind the buffer,
			 * so allocate a larger buffer to prevent access violations.
			 */
			unsigned char	*escBuffer2 = new unsigned char [escBufferSize + 4];

			memcpy(escBuffer2, escBuffer, escBufferSize);

			ex_aacDecoder_ConfigRaw(handle, &escBuffer2, &escBufferSize);

			delete [] escBuffer2;

			ex_MP4Free(escBuffer);

			/* Decode one frame to initialize decoder.
			 */
			unsigned int	 bufferSize = ex_MP4GetSampleSize(mp4File, mp4Track, 1);
			unsigned char	*buffer	    = new unsigned char [bufferSize + 4];

			ex_MP4ReadSample(mp4File, mp4Track, 1, (uint8_t **) &buffer, (uint32_t *) &bufferSize, NIL, NIL, NIL, NIL);

			unsigned int	 bytesValid = bufferSize;

			ex_aacDecoder_Fill(handle, &buffer, &bufferSize, &bytesValid);

			short		*outputBuffer = new short [16384];

			if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, 16384, 0) != AAC_DEC_OK)
			{
				errorState  = True;
				errorString = "Unsupported audio format";
			}

			delete [] outputBuffer;
			delete [] buffer;

			/* Get sample rate and number of channels.
			 */
			if (!errorState)
			{
				CStreamInfo	*streamInfo = ex_aacDecoder_GetStreamInfo(handle);

				frameSize	= streamInfo->frameSize;
				sbrRatio	= streamInfo->frameSize / streamInfo->aacSamplesPerFrame;

				format.rate	= streamInfo->sampleRate;
				format.channels = streamInfo->numChannels;

				track.length	= Int64(ex_MP4GetTrackNumberOfSamples(mp4File, mp4Track)) * streamInfo->frameSize;
				track.length   -= streamInfo->frameSize; // To account for encoder delay.

				format.bits	= 16;
			}

			ex_aacDecoder_Close(handle);

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
	mp4File		 = NIL;
	handle		 = NIL;

	mp4Track	 = -1;
	sampleId	 = 1;

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
		mp4File	 = ex_MP4ReadProvider(track.fileName.ConvertTo("UTF-8"), NIL);
		mp4Track = GetAudioTrack(mp4File);

		if (mp4Track == -1)
		{
			ex_MP4Close(mp4File, 0);

			return False;
		}

		driver->Seek(0);

		handle	 = ex_aacDecoder_Open(TT_MP4_RAW, 1);

		/* Get codec configuration.
		 */
		unsigned char	*escBuffer     = NIL;
		unsigned int	 escBufferSize = 0;

		ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (uint8_t **) &escBuffer, (uint32_t *) &escBufferSize);

		/* The FDK bitstream reader reads up to 4 bytes behind the buffer,
		 * so allocate a larger buffer to prevent access violations.
		 */
		unsigned char	*escBuffer2 = new unsigned char [escBufferSize + 4];

		memcpy(escBuffer2, escBuffer, escBufferSize);

		ex_aacDecoder_ConfigRaw(handle, &escBuffer2, &escBufferSize);

		delete [] escBuffer2;

		ex_MP4Free(escBuffer);
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

	if (mp4File == NIL) return True;

	/* Close MP4 file.
	 */
	ex_MP4Close(mp4File, 0);

	return True;
}

Bool BoCA::DecoderFDKAAC::Seek(Int64 samplePosition)
{
	if (mp4File == NIL) return False;

	MP4Timestamp	 time = Math::Round(Float(samplePosition) / track.GetFormat().rate * ex_MP4GetTrackTimeScale(mp4File, mp4Track));

	sampleId	 = ex_MP4GetSampleIdFromTime(mp4File, mp4Track, time, true);
	delaySamplesLeft = delaySamples + time - ex_MP4GetSampleTime(mp4File, mp4Track, sampleId);

	return True;
}

Int BoCA::DecoderFDKAAC::ReadData(Buffer<UnsignedByte> &data)
{
	static Int	 maxFrameSize = 2048;

	if (finished) return -1;

	const Format	&format = track.GetFormat();

	Int	 samplesRead = 0;

	samplesBuffer.Resize(0);

	if (mp4File != NIL)
	{
		MP4SampleId	 numberOfSamples = ex_MP4GetTrackNumberOfSamples(mp4File, mp4Track);

		if (sampleId <= numberOfSamples)
		{
			unsigned int	 bufferSize = ex_MP4GetSampleSize(mp4File, mp4Track, sampleId);

			dataBuffer.Resize(bufferSize + 4); // + 4 to account for FDK bitstream implementation overreading.

			unsigned char	*buffer	    = dataBuffer;

			if (!ex_MP4ReadSample(mp4File, mp4Track, sampleId++, (uint8_t **) &buffer, (uint32_t *) &bufferSize, NIL, NIL, NIL, NIL)) return -1;

			unsigned int	 bytesValid = bufferSize;

			if (bytesValid > 0)
			{
				unsigned char	*inputBuffer	 = buffer + bufferSize - bytesValid;
				unsigned int	 inputBufferSize = bytesValid;

				ex_aacDecoder_Fill(handle, &inputBuffer, &inputBufferSize, &bytesValid);
			}

			if (frameSize == 0) samplesBuffer.Resize((samplesRead + maxFrameSize) * format.channels);
			else		    samplesBuffer.Resize((samplesRead + frameSize) * format.channels);

			short	*outputBuffer = samplesBuffer + samplesRead * format.channels;

			if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, samplesBuffer.Size() - samplesRead * format.channels, 0) == AAC_DEC_OK)
			{
				if (frameSize == 0)
				{
					CStreamInfo	*streamInfo = ex_aacDecoder_GetStreamInfo(handle);

					frameSize = streamInfo->frameSize;
					sbrRatio  = streamInfo->frameSize / streamInfo->aacSamplesPerFrame;

					/* Get delay from gapless information.
					 */
					Int	 delay	= 0, padding = 0;
					Int64	 length	= 0;

					if (ReadGaplessInfo(mp4File, delay, padding, length) && (delay + padding + length) * sbrRatio == Int64(numberOfSamples) * frameSize)
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

					samplesBuffer.Resize((samplesRead + frameSize) * format.channels);
				}

				samplesRead += frameSize;
			}
		}
		else
		{
			samplesBuffer.Resize((samplesRead + 2 * frameSize) * format.channels);

			short	*outputBuffer = samplesBuffer + samplesRead * format.channels;

			ex_aacDecoder_DecodeFrame(handle, outputBuffer, 			      samplesBuffer.Size() -  samplesRead	       * format.channels, AACDEC_FLUSH);
			ex_aacDecoder_DecodeFrame(handle, outputBuffer + frameSize * format.channels, samplesBuffer.Size() - (samplesRead + frameSize) * format.channels, AACDEC_FLUSH);

			samplesRead += 2 * frameSize;
			finished     = True;
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
			samplesBuffer.Resize((samplesRead + 2 * frameSize) * format.channels);

			short	*outputBuffer = samplesBuffer + samplesRead * format.channels;

			ex_aacDecoder_DecodeFrame(handle, outputBuffer, 			      samplesBuffer.Size() -  samplesRead	       * format.channels, AACDEC_FLUSH);
			ex_aacDecoder_DecodeFrame(handle, outputBuffer + frameSize * format.channels, samplesBuffer.Size() - (samplesRead + frameSize) * format.channels, AACDEC_FLUSH);

			samplesRead += 2 * frameSize;
			finished     = True;
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

Int BoCA::DecoderFDKAAC::GetAudioTrack(MP4FileHandle mp4File) const
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

Bool BoCA::DecoderFDKAAC::ReadGaplessInfo(MP4FileHandle mp4File, Int &delay, Int &padding, Int64 &length) const
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
