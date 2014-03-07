 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <smooth/io/drivers/driver_posix.h>

#include "mad.h"
#include "config.h"
#include "xing/dxhead.h"

using namespace smooth::IO;

const String &BoCA::DecoderMAD::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (maddll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>MAD MP3 Decoder</name>				\
		    <version>1.0</version>					\
		    <id>mad-dec</id>						\
		    <type>decoder</type>					\
		    <replace>lame-dec</replace>					\
		    <format>							\
		      <name>MPEG Audio Files</name>				\
		      <extension>mp1</extension>				\
		      <extension>mp2</extension>				\
		      <extension>mp3</extension>				\
		      <tag id=\"id3v2-tag\" mode=\"prepend\">ID3v2</tag>	\
		    </format>							\
		  </component>							\
										\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadMADDLL();
}

Void smooth::DetachDLL()
{
	FreeMADDLL();
}

namespace BoCA
{
	mad_flow	 MADInputCallback(void *, mad_stream *);
	mad_flow	 MADOutputCallback(void *, const mad_header *, mad_pcm *);
	mad_flow	 MADHeaderCallback(void *, const mad_header *, mad_pcm *);
	mad_flow	 MADErrorCallback(void *, mad_stream *, mad_frame *);

	/* FIXME: This is the scaling function included in the MAD
	 *	  package. It should be replaced by a more decent one.
	 */
	static inline signed int scale(mad_fixed_t sample, int scale)
	{
		/* Round
		 */
		sample += (1L << (MAD_F_FRACBITS - scale));

		/* Clip
		 */
		if	(sample >= MAD_F_ONE) sample =	MAD_F_ONE - 1;
		else if (sample < -MAD_F_ONE) sample = -MAD_F_ONE;

		/* Quantize
		 */
		return sample >> (MAD_F_FRACBITS + 1 - scale);
	}
};

Bool BoCA::DecoderMAD::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.EndsWith(".mp1") ||
	       lcURI.EndsWith(".mp2") ||
	       lcURI.EndsWith(".mp3");
}

Error BoCA::DecoderMAD::GetStreamInfo(const String &streamURI, Track &track)
{
	Driver		*ioDriver = new DriverPOSIX(streamURI, IS_READ);
	InStream	*f_in = new InStream(STREAM_DRIVER, ioDriver);

	track.fileSize	= f_in->Size();
	track.length	= -1;

	infoTrack = &track;
	stop	  = False;
	finished  = False;

	SkipID3v2Tag(f_in);
	ParseVBRHeaders(f_in);

	driver = ioDriver;
	driver->Seek(f_in->GetPos());

	readDataMutex = new Mutex();
	samplesBufferMutex = new Mutex();

	ReadMAD(False);

	delete readDataMutex;
	delete samplesBufferMutex;

	delete f_in;
	delete ioDriver;

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

	return Success();
}

BoCA::DecoderMAD::DecoderMAD()
{
	configLayer	   = NIL;

	packageSize	   = 0;

	readDataMutex	   = NIL;
	samplesBufferMutex = NIL;

	decoderThread	   = NIL;

	infoTrack	   = NIL;

	stop		   = False;
	finished	   = False;

	numFrames	   = 0;

	delaySamples	   = 0;
	padSamples	   = 0;

	/* Initialize to decoder delay.
	 */
	delaySamplesLeft   = 529;
}

BoCA::DecoderMAD::~DecoderMAD()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DecoderMAD::Activate()
{
	stop	 = False;
	finished = False;

	InStream	*f_in = new InStream(STREAM_DRIVER, driver);

	SkipID3v2Tag(f_in);
	ParseVBRHeaders(f_in);

	driver->Seek(f_in->GetPos());

	delete f_in;

	readDataMutex = new Mutex();
	samplesBufferMutex = new Mutex();

	readDataMutex->Lock();

	decoderThread = NIL;

	return True;
}

Bool BoCA::DecoderMAD::Deactivate()
{
	if (decoderThread != NIL)
	{
		stop = True;

		readDataMutex->Release();

		decoderThread->Wait();
	}

	delete readDataMutex;
	delete samplesBufferMutex;

	return True;
}

Int BoCA::DecoderMAD::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (decoderThread == NIL) decoderThread = NonBlocking1<Bool>(&DecoderMAD::ReadMAD, this).Call(True);

	if (decoderThread->GetStatus() != THREAD_RUNNING && samplesBuffer.Size() <= 0) return -1;

	const Format	&format = track.GetFormat();

	readDataMutex->Release();

	while (decoderThread->GetStatus() == THREAD_RUNNING && samplesBuffer.Size() <= 0) S::System::System::Sleep(0);

	readDataMutex->Lock();

	samplesBufferMutex->Lock();

	size = samplesBuffer.Size() * (format.bits / 8);

	data.Resize(size);

	for (Int i = 0; i < samplesBuffer.Size(); i++)
	{
		int	 sample = scale(samplesBuffer[i], format.bits);

		if	(format.bits == 16				) ((Short *) (unsigned char *) data)[i] = sample;

		else if (format.bits == 24 && endianness == EndianLittle) { data[i * 3 + 2] = (sample >> 16) & 0xFF; data[i * 3 + 1] = (sample >> 8) & 0xFF; data[i * 3    ] = sample & 0xFF; }
		else if (format.bits == 24 && endianness == EndianBig	) { data[i * 3    ] = (sample >> 16) & 0xFF; data[i * 3 + 1] = (sample >> 8) & 0xFF; data[i * 3 + 2] = sample & 0xFF; }
	}

	samplesBuffer.Resize(0);

	samplesBufferMutex->Release();

	return size;
}

Bool BoCA::DecoderMAD::SkipID3v2Tag(InStream *in)
{
	/* Check for an ID3v2 tag at the beginning of the
	 * file and skip it if it exists as it might cause
	 * problems if the tag is unsynchronized.
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

Bool BoCA::DecoderMAD::ParseVBRHeaders(InStream *in)
{
	/* Check for a Xing header and extract
	 * the number of samples if it exists.
	 */
	Buffer<UnsignedByte>	 buffer(228);

	/* Read data and seek back to before
	 * the Xing header.
	 */
	in->InputData(buffer, 156);

	XHEADDATA		 data;

	data.toc = NIL;

	if (GetXingHeader(&data, buffer))
	{
		numFrames = data.frames;

		in->InputData(buffer, 228);

		if (buffer[0] == 'L' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'E')
		{
			delaySamples = ( buffer[21]	    << 4) | ((buffer[22] & 0xF0) >> 4);
			padSamples   = ((buffer[22] & 0x0F) << 8) | ( buffer[23]	     );

			delaySamplesLeft += delaySamples;
		}

		return True;
	}
	else if (buffer[36] == 'V' && buffer[37] == 'B' && buffer[38] == 'R' && buffer[39] == 'I')
	{
		numFrames    = ((buffer[50] << 24) | (buffer[51] << 16) | (buffer[52] << 8) | (buffer[53])) - 1;

		delaySamples = 576;
		padSamples   = ((buffer[42] << 8) | (buffer[43])) - delaySamples;

		delaySamplesLeft += delaySamples;

		in->RelSeek(228);

		return True;
	}

	in->RelSeek(-156);

	return False;
}

Int BoCA::DecoderMAD::ReadMAD(Bool readData)
{
	mad_decoder	 decoder;

	if (readData)	ex_mad_decoder_init(&decoder, this, &MADInputCallback, NIL, NIL, &MADOutputCallback, &MADErrorCallback, NIL);
	else		ex_mad_decoder_init(&decoder, this, &MADInputCallback, NIL, NIL, &MADHeaderCallback, &MADErrorCallback, NIL);

	ex_mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

	ex_mad_decoder_finish(&decoder);

	return Success();
}

ConfigLayer *BoCA::DecoderMAD::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMAD();

	return configLayer;
}

mad_flow BoCA::MADInputCallback(void *client_data, mad_stream *stream)
{
	DecoderMAD	*filter = (DecoderMAD *) client_data;

	if (filter->stop || filter->finished) return MAD_FLOW_STOP;

	filter->readDataMutex->Lock();

	/* Check if we have any more data. If not, append an empty
	 * frame to the last frame to allow the decoder to finish.
	 */
	if (filter->driver->GetPos() == filter->driver->GetSize()) filter->finished = True;

	Int	 bytes = Math::Min((Int64) 131072, filter->finished ? 1440 : filter->driver->GetSize() - filter->driver->GetPos());
	Int	 backup = stream->bufend - stream->next_frame;

	filter->inputBuffer.Resize(bytes + backup);

	if (filter->finished) filter->inputBuffer.Zero();

	memmove(filter->inputBuffer, stream->next_frame, backup);

	if (!filter->finished) filter->driver->ReadData(filter->inputBuffer + backup, bytes);

	filter->readDataMutex->Release();

	ex_mad_stream_buffer(stream, filter->inputBuffer, bytes + backup);

	filter->inBytes += bytes;

	return MAD_FLOW_CONTINUE;
}

mad_flow BoCA::MADOutputCallback(void *client_data, const mad_header *header, mad_pcm *pcm)
{
	DecoderMAD	*filter = (DecoderMAD *) client_data;

	filter->samplesBufferMutex->Lock();

	Int	 oSize = filter->samplesBuffer.Size();
	Int	 channels = header->mode == MAD_MODE_SINGLE_CHANNEL ? 1 : 2;

	if (pcm->length > filter->delaySamplesLeft)
	{
		filter->samplesBuffer.Resize(oSize + (pcm->length - filter->delaySamplesLeft) * channels);

		for (Int i = filter->delaySamplesLeft; i < (signed) pcm->length; i++)
		{
			for (Int j = 0; j < channels; j++)
			{
				filter->samplesBuffer[oSize + (i - filter->delaySamplesLeft) * channels + j] = pcm->samples[j][i];
			}
		}
	}

	filter->delaySamplesLeft = Math::Max(0, filter->delaySamplesLeft - pcm->length);

	filter->samplesBufferMutex->Release();

	return MAD_FLOW_CONTINUE;
}

mad_flow BoCA::MADHeaderCallback(void *client_data, const mad_header *header, mad_pcm *pcm)
{
	DecoderMAD	*filter = (DecoderMAD *) client_data;

	Config	*config = Config::Get();
	Format	 format = filter->infoTrack->GetFormat();

	format.bits	= config->GetIntValue("MAD", "Enable24Bit", False) ? 24 : 16;
	format.channels	= header->mode == MAD_MODE_SINGLE_CHANNEL ? 1 : 2;
	format.rate	= header->samplerate;

	filter->infoTrack->approxLength = filter->infoTrack->fileSize / (header->bitrate / 8) * format.rate;

	filter->infoTrack->SetFormat(format);

	/* If we previously found a Xing header,
	 * we can compute the exact duration from
	 * the number of frames in the file.
	 */
	if (filter->numFrames > 0)
	{
		filter->infoTrack->length = filter->numFrames * Math::Round((Float) header->duration.fraction / MAD_TIMER_RESOLUTION * format.rate);

		filter->infoTrack->length -= (filter->delaySamples + filter->padSamples);
	}

	return MAD_FLOW_STOP;
}

mad_flow BoCA::MADErrorCallback(void *client_data, mad_stream *stream, mad_frame *frame)
{
	return MAD_FLOW_CONTINUE;
}
