 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2026 Robert Kausch <robert.kausch@freac.org>
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

#include <smooth/io/drivers/driver_ansi.h>

#include "mad.h"
#include "config.h"

#include "../../shared/mp3frame.h"
#include "../../shared/vbrtag.h"

using namespace smooth::IO;
using namespace smooth::Threads;

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
	DriverANSI	 ioDriver(File(streamURI), IS_READ);
	InStream	 in(STREAM_DRIVER, &ioDriver);

	SkipID3v2Tag(in);
	ParseVBRHeaders(in);

	track.fileSize	   = in.Size();
	track.length	   = -1;

	infoTrack	   = &track;
	stop		   = False;
	finishing	   = False;
	finished	   = False;

	offset		   = in.GetPos();
	driver		   = &ioDriver;
	driver->Seek(offset);

	ReadMAD(False);

	if (track.GetFormat() == Format()) { errorState = True; errorString = "Invalid file format"; }

	in.Close();
	ioDriver.Close();

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

	if (errorState)	return Error();
	else		return Success();
}

BoCA::DecoderMAD::DecoderMAD() : samplesRequestedSignal(1), samplesAvailableSignal(1)
{
	configLayer	 = NIL;

	infoTrack	 = NIL;
	decoderThread	 = NIL;

	stop		 = False;
	finishing	 = False;
	finished	 = False;

	offset		 = 0;

	numBytes	 = 0;
	numFrames	 = 0;

	delaySamples	 = 0;
	padSamples	 = 0;

	/* Initialize to decoder delay.
	 */
	delaySamplesLeft = 529;
}

BoCA::DecoderMAD::~DecoderMAD()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DecoderMAD::Activate()
{
	/* Skip headers.
	 */
	InStream	 in(STREAM_DRIVER, driver);

	SkipID3v2Tag(in);
	ParseVBRHeaders(in);

	driver->Seek(in.GetPos());

	/* Prepare decoder.
	 */
	infoTrack     = new Track();
	decoderThread = NIL;

	stop	      = False;
	finishing     = False;
	finished      = False;

	samplesAvailableSignal.Wait();

	return True;
}

Bool BoCA::DecoderMAD::Deactivate()
{
	if (decoderThread != NIL)
	{
		stop = True;

		samplesRequestedSignal.Release();

		decoderThread->Wait();
	}

	delete infoTrack;

	return True;
}

Int BoCA::DecoderMAD::ReadData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	if (decoderThread == NIL) decoderThread = NonBlocking1<Bool>(&DecoderMAD::ReadMAD, this).Call(True);

	if (finished && samplesBuffer.Size() <= 0) return -1;

	samplesAvailableSignal.Wait();

	/* Convert samples to target format.
	 */
	const Format	&format = track.GetFormat();

	data.Resize(samplesBuffer.Size() * (format.bits / 8));

	for (Int i = 0; i < samplesBuffer.Size(); i++)
	{
		int	 sample = scale(samplesBuffer[i], format.bits);

		if	(format.bits == 16				) ((Short *) (unsigned char *) data)[i] = sample;

		else if (format.bits == 24 && endianness == EndianLittle) { data[i * 3 + 2] = (sample >> 16) & 0xFF; data[i * 3 + 1] = (sample >> 8) & 0xFF; data[i * 3    ] = sample & 0xFF; }
		else if (format.bits == 24 && endianness == EndianBig	) { data[i * 3    ] = (sample >> 16) & 0xFF; data[i * 3 + 1] = (sample >> 8) & 0xFF; data[i * 3 + 2] = sample & 0xFF; }
	}

	samplesBuffer.Resize(0);

	samplesRequestedSignal.Release();

	return data.Size();
}

Bool BoCA::DecoderMAD::SkipID3v2Tag(InStream &in)
{
	/* Check for an ID3v2 tag at the beginning of the
	 * file and skip it if it exists as it might cause
	 * problems if the tag is unsynchronized.
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

Bool BoCA::DecoderMAD::ParseVBRHeaders(InStream &in)
{
	/* Read MPEG header and get frame size.
	 */
	Buffer<UnsignedByte>	 header(3);

	in.InputData(header, 3);
	in.RelSeek(-3);

	Int	 frameSize = MP3Frame::GetMPEGFrameSize(header);

	if (frameSize < 156) return False;

	/* Read frame.
	 */
	Buffer<UnsignedByte>	 buffer(frameSize);

	in.InputData(buffer, frameSize);

	/* Check for a Xing or VBRI header and extract the number of frames.
	 */
	VbrTag	 vbrTag;

	if (vbrTag.Parse(buffer))
	{
		if (vbrTag.flags & VbrTag::FLAG_BYTES)  numBytes  = vbrTag.bytes - frameSize;
		if (vbrTag.flags & VbrTag::FLAG_FRAMES) numFrames = vbrTag.frames;

		if (vbrTag.flags & VbrTag::FLAG_GAPLESS)
		{
			delaySamples = vbrTag.delay;
			padSamples   = vbrTag.padding;

			delaySamplesLeft += delaySamples;
		}
	}
	else
	{
		/* Seek back to before the frame if no Xing or VBRI header was found.
		 */
		in.RelSeek(-frameSize);

		return False;
	}

	/* Sanity check for header vs. actual file size (account
	 * for possible ID3v1 and addtional 4kB of other tags).
	 */
	if (numBytes > 0 && in.Size() - in.GetPos() > numBytes * 1.01 + 128 + 4096)
	{
		numFrames  = 0;

		padSamples = 0;
	}

	return True;
}

Int BoCA::DecoderMAD::ReadMAD(Bool readData)
{
	/* Initialize decoder.
	 */
	mad_decoder	 decoder;

	if (readData)	ex_mad_decoder_init(&decoder, this, &MADInputCallback, NIL, NIL, &MADOutputCallback, &MADErrorCallback, NIL);
	else		ex_mad_decoder_init(&decoder, this, &MADInputCallback, NIL, NIL, &MADHeaderCallback, &MADErrorCallback, NIL);

	/* Process header and audio.
	 */
	ex_mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

	/* Finish and free decoder.
	 */
	finished = True;

	samplesAvailableSignal.Release();

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

	if (filter->stop || filter->finishing) return MAD_FLOW_STOP;

	/* Check if we have any more data. If not, append an empty
	 * frame to the last frame to allow the decoder to finish.
	 */
	if (filter->driver->GetPos() == filter->driver->GetSize()) filter->finishing = True;

	Int	 bytes = Math::Min((Int64) 131072, filter->finishing ? 1440 : filter->driver->GetSize() - filter->driver->GetPos());
	Int	 backup = stream->bufend - stream->next_frame;

	memmove(filter->inputBuffer, stream->next_frame, backup);

	filter->inputBuffer.Resize(bytes + backup);

	if (!filter->finishing) filter->driver->ReadData(filter->inputBuffer + backup, bytes);
	else			memset(filter->inputBuffer + backup, 0, bytes);

	ex_mad_stream_buffer(stream, filter->inputBuffer, bytes + backup);

	filter->inBytes += bytes;

	return MAD_FLOW_CONTINUE;
}

mad_flow BoCA::MADOutputCallback(void *client_data, const mad_header *header, mad_pcm *pcm)
{
	DecoderMAD	*filter = (DecoderMAD *) client_data;

	if (filter->stop) return MAD_FLOW_STOP;

	filter->samplesRequestedSignal.Wait();

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

	filter->samplesAvailableSignal.Release();

	return MAD_FLOW_CONTINUE;
}

mad_flow BoCA::MADHeaderCallback(void *client_data, const mad_header *header, mad_pcm *pcm)
{
	DecoderMAD	*filter = (DecoderMAD *) client_data;
	const Config	*config = filter->GetConfiguration();

	Format	 format = filter->infoTrack->GetFormat();

	format.bits	= config->GetIntValue(ConfigureMAD::ConfigID, "Enable24Bit", False) ? 24 : 16;
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
