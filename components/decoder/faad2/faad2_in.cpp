 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2011 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "faad2_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::FAAD2In::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (faad2dll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>FAAD2 MP4/AAC Decoder</name>					\
		    <version>1.0</version>						\
		    <id>faad2-in</id>							\
		    <type>decoder</type>						\
											\
		";

		if (mp4v2dll != NIL)
		{
			componentSpecs.Append("						\
											\
			    <format>							\
			      <name>MPEG-4 Audio Files</name>				\
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
	LoadFAAD2DLL();
	LoadMP4v2DLL();
}

Void smooth::DetachDLL()
{
	FreeFAAD2DLL();
	FreeMP4v2DLL();
}

Bool BoCA::FAAD2In::CanOpenStream(const String &streamURI)
{
	Bool	 isValidFile = False;

	if (mp4v2dll != NIL && !streamURI.ToLower().EndsWith(".aac"))
	{
		if (String::IsUnicode(streamURI))
		{
			File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));

			mp4File = ex_MP4Read(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"), 0);
		}
		else
		{
			mp4File = ex_MP4Read(streamURI, 0);
		}

		mp4Track = GetAudioTrack();

		if (mp4Track >= 0)
		{
			Int	 type = ex_MP4GetTrackAudioMpeg4Type(mp4File, mp4Track);

			if (type == MP4_MPEG4_AAC_MAIN_AUDIO_TYPE ||
			    type == MP4_MPEG4_AAC_LC_AUDIO_TYPE	  ||
			    type == MP4_MPEG4_AAC_SSR_AUDIO_TYPE  ||
			    type == MP4_MPEG4_AAC_LTP_AUDIO_TYPE  ||
			    type == MP4_MPEG4_AAC_HE_AUDIO_TYPE	  ||
			    type == MP4_MPEG4_AAC_SCALABLE_AUDIO_TYPE) isValidFile = True;
		}

		ex_MP4Close(mp4File);

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

	return isValidFile;;
}

Error BoCA::FAAD2In::GetStreamInfo(const String &streamURI, Track &track)
{
	Format	 format = track.GetFormat();

	if (!streamURI.ToLower().EndsWith(".aac"))
	{
		track.fileSize	= File(streamURI).GetFileSize();
		track.length	= -1;

		if (String::IsUnicode(streamURI))
		{
			File(streamURI).Copy(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"));

			mp4File = ex_MP4Read(Utilities::GetNonUnicodeTempFileName(streamURI).Append(".in"), 0);
		}
		else
		{
			mp4File = ex_MP4Read(streamURI, 0);
		}

		mp4Track = GetAudioTrack();

		if (mp4Track >= 0)
		{
			handle	= ex_NeAACDecOpen();
			fConfig	= ex_NeAACDecGetCurrentConfiguration(handle);

			fConfig->defSampleRate	= 44100;
			fConfig->defObjectType	= LC;
			fConfig->outputFormat	= FAAD_FMT_16BIT;

			ex_NeAACDecSetConfiguration(handle, fConfig);

			unsigned char	*esc_buffer	= NIL;
			unsigned long	 buffer_size	= 0;

			ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (uint8_t **) &esc_buffer, (uint32_t *) &buffer_size);

			ex_NeAACDecInit2(handle, (unsigned char *) esc_buffer, buffer_size, (unsigned long *) &format.rate, (unsigned char *) &format.channels);

			track.length	= Math::Round(ex_MP4GetTrackDuration(mp4File, mp4Track) * format.rate / ex_MP4GetTrackTimeScale(mp4File, mp4Track));

			format.order	= BYTE_INTEL;
			format.bits	= 16;

			ex_MP4Free(esc_buffer);

			ex_NeAACDecClose(handle);

			track.SetFormat(format);
		}

		ex_MP4Close(mp4File);

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

		format.order	= BYTE_INTEL;
		format.bits	= 16;

		track.fileSize	= f_in->Size();
		track.length	= -1;

		SkipID3v2Tag(f_in);

		if (!SyncOnAACHeader(f_in))
		{
			delete f_in;

			errorState = True;
			errorString = "No AAC file.";

			return Error();
		}

		handle	= ex_NeAACDecOpen();
		fConfig	= ex_NeAACDecGetCurrentConfiguration(handle);

		fConfig->defSampleRate	= 44100;
		fConfig->defObjectType	= LC;
		fConfig->outputFormat	= FAAD_FMT_16BIT;

		ex_NeAACDecSetConfiguration(handle, fConfig);

		Int		 size = Math::Min((Int64) 32768, track.fileSize);
		unsigned char	*data = new unsigned char [size];

		f_in->InputData((void *) data, size);

		ex_NeAACDecInit(handle, data, size, (unsigned long *) &format.rate, (unsigned char *) &format.channels);

		Void		*samples = NIL;
		Int		 bytesConsumed = 0;
		Int		 samplesRead = 0;
		Int		 samplesBytes = 0;

		do
		{
			NeAACDecFrameInfo	 frameInfo;

			samples = ex_NeAACDecDecode(handle, &frameInfo, data + bytesConsumed, size - bytesConsumed);

			if (frameInfo.error)
			{
				errorState = True;
				errorString = ex_NeAACDecGetErrorMessage(frameInfo.error);

				break;
			}

			bytesConsumed += frameInfo.bytesconsumed;

			if (bytesConsumed >= 8192)
			{
				samplesRead += frameInfo.samples;
				samplesBytes += frameInfo.bytesconsumed;
			}

			if (size - bytesConsumed < bytesConsumed) samples = NIL;
		}
		while (samples != NIL);

		if (samplesRead > 0 && samplesBytes > 0) track.approxLength = samplesRead / format.channels * (track.fileSize / samplesBytes);

		delete [] data;

		delete f_in;

		ex_NeAACDecClose(handle);

		if (errorState) return Error();

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			tagger->ParseStreamInfo(streamURI, track);

			boca.DeleteComponent(tagger);
		}
	}

	track.SetFormat(format);

	return Success();
}

BoCA::FAAD2In::FAAD2In()
{
	packageSize = 0;
}

BoCA::FAAD2In::~FAAD2In()
{
}

Bool BoCA::FAAD2In::Activate()
{
	if (!track.origFilename.ToLower().EndsWith(".aac"))
	{
		if (String::IsUnicode(track.origFilename))
		{
			File(track.origFilename).Copy(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in"));

			mp4File	= ex_MP4Read(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in"), 0);
		}
		else
		{
			mp4File	= ex_MP4Read(track.origFilename, 0);
		}

		mp4Track	= GetAudioTrack();

		if (mp4Track == -1) return False;
	}
	else
	{
		InStream	*in = new InStream(STREAM_DRIVER, driver);

		SkipID3v2Tag(in);
		SyncOnAACHeader(in);

		driver->Seek(in->GetPos());

		delete in;
	}

	handle	= ex_NeAACDecOpen();
	fConfig	= ex_NeAACDecGetCurrentConfiguration(handle);

	fConfig->defSampleRate	= 44100;
	fConfig->defObjectType	= LC;
	fConfig->outputFormat	= FAAD_FMT_16BIT;

	ex_NeAACDecSetConfiguration(handle, fConfig);

	if (!track.origFilename.ToLower().EndsWith(".aac"))
	{
		unsigned char	*buffer		= NIL;
		unsigned long	 buffer_size	= 0;

		ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (uint8_t **) &buffer, (uint32_t *) &buffer_size);

		unsigned long	 rate;
		unsigned char	 channels;

		ex_NeAACDecInit2(handle, (unsigned char *) buffer, buffer_size, &rate, &channels);

		sampleId = 1;

		ex_MP4Free(buffer);
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

Bool BoCA::FAAD2In::Deactivate()
{
	ex_NeAACDecClose(handle);

	if (!track.origFilename.ToLower().EndsWith(".aac"))
	{
		ex_MP4Close(mp4File);

		if (String::IsUnicode(track.origFilename))
		{
			File(Utilities::GetNonUnicodeTempFileName(track.origFilename).Append(".in")).Delete();
		}
	}

	return True;
}

Bool BoCA::FAAD2In::Seek(Int64 samplePosition)
{
	if (!track.origFilename.ToLower().EndsWith(".aac"))
	{
		/* ToDo: Seeking by sample ID is not exact!
		 */
		MP4Timestamp	 time = Float(samplePosition / track.GetFormat().rate) * ex_MP4GetTrackTimeScale(mp4File, mp4Track);

		sampleId = ex_MP4GetSampleIdFromTime(mp4File, mp4Track, time, true);

		return True;
	}

	return False;
}

Int BoCA::FAAD2In::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (size <= 0) return -1;

	inBytes += size;

	Void	*samples = NIL;
	Int	 samplesRead = 0;

	samplesBuffer.Resize(0);

	if (!track.origFilename.ToLower().EndsWith(".aac"))
	{
		do
		{
			unsigned char	*buffer		= NIL;
			unsigned long	 buffer_size	= 0;

			ex_MP4ReadSample(mp4File, mp4Track, sampleId++, (uint8_t **) &buffer, (uint32_t *) &buffer_size, NIL, NIL, NIL, NIL);

			NeAACDecFrameInfo frameInfo;

			samples = ex_NeAACDecDecode(handle, &frameInfo, buffer, buffer_size);

			ex_MP4Free(buffer);

	        	if ((frameInfo.error == 0) && (frameInfo.samples > 0) && (samples != NIL))
			{
				samplesBuffer.Resize((samplesRead + frameInfo.samples) * 2);

				memcpy(samplesBuffer + samplesRead * 2, samples, frameInfo.samples * 2);

				samplesRead += frameInfo.samples;
			}
		}
		while (samples != NIL && samplesRead < (track.length * track.GetFormat().channels * (2 * Float(size) / track.fileSize)));
	}
	else
	{
		dataBuffer.Resize(size + backBuffer.Size());

		size = driver->ReadData((unsigned char *) dataBuffer + backBuffer.Size(), size);

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

			samples = ex_NeAACDecDecode(handle, &frameInfo, (unsigned char *) dataBuffer + bytesConsumed, size - bytesConsumed);

		        if ((frameInfo.error == 0) && (frameInfo.samples > 0) && (samples != NIL))
			{
				samplesBuffer.Resize((samplesRead + frameInfo.samples) * 2);

				memcpy(samplesBuffer + samplesRead * 2, samples, frameInfo.samples * 2);

				samplesRead += frameInfo.samples;
			}

			bytesConsumed += frameInfo.bytesconsumed;

			if ((size - bytesConsumed < bytesConsumed) && (driver->GetPos() < driver->GetSize())) samples = NIL;
		}
		while (samples != NIL);

		if ((size - bytesConsumed) > 0)
		{
			backBuffer.Resize(size - bytesConsumed);

			memcpy(backBuffer, (unsigned char *) dataBuffer + bytesConsumed, backBuffer.Size());
		}
	}

	if (samplesRead > 0)
	{
		data.Resize(samplesRead * 2);

		memcpy((unsigned char *) data, samplesBuffer, samplesRead * 2);
	}

	return samplesRead * 2;
}

Int BoCA::FAAD2In::GetAudioTrack()
{
	Int nOfTracks = ex_MP4GetNumberOfTracks(mp4File, NIL, 0);

	for (Int i = 0; i < nOfTracks; i++)
	{
		MP4TrackId	 trackId	= ex_MP4FindTrackId(mp4File, i, NIL, 0);
		String		 trackType	= ex_MP4GetTrackType(mp4File, trackId);

		if (trackType == MP4_AUDIO_TRACK_TYPE) return trackId;
	}

	return -1;
}

Bool BoCA::FAAD2In::SkipID3v2Tag(InStream *in)
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

Bool BoCA::FAAD2In::SyncOnAACHeader(InStream *in)
{
	Int	 startPos = in->GetPos();

	/* Try to sync on ADIF header
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

	/* Try to sync on ADTS header
	 */
	for (Int n = 0; n < 1024; n++)
	{
		if (  in->InputNumber(1)	       != 0xFF) continue;
		if ( (in->InputNumber(1) & 0xF6)       != 0xF0) continue;
		if (((in->InputNumber(1) & 0x3C) >> 2) >=   12) continue;

		/* No ADTS sync found in the first 1 kB;
		 * probably not an AAC file.
		 */
		if (n == 1023) break;

		in->RelSeek(-3);

		inBytes += n;

		return True;
	}

	return False;
}
