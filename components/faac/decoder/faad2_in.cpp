 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2008 Robert Kausch <robert.kausch@bonkenc.org>
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

/* AAC object types */
#define MAIN 1
#define LOW  2
#define SSR  3
#define LTP  4

const String &BoCA::FAAD2In::GetComponentSpecs()
{

	static String	 componentSpecs = "		\
							\
	  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
	  <component>					\
	    <name>FAAD2 MP4/AAC Decoder</name>		\
	    <version>1.0</version>			\
	    <id>faad2-in</id>				\
	    <type>decoder</type>			\
	    <format>					\
	      <name>MP4 Audio Files</name>		\
	      <extension>m4a</extension>		\
	      <extension>m4b</extension>		\
	      <extension>m4r</extension>		\
	      <extension>mp4</extension>		\
	      <extension>3gp</extension>		\
	    </format>					\
	    <format>					\
	      <name>Advanced Audio Files</name>		\
	      <extension>aac</extension>		\
	    </format>					\
	  </component>					\
							\
	";

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
	String	 lcURI = streamURI.ToLower();

	return lcURI.EndsWith(".mp4") |
	       lcURI.EndsWith(".m4a") |
	       lcURI.EndsWith(".m4b") |
	       lcURI.EndsWith(".m4r") |
	       lcURI.EndsWith(".3gp") |
	       lcURI.EndsWith(".aac");
}

Error BoCA::FAAD2In::GetStreamInfo(const String &streamURI, Track &format)
{
	if (!streamURI.ToLower().EndsWith(".aac"))
	{
		if (GetTempFile(streamURI) != streamURI)
		{
			File	 mp4File(streamURI);

			mp4File.Copy(GetTempFile(streamURI));
		}

		InStream	*f_in = new InStream(STREAM_FILE, GetTempFile(streamURI), IS_READONLY);

		format.fileSize	= f_in->Size();
		format.length	= -1;

		delete f_in;

		mp4File = ex_MP4Read(GetTempFile(streamURI), 0);

		char		*buffer		= NIL;
		unsigned short	 trackNr	= 0;
		unsigned short	 nOfTracks	= 0;

		char	*prevInFormat = String::SetInputFormat("UTF-8");

		if (ex_MP4GetMetadataName(mp4File, &buffer))						{ format.title = buffer; ex_MP4Free(buffer); }
		if (ex_MP4GetMetadataArtist(mp4File, &buffer))						{ format.artist = buffer; ex_MP4Free(buffer); }
		if (ex_MP4GetMetadataComment(mp4File, &buffer))						{ format.comment = buffer; ex_MP4Free(buffer); }
		if (ex_MP4GetMetadataYear(mp4File, &buffer))						{ format.year = String(buffer).ToInt(); ex_MP4Free(buffer); }
		if (ex_MP4GetMetadataAlbum(mp4File, &buffer))						{ format.album = buffer; ex_MP4Free(buffer); }
		if (ex_MP4GetMetadataGenre(mp4File, &buffer))						{ format.genre = buffer; ex_MP4Free(buffer); }
		if (ex_MP4GetMetadataTrack(mp4File, (u_int16_t *) &trackNr, (u_int16_t *) &nOfTracks))	{ format.track = trackNr; }

		String::SetInputFormat(prevInFormat);

		mp4Track = GetAudioTrack();

		if (mp4Track >= 0)
		{
			handle	= ex_NeAACDecOpen();
			fConfig	= ex_NeAACDecGetCurrentConfiguration(handle);

			fConfig->defSampleRate	= 44100;
			fConfig->defObjectType	= LOW;
			fConfig->outputFormat	= FAAD_FMT_16BIT;

			ex_NeAACDecSetConfiguration(handle, fConfig);

			unsigned char	*esc_buffer	= NIL;
			unsigned long	 buffer_size	= 0;

			ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (u_int8_t **) &esc_buffer, (u_int32_t *) &buffer_size);

			ex_NeAACDecInit2(handle, (unsigned char *) esc_buffer, buffer_size,
 (unsigned long *) &format.rate, (unsigned char *) &format.channels);

			format.length	= Math::Round(double(signed(ex_MP4GetTrackDuration(mp4File, mp4Track))) * format.channels * format.rate / double(signed(ex_MP4GetTrackTimeScale(mp4File, mp4Track))));
			format.order	= BYTE_INTEL;
			format.bits	= 16;

			ex_MP4Free(esc_buffer);

			ex_NeAACDecClose(handle);
		}

		ex_MP4Close(mp4File);

		if (GetTempFile(streamURI) != streamURI)
		{
			File	 tempFile(GetTempFile(streamURI));

			tempFile.Delete();
		}
	}
	else
	{
		handle	= ex_NeAACDecOpen();
		fConfig	= ex_NeAACDecGetCurrentConfiguration(handle);

		fConfig->defSampleRate	= 44100;
		fConfig->defObjectType	= LOW;
		fConfig->outputFormat	= FAAD_FMT_16BIT;

		ex_NeAACDecSetConfiguration(handle, fConfig);

		InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READONLY);

		format.order	= BYTE_INTEL;
		format.bits	= 16;
		format.fileSize	= f_in->Size();
		format.length	= -1;

		Int		 size = Math::Min(32768, format.fileSize);
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

		if (samplesRead > 0 && samplesBytes > 0) format.approxLength = samplesRead * (format.fileSize / samplesBytes);

		delete [] data;

		delete f_in;

		ex_NeAACDecClose(handle);

		if (errorState) return Error();

		if (Config::Get()->enable_id3)
		{
			format.track = -1;
			format.outfile = NIL;

			format.ParseID3V2Tag(streamURI); 
		}
	}

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
	if (!format.origFilename.ToLower().EndsWith(".aac"))
	{
		if (GetTempFile(format.origFilename) != format.origFilename)
		{
			File	 mp4File(format.origFilename);

			mp4File.Copy(GetTempFile(format.origFilename));
		}

		mp4File		= ex_MP4Read(GetTempFile(format.origFilename), 0);
		mp4Track	= GetAudioTrack();

		if (mp4Track == -1) return False;
	}

	handle	= ex_NeAACDecOpen();
	fConfig	= ex_NeAACDecGetCurrentConfiguration(handle);

	fConfig->defSampleRate	= 44100;
	fConfig->defObjectType	= LOW;
	fConfig->outputFormat	= FAAD_FMT_16BIT;

	ex_NeAACDecSetConfiguration(handle, fConfig);

	if (!format.origFilename.ToLower().EndsWith(".aac"))
	{
		unsigned char	*buffer		= NIL;
		unsigned long	 buffer_size	= 0;

		ex_MP4GetTrackESConfiguration(mp4File, mp4Track, (u_int8_t **) &buffer, (u_int32_t *) &buffer_size);

		unsigned long	 rate;
		unsigned char	 channels;

		ex_NeAACDecInit2(handle, (unsigned char *) buffer, buffer_size, &rate, &channels);

		sampleId = 0;

		ex_MP4Free(buffer);
	}
	else
	{
		Int		 size = 4096;
		unsigned char	*data = new unsigned char [size];

		driver->ReadData(data, size);

		unsigned long	 rate;
		unsigned char	 channels;

		ex_NeAACDecInit(handle, data, size, &rate, &channels);

		delete [] data;

		driver->Seek(0);
	}

	return True;
}

Bool BoCA::FAAD2In::Deactivate()
{
	ex_NeAACDecClose(handle);

	if (!format.origFilename.ToLower().EndsWith(".aac"))
	{
		ex_MP4Close(mp4File);

		if (GetTempFile(format.origFilename) != format.origFilename)
		{
			File	 tempFile(GetTempFile(format.origFilename));

			tempFile.Delete();
		}
	}

	return True;
}

Int BoCA::FAAD2In::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (size <= 0) return -1;

	inBytes += size;

	Void	*samples = NIL;
	Int	 samplesRead = 0;

	samplesBuffer.Resize(0);

	if (!format.origFilename.ToLower().EndsWith(".aac"))
	{
		do
		{
			unsigned char	*buffer		= NIL;
			unsigned long	 buffer_size	= 0;

			ex_MP4ReadSample(mp4File, mp4Track, sampleId++, (u_int8_t **) &buffer, (u_int32_t *) &buffer_size, NIL, NIL, NIL, NIL);

			NeAACDecFrameInfo frameInfo;

			samples = ex_NeAACDecDecode(handle, &frameInfo, buffer, buffer_size);

			ex_MP4Free(buffer);

	        	if ((frameInfo.error == 0) && (frameInfo.samples > 0) && (samples != NIL))
			{
				backBuffer.Resize(samplesRead * 2);

				memcpy(backBuffer, samplesBuffer, samplesRead * 2);

				samplesBuffer.Resize((samplesRead + frameInfo.samples) * 2);

				memcpy(samplesBuffer, backBuffer, samplesRead * 2);
				memcpy(samplesBuffer + samplesRead * 2, samples, frameInfo.samples * 2);

				samplesRead += frameInfo.samples;
			}
		}
		while (samples != NIL && sampleId < ((format.length / 2048) * (double(inBytes) / format.fileSize)));
	}
	else
	{
		dataBuffer.Resize(size + backBuffer.Size());

		driver->ReadData((unsigned char *) dataBuffer + backBuffer.Size(), size);

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
				unsigned char	*buffer = new unsigned char [samplesRead * 2];

				memcpy(buffer, samplesBuffer, samplesRead * 2);

				samplesBuffer.Resize((samplesRead + frameInfo.samples) * 2);

				memcpy(samplesBuffer, buffer, samplesRead * 2);
				memcpy(samplesBuffer + samplesRead * 2, samples, frameInfo.samples * 2);

				delete [] buffer;

				samplesRead += frameInfo.samples;
			}

			bytesConsumed += frameInfo.bytesconsumed;

			if ((size - bytesConsumed < bytesConsumed) && (inBytes % 6144) == 0) samples = NIL;
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

String BoCA::FAAD2In::GetTempFile(const String &oFileName)
{
	String	 rVal	= oFileName;
	Int	 lastBs	= -1;

	for (Int i = 0; i < rVal.Length(); i++)
	{
		if (rVal[i] > 255)	rVal[i] = '#';
		if (rVal[i] == '\\')	lastBs = i;
	}

	if (rVal == oFileName) return rVal;

	String	 tempDir = S::System::System::GetTempDirectory();

	for (Int j = lastBs + 1; j < rVal.Length(); j++)
	{
		tempDir[tempDir.Length()] = rVal[j];
	}

	return tempDir.Append(".in.temp");
}
