 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "fdkaac.h"

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
		    <name>Fraunhofer FDK AAC Decoder</name>				\
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
			      <name>MP4 Audio Files</name>				\
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
		      <name>Advanced Audio Files</name>					\
		      <extension>aac</extension>					\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>		\
		    </format>								\
		  </component>								\
											\
		");
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
	Bool	 isValidFile = False;

	if (mp4v2dll != NIL && !streamURI.ToLower().EndsWith(".aac"))
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

		if (mp4Track >= 0)
		{
			Int	 type = ex_MP4GetTrackAudioMpeg4Type(mp4File, mp4Track);

			if (type == AOT_ER_AAC_LC		||
			    type == AOT_ER_AAC_LD		||
			    type == AOT_ER_AAC_ELD		||

			    type == MP4_MPEG4_AAC_LC_AUDIO_TYPE	||
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
		InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READ);

		SkipID3v2Tag(f_in);

		isValidFile = SyncOnAACHeader(f_in);

		delete f_in;
	}

	return isValidFile;
}

Error BoCA::DecoderFDKAAC::GetStreamInfo(const String &streamURI, Track &track)
{
	Format	 format = track.GetFormat();

	if (!streamURI.ToLower().EndsWith(".aac"))
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

		if (mp4Track >= 0)
		{
			HANDLE_AACDECODER	 handle = ex_aacDecoder_Open(TT_MP4_RAW, 1);

			/* Get codec configuration.
			 */
			unsigned char	*escBuffer     = NIL;
			unsigned int	 escBufferSize = 0;

			ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (uint8_t **) &escBuffer, (uint32_t *) &escBufferSize);

			ex_aacDecoder_ConfigRaw(handle, &escBuffer, &escBufferSize);

			ex_MP4Free(escBuffer);

			/* Decode one frame to initialize decoder.
			 */
			unsigned char	*buffer	    = NIL;
			unsigned int	 bufferSize = 0;

			ex_MP4ReadSample(mp4File, mp4Track, 1, (uint8_t **) &buffer, (uint32_t *) &bufferSize, NIL, NIL, NIL, NIL);

			unsigned int	 bytesValid = bufferSize;

			ex_aacDecoder_Fill(handle, &buffer, &bufferSize, &bytesValid);

			short	*outputBuffer = new short [16384];

			if (ex_aacDecoder_DecodeFrame(handle, outputBuffer, 16384, 0) != AAC_DEC_OK)
			{
				errorState  = True;
				errorString = "Unable to decode audio data.";
			}

			delete [] outputBuffer;

			ex_MP4Free(buffer);

			/* Get sample rate and number of channels.
			 */
			if (!errorState)
			{
				CStreamInfo	*streamInfo = ex_aacDecoder_GetStreamInfo(handle);

				format.rate	= streamInfo->sampleRate;
				format.channels = streamInfo->numChannels;

				track.length	= Math::Round(ex_MP4GetTrackDuration(mp4File, mp4Track) * Float(format.rate / ex_MP4GetTrackTimeScale(mp4File, mp4Track)));

				format.bits	= 16;
			}

			ex_aacDecoder_Close(handle);

			track.SetFormat(format);

			/* Look for iTunes metadata with gapless information.
			 */
			MP4ItmfItemList	*items = ex_MP4ItmfGetItemsByMeaning(mp4File, "com.apple.iTunes", "iTunSMPB");

			if (items != NIL)
			{
				if (items->size == 1)
				{
					String			 valueString = (char *) items->elements[0].dataList.elements[0].value;
					const Array<String>	&values	     = valueString.Trim().Explode(" ");

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
		InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READ);

		format.bits	= 16;

		track.fileSize	= f_in->Size();
		track.length	= -1;

		SkipID3v2Tag(f_in);

		if (!SyncOnAACHeader(f_in))
		{
			delete f_in;

			errorState  = True;
			errorString = "No AAC file.";

			return Error();
		}

		HANDLE_AACDECODER	 handle = ex_aacDecoder_Open( adifFound ? TT_MP4_ADIF :
								     (adtsFound ? TT_MP4_ADTS : TT_MP4_LOAS), 1);

		/* Decode one frame to initialize decoder.
		 */
		unsigned int	 size = Math::Min((Int64) 32768, track.fileSize - f_in->GetPos());
		unsigned char	*data = new unsigned char [size];

		f_in->InputData((void *) data, size);

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
			errorString = "Unrecognized file format.";
		}

		/* Close handles.
		 */
		delete [] outputBuffer;
		delete [] data;

		delete f_in;

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
				if (tagger->ParseStreamInfo(streamURI, track) == Success()) foundTag = True;

				boca.DeleteComponent(tagger);
			}

			if (!foundTag)
			{
				tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v1-tag");

				if (tagger != NIL)
				{
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
	packageSize	 = 0;

	mp4File		 = NIL;
	handle		 = NIL;

	mp4Track	 = -1;
	sampleId	 = 0;

	adifFound	 = False;
	adtsFound	 = False;
	loasFound	 = False;

	frameSize	 = 0;

	delaySamples	 = 0;
	delaySamplesLeft = 0;
}

BoCA::DecoderFDKAAC::~DecoderFDKAAC()
{
}

Bool BoCA::DecoderFDKAAC::Activate()
{
	if (!track.origFilename.ToLower().EndsWith(".aac"))
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

		handle	 = ex_aacDecoder_Open(TT_MP4_RAW, 1);

		/* Get codec configuration.
		 */
		unsigned char	*escBuffer     = NIL;
		unsigned int	 escBufferSize = 0;

		ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (uint8_t **) &escBuffer, (uint32_t *) &escBufferSize);

		ex_aacDecoder_ConfigRaw(handle, &escBuffer, &escBufferSize);

		ex_MP4Free(escBuffer);

		/* Look for iTunes metadata with gapless information.
		 */
		MP4ItmfItemList	*items = ex_MP4ItmfGetItemsByMeaning(mp4File, "com.apple.iTunes", "iTunSMPB");

		if (items != NIL)
		{
			if (items->size == 1)
			{
				String			 valueString = (char *) items->elements[0].dataList.elements[0].value;
				const Array<String>	&values	     = valueString.Trim().Explode(" ");

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
		InStream	*in = new InStream(STREAM_DRIVER, driver);

		SkipID3v2Tag(in);
		SyncOnAACHeader(in);

		driver->Seek(in->GetPos());

		delete in;

 		handle = ex_aacDecoder_Open( adifFound ? TT_MP4_ADIF :
					    (adtsFound ? TT_MP4_ADTS : TT_MP4_LOAS), 1);
	}

	return True;
}

Bool BoCA::DecoderFDKAAC::Deactivate()
{
	ex_aacDecoder_Close(handle);

	if (!track.origFilename.ToLower().EndsWith(".aac"))
	{
		ex_MP4Close(mp4File, 0);

		if (String::IsUnicode(track.origFilename))
		{
			File(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in")).Delete();
		}
	}

	return True;
}

Bool BoCA::DecoderFDKAAC::Seek(Int64 samplePosition)
{
	if (!track.origFilename.ToLower().EndsWith(".aac"))
	{
		MP4Timestamp	 time = Math::Round(Float(samplePosition) / track.GetFormat().rate * ex_MP4GetTrackTimeScale(mp4File, mp4Track));

		sampleId	 = ex_MP4GetSampleIdFromTime(mp4File, mp4Track, time, true);
		delaySamplesLeft = delaySamples + time - ex_MP4GetSampleTime(mp4File, mp4Track, sampleId);

		return True;
	}

	return False;
}

Int BoCA::DecoderFDKAAC::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	static Int	 maxFrameSize = 2048;

	const Format	&format = track.GetFormat();

	Int	 samplesRead = 0;

	samplesBuffer.Resize(0);

	if (!track.origFilename.ToLower().EndsWith(".aac"))
	{
		unsigned char	*buffer	    = NIL;
		unsigned int	 bufferSize = 0;

		if (ex_MP4ReadSample(mp4File, mp4Track, sampleId++, (uint8_t **) &buffer, (uint32_t *) &bufferSize, NIL, NIL, NIL, NIL))
		{
			unsigned int	 bytesValid = bufferSize;

			while (True)
			{
				if (bytesValid > 0)
				{
					unsigned char	*inputBuffer	 = buffer + bufferSize - bytesValid;
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
					if (delaySamples == 0)
					{
						delaySamples	 = frameSize;
						delaySamplesLeft = frameSize;
					}

					/* Add FDK decoder delay.
					 */
					delaySamplesLeft += frameSize;

					/* No decoder delay for LD/ELD object types.
					 */
					if (streamInfo->aot == AOT_ER_AAC_LD || streamInfo->aot == AOT_ER_AAC_ELD) delaySamplesLeft -= frameSize;

					samplesBuffer.Resize((samplesRead + frameSize) * format.channels);
				}

				samplesRead += frameSize;
			}

			ex_MP4Free(buffer);
		}
		else
		{
			samplesBuffer.Resize((samplesRead + 2 * frameSize) * format.channels);

			short	*outputBuffer = samplesBuffer + samplesRead * format.channels;

			ex_aacDecoder_DecodeFrame(handle, outputBuffer, 			      samplesBuffer.Size() -  samplesRead	       * format.channels, AACDEC_FLUSH);
			ex_aacDecoder_DecodeFrame(handle, outputBuffer + frameSize * format.channels, samplesBuffer.Size() - (samplesRead + frameSize) * format.channels, AACDEC_FLUSH);

			samplesRead += 2 * frameSize;
		}
	}
	else
	{
		if (size <= 0) return -1;

		dataBuffer.Resize(size);

		size = driver->ReadData(dataBuffer, size);

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

				/* Set delay samples to minimum encoder delay plus decoder delay.
				 */
				delaySamples	 = frameSize * 2;
				delaySamplesLeft = frameSize * 2;

				/* No decoder delay for LD/ELD object types.
				 */
				if (streamInfo->aot == AOT_ER_AAC_LD || streamInfo->aot == AOT_ER_AAC_ELD) delaySamplesLeft -= frameSize;

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

Bool BoCA::DecoderFDKAAC::SkipID3v2Tag(InStream *in)
{
	/* Check for an ID3v2 tag at the beginning
	 * of the file and skip it if it exists.
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

Bool BoCA::DecoderFDKAAC::SyncOnAACHeader(InStream *in)
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

		adifFound = True;

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

		adtsFound = True;

		return True;
	}

	in->Seek(startPos);

	/* Try to sync on LOAS/LATM header.
	 */
	for (Int n = 0; n < 1024; n++)
	{
		if (  in->InputNumber(1)	 != 0x56) continue;
		if ( (in->InputNumber(1) & 0xE0) != 0xE0) continue;

		/* No LOAS sync found in the first 1 kB.
		 */
		if (n == 1023) break;

		in->RelSeek(-2);

		inBytes += n;

		loasFound = True;

		return True;
	}

	/* No sync; probably not an AAC file.
	 */
	return False;
}
