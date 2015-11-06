 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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
		      <tag id=\"id3v1-tag\" mode=\"append\">ID3v1</tag>		\
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

	offset = f_in->GetPos();
	driver = ioDriver;
	driver->Seek(offset);

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

	offset		   = 0;

	numBytes	   = 0;
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

Int BoCA::DecoderMAD::ReadData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (decoderThread == NIL) decoderThread = NonBlocking1<Bool>(&DecoderMAD::ReadMAD, this).Call(True);

	if (decoderThread->GetStatus() != THREAD_RUNNING && samplesBuffer.Size() <= 0) return -1;

	const Format	&format = track.GetFormat();

	readDataMutex->Release();

	while (decoderThread->GetStatus() == THREAD_RUNNING && samplesBuffer.Size() <= 0) S::System::System::Sleep(0);

	readDataMutex->Lock();

	samplesBufferMutex->Lock();

	Int	 size = samplesBuffer.Size() * (format.bits / 8);

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
	/* Read MPEG header and get frame size.
	 */
	Buffer<UnsignedByte>	 header(3);

	in->InputData(header, 3);
	in->RelSeek(-3);

	Int	 frameSize = GetMPEGFrameSize(header);

	if (frameSize < 156) return False;

	/* Read frame.
	 */
	Buffer<UnsignedByte>	 buffer(frameSize);

	in->InputData(buffer, frameSize);

	/* Check for a Xing or VBRI header and extract the number of frames.
	 */
	XHEADDATA		 data;

	data.toc = NIL;

	if (GetXingHeader(&data, buffer))
	{
		numBytes  = data.bytes - frameSize;
		numFrames = data.frames;

		/* Check for a LAME header and extract length information.
		 */
		if (frameSize >= 192 && buffer[0x9C] == 'L' && buffer[0x9D] == 'A' && buffer[0x9E] == 'M' && buffer[0x9F] == 'E')
		{
			delaySamples = ( buffer[0xB1]	      << 4) | ((buffer[0xB2] & 0xF0) >> 4);
			padSamples   = ((buffer[0xB2] & 0x0F) << 8) | ( buffer[0xB3]		 );

			delaySamplesLeft += delaySamples;
		}
	}
	else if (buffer[0x24] == 'V' && buffer[0x25] == 'B' && buffer[0x26] == 'R' && buffer[0x27] == 'I')
	{
		numBytes     = ((buffer[0x2E] << 24) | (buffer[0x2F] << 16) | (buffer[0x30] << 8) | (buffer[0x31])) - frameSize;
		numFrames    = ((buffer[0x32] << 24) | (buffer[0x33] << 16) | (buffer[0x34] << 8) | (buffer[0x35])) - 1;

		delaySamples = 576;
		padSamples   = ((buffer[0x2A] << 8) | (buffer[0x2B])) - delaySamples;

		delaySamplesLeft += delaySamples;
	}
	else
	{
		/* Seek back to before the frame if no Xing or VBRI header was found.
		 */
		in->RelSeek(-frameSize);

		return False;
	}

	/* Sanity check for header vs. actual file size (account
	 * for possible ID3v1 and addtional 4kB of other tags).
	 */
	if (numBytes > 0 && in->Size() - in->GetPos() > numBytes * 1.01 + 128 + 4096)
	{
		numFrames  = 0;

		padSamples = 0;
	}

	return True;
}

Int BoCA::DecoderMAD::GetMPEGFrameSize(const Buffer<UnsignedByte> &header)
{
	/* MPEG bitrate table - [version][layer][bitrate]
	 */
	const UnsignedInt16	 mpegBitrate[4][4][16] = {
		{ // Version 2.5
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Reserved
			{ 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 }, // Layer 3
			{ 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 }, // Layer 2
			{ 0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, 0 }  // Layer 1
		},
		{ // Reserved
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Invalid
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Invalid
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Invalid
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }  // Invalid
		},
		{ // Version 2
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Reserved
			{ 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 }, // Layer 3
			{ 0,   8,  16,  24,  32,  40,  48,  56,  64,  80,  96, 112, 128, 144, 160, 0 }, // Layer 2
			{ 0,  32,  48,  56,  64,  80,  96, 112, 128, 144, 160, 176, 192, 224, 256, 0 }  // Layer 1
		},
		{ // Version 1
			{ 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 0 }, // Reserved
			{ 0,  32,  40,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 0 }, // Layer 3
			{ 0,  32,  48,  56,  64,  80,  96, 112, 128, 160, 192, 224, 256, 320, 384, 0 }, // Layer 2
			{ 0,  32,  64,  96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0 }, // Layer 1
		}
	};

	/* MPEG sample rate table - [version][srate]
	 */
	const UnsignedInt16	 mpegSampleRate[4][4] = {
		{ 11025, 12000,  8000, 0 }, // MPEG 2.5
		{     0,     0,     0, 0 }, // Reserved
		{ 22050, 24000, 16000, 0 }, // MPEG 2
		{ 44100, 48000, 32000, 0 }  // MPEG 1
	};

	/* MPEG samples per frame table - [version][layer]
	 */
	const UnsignedInt16	 mpegFrameSamples[4][4] = {
		{    0,  576, 1152,  384 }, // MPEG 2.5
		{    0,    0,    0,    0 }, // Reserved
		{    0,  576, 1152,  384 }, // MPEG 2
		{    0, 1152, 1152,  384 }  // MPEG 1
	};

	/* MPEG unit size - [layer]
	 */
	const UnsignedInt8	 mpegUnitSize[4] = { 0, 1, 1, 4 }; // Reserved, 3, 2, 1

	/* Check header validity.
	 */
	if ((header[0] & 0xFF) != 0xFF ||	  // 8 synchronization bits
	    (header[1] & 0xE0) != 0xE0 ||	  // 3 synchronization bits
	    (header[1] & 0x18) == 0x08 ||	  // reserved version
	    (header[1] & 0x06) == 0x00 ||	  // reserved layer
	    (header[2] & 0xF0) == 0xF0) return 0; // reserved bitrate

	/* Get header values.
	 */
	Int	 version    = (header[1] & 0x18) >> 3; // version
	Int	 layer	    = (header[1] & 0x06) >> 1; // layer
	Int	 padding    = (header[2] & 0x02) >> 1; // padding
	Int	 bitrate    = (header[2] & 0xf0) >> 4; // bitrate
	Int	 sampleRate = (header[2] & 0x0c) >> 2; // samplerate
    
	/* Return frame size.
	 */
	return (((Float) mpegFrameSamples[version][layer] / 8.0 * (Float) (mpegBitrate[version][layer][bitrate] * 1000)) / (Float) mpegSampleRate[version][sampleRate]) + ((padding) ? mpegUnitSize[layer] : 0);
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

	memmove(filter->inputBuffer, stream->next_frame, backup);

	filter->inputBuffer.Resize(bytes + backup);

	if (!filter->finished) filter->driver->ReadData(filter->inputBuffer + backup, bytes);
	else		       memset(filter->inputBuffer + backup, 0, bytes);

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
	const Config	*config = filter->GetConfiguration();

	Format	 format = filter->infoTrack->GetFormat();

	format.bits	= config->GetIntValue("MAD", "Enable24Bit", False) ? 24 : 16;
	format.channels	= header->mode == MAD_MODE_SINGLE_CHANNEL ? 1 : 2;
	format.rate	= header->samplerate;

	filter->infoTrack->approxLength = (filter->infoTrack->fileSize - filter->offset) / (header->bitrate / 8) * format.rate;

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
