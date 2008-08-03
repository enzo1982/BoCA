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

#include <smooth/io/drivers/driver_posix.h>

#include "mad_in.h"
#include "dllinterface.h"
#include "dxhead.h"

using namespace smooth::IO;

const String &BoCA::MADIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (maddll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>MAD MP3 Decoder</name>		\
		    <version>1.0</version>			\
		    <id>mad-in</id>				\
		    <type>decoder</type>			\
		    <format>					\
		      <name>MPEG Audio Files</name>		\
		      <extension>mp1</extension>		\
		      <extension>mp2</extension>		\
		      <extension>mp3</extension>		\
		    </format>					\
		  </component>					\
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
	mad_flow	 MADHeaderCallback(void *, const mad_header *);
	mad_flow	 MADErrorCallback(void *, mad_stream *, mad_frame *);

	/* FIXME: This is the scaling function included in the MAD
	 *	  package. It should be replaced by a more decent one.
	 */
	static inline signed int scale(mad_fixed_t sample)
	{
		/* Round
		 */
		sample += (1L << (MAD_F_FRACBITS - 16));

		/* Clip
		 */
		if	(sample >= MAD_F_ONE) sample = MAD_F_ONE - 1;
		else if (sample < -MAD_F_ONE) sample = -MAD_F_ONE;

		/* Quantize
		 */
		return sample >> (MAD_F_FRACBITS + 1 - 16);
	}
};

Bool BoCA::MADIn::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.EndsWith(".mp1") ||
	       lcURI.EndsWith(".mp2") ||
	       lcURI.EndsWith(".mp3");
}

Error BoCA::MADIn::GetStreamInfo(const String &streamURI, Track &format)
{
	Driver		*ioDriver = new DriverPOSIX(streamURI, IS_READONLY);
	InStream	*f_in = new InStream(STREAM_DRIVER, ioDriver);

	format.order	= BYTE_INTEL;
	format.bits	= 16;
	format.fileSize	= f_in->Size();
	format.length	= -1;

	infoFormat = &format;
	finished = False;

	SkipID3v2Tag(f_in);
	ReadXingTag(f_in);

	driver = ioDriver;

	readDataMutex = new Mutex();
	samplesBufferMutex = new Mutex();

	decoderThread = NonBlocking1<Bool>(&MADIn::ReadMAD, this).Call(False);

	while (decoderThread->GetStatus() == THREAD_RUNNING) Sleep(10);

	delete readDataMutex;
	delete samplesBufferMutex;

	delete f_in;
	delete ioDriver;

	if (Config::Get()->enable_id3)
	{
		format.track = -1;
		format.outfile = NIL;

		format.ParseID3Tag(streamURI);
	}

	return Success();
}

BoCA::MADIn::MADIn()
{
	packageSize = 0;

	infoFormat = NIL;
	numFrames = 0;
}

BoCA::MADIn::~MADIn()
{
}

Bool BoCA::MADIn::Activate()
{
	finished = False;

	InStream	*f_in = new InStream(STREAM_DRIVER, driver);

	SkipID3v2Tag(f_in);

	delete f_in;

	readDataMutex = new Mutex();
	samplesBufferMutex = new Mutex();

	readDataMutex->Lock();

	decoderThread = NonBlocking1<Bool>(&MADIn::ReadMAD, this).Call(True);

	return True;
}

Bool BoCA::MADIn::Deactivate()
{
	decoderThread->Stop();

	delete readDataMutex;
	delete samplesBufferMutex;

	return True;
}

Int BoCA::MADIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (decoderThread->GetStatus() != THREAD_RUNNING) return -1;

	readDataMutex->Release();

	while (decoderThread->GetStatus() == THREAD_RUNNING && samplesBuffer.Size() <= 0) Sleep(10);

	readDataMutex->Lock();

	samplesBufferMutex->Lock();

	size = samplesBuffer.Size() * (format.bits / 8);

	data.Resize(size);

	for (Int i = 0; i < samplesBuffer.Size(); i++) ((Short *) (unsigned char *) data)[i] = scale(samplesBuffer[i]);

	samplesBuffer.Resize(0);

	samplesBufferMutex->Release();

	return size;
}

Bool BoCA::MADIn::SkipID3v2Tag(InStream *in)
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

Bool BoCA::MADIn::ReadXingTag(InStream *in)
{
	/* Check for a Xing header and extract
	 * the number of samples if it exists.
	 */

	Buffer<UnsignedByte>	 buffer(156);

	/* Read data and seek back to before
	 * the Xing header.
	 */
	in->InputData(buffer, 156);
	in->RelSeek(-156);

	XHEADDATA		 data;

	if (GetXingHeader(&data, buffer))
	{
		numFrames = data.frames;

		return True;
	}

	return False;
}

Int BoCA::MADIn::ReadMAD(Bool readData)
{
	if (readData)	ex_mad_decoder_init(&decoder, this, &MADInputCallback, NIL, NIL, &MADOutputCallback, &MADErrorCallback, NIL);
	else		ex_mad_decoder_init(&decoder, this, &MADInputCallback, &MADHeaderCallback, NIL, &MADOutputCallback, &MADErrorCallback, NIL);

	ex_mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

	ex_mad_decoder_finish(&decoder);

	return Success();
}

mad_flow BoCA::MADInputCallback(void *client_data, mad_stream *stream)
{
	MADIn	*filter = (MADIn *) client_data;

	if (filter->driver->GetPos() == filter->driver->GetSize()) return MAD_FLOW_STOP;

	static Buffer<UnsignedByte>	 inputBuffer;

	filter->readDataMutex->Lock();

	Int	 bytes = Math::Min(10000, filter->driver->GetSize() - filter->driver->GetPos());
	Int	 backup = stream->bufend - stream->next_frame;

	inputBuffer.Resize(bytes + backup);

	memmove(inputBuffer, stream->next_frame, backup);

	filter->driver->ReadData(inputBuffer + backup, bytes);

	filter->readDataMutex->Release();

	ex_mad_stream_buffer(stream, inputBuffer, bytes + backup);

	filter->inBytes += bytes;

	return MAD_FLOW_CONTINUE;
}

mad_flow BoCA::MADOutputCallback(void *client_data, const mad_header *header, mad_pcm *pcm)
{
	MADIn	*filter = (MADIn *) client_data;

	filter->samplesBufferMutex->Lock();

	Int	 oSize = filter->samplesBuffer.Size();

	filter->samplesBuffer.Resize(oSize + pcm->length * filter->format.channels);

	for (Int i = 0; i < (signed) pcm->length; i++)
	{
		for (Int j = 0; j < filter->format.channels; j++)
		{
			filter->samplesBuffer[oSize + i * filter->format.channels + j] = pcm->samples[j][i];
		}
	}

	filter->samplesBufferMutex->Release();

	return MAD_FLOW_CONTINUE;
}

mad_flow BoCA::MADHeaderCallback(void *client_data, const mad_header *header)
{
	MADIn	*filter = (MADIn *) client_data;

	filter->infoFormat->channels	 = header->mode == MAD_MODE_SINGLE_CHANNEL ? 1 : 2;
	filter->infoFormat->rate	 = header->samplerate;
	filter->infoFormat->approxLength = filter->infoFormat->fileSize / (header->bitrate / 8) * filter->infoFormat->rate * filter->infoFormat->channels;

	/* If we previously found a Xing header,
	 * we can compute the exact duration from
	 * the number of frames in the file.
	 */
	if (filter->numFrames > 0)
	{
		filter->infoFormat->length = filter->numFrames * Math::Round((Float) header->duration.fraction / MAD_TIMER_RESOLUTION * filter->infoFormat->rate) * filter->infoFormat->channels;
	}

	return MAD_FLOW_STOP;
}

mad_flow BoCA::MADErrorCallback(void *client_data, mad_stream *stream, mad_frame *frame)
{
	return MAD_FLOW_CONTINUE;
}
