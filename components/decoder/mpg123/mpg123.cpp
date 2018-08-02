 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2018 Robert Kausch <robert.kausch@freac.org>
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

#include "mpg123.h"
#include "config.h"
#include "xing/dxhead.h"

using namespace smooth::IO;

const String &BoCA::DecoderMPG123::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (mpg123dll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>mpg123 Decoder</name>					\
		    <version>1.0</version>					\
		    <id>mpg123-dec</id>						\
		    <type>decoder</type>					\
		    <replace>lame-dec</replace>					\
		    <replace>mad-dec</replace>					\
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
	LoadMPG123DLL();

	if (mpg123dll != NIL && ex_mpg123_init() != MPG123_OK)
	{
		FreeMPG123DLL();
	}
}

Void smooth::DetachDLL()
{
	if (mpg123dll == NIL) return;

	ex_mpg123_exit();

	FreeMPG123DLL();
}

Bool BoCA::DecoderMPG123::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.EndsWith(".mp1") ||
	       lcURI.EndsWith(".mp2") ||
	       lcURI.EndsWith(".mp3");
}

Error BoCA::DecoderMPG123::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	Format	 format = track.GetFormat();

	track.fileSize	= in.Size();
	track.length	= -1;

	SkipID3v2Tag(&in);
	ParseVBRHeaders(&in);

	Buffer<unsigned char>	 buffer(4096);

	mpg123_handle	*context = ex_mpg123_new(NIL, NIL);
	Int		 offset	 = in.GetPos();

	ex_mpg123_open_feed(context);

	do
	{
		Int	 bytes = Math::Min((Int64) buffer.Size(), in.Size() - in.GetPos());

		in.InputData((void *) buffer, bytes);

		int	 result = ex_mpg123_decode(context, buffer, bytes, NIL, 0, NIL);

		if (result == MPG123_NEW_FORMAT)
		{
			long	 rate	  = 0;
			int	 channels = 0;
			int	 enc	  = MPG123_ENC_ANY;

			ex_mpg123_getformat(context, &rate, &channels, &enc);

			format.channels	= channels;
			format.rate	= rate;

			if	(enc & MPG123_ENC_8)  format.bits = 8;
			else if (enc & MPG123_ENC_16) format.bits = 16;
			else if (enc & MPG123_ENC_24) format.bits = 24;
			else if (enc & MPG123_ENC_32) format.bits = 32;

			mpg123_frameinfo	 mp3data;

			ex_mpg123_info(context, &mp3data);

			if	(numFrames	 > 0) track.length = numFrames * ex_mpg123_spf(context) - delaySamples - padSamples;
			else if (mp3data.bitrate > 0) track.approxLength = (track.fileSize - offset) / (mp3data.bitrate * 1000 / 8) * format.rate;

			break;
		}
	}
	while (in.GetPos() < in.Size());

	ex_mpg123_delete(context);

	if (format == Format()) { errorState = True; errorString = "Invalid file format"; }

	track.SetFormat(format);

	in.Close();

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

BoCA::DecoderMPG123::DecoderMPG123()
{
	configLayer	 = NIL;

	context		 = 0;

	numBytes	 = 0;
	numFrames	 = 0;

	delaySamples	 = 0;
	padSamples	 = 0;

	dataOffset	 = 0;

	/* Initialize to decoder delay.
	 */
	delaySamplesLeft = 529;
}

BoCA::DecoderMPG123::~DecoderMPG123()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::DecoderMPG123::Activate()
{
	InStream	*f_in = new InStream(STREAM_DRIVER, driver);

	SkipID3v2Tag(f_in);
	ParseVBRHeaders(f_in);

	dataOffset = f_in->GetPos();

	delete f_in;

	driver->Seek(dataOffset);

	context = ex_mpg123_new(NIL, NIL);

	const Config	*config = GetConfiguration();

	String	 selectedDecoder = config->GetStringValue(ConfigureMPG123::ConfigID, "Decoder", NIL);

	if (selectedDecoder != NIL) ex_mpg123_decoder(context, selectedDecoder);

	ex_mpg123_open_feed(context);

	return True;
}

Bool BoCA::DecoderMPG123::Deactivate()
{
	ex_mpg123_delete(context);

	return True;
}

Bool BoCA::DecoderMPG123::Seek(Int64 samplePosition)
{
	Buffer<UnsignedByte>	 data(131072);
	off_t			 target = 0;

	while (ex_mpg123_feedseek(context, samplePosition, SEEK_SET, &target) == MPG123_NEED_MORE)
	{
		Int64	 size = driver->ReadData(data, data.Size());

		if (size == 0) break;

		ex_mpg123_feed(context, data, size);
	}

	driver->Seek(dataOffset + target);

	return True;
}

Int BoCA::DecoderMPG123::ReadData(Buffer<UnsignedByte> &data)
{
	/* Read input data.
	 */
	Int	 size = driver->ReadData(data, data.Size());

	if (size <= 0) return -1;

	inBytes += size;

	/* Decode samples.
	 */
	const Format	&format	     = track.GetFormat();
	size_t		 samplesDone = 0;

	samples.Resize(size * 48);

	if (ex_mpg123_decode(context, data, size, samples, samples.Size(), &samplesDone) == MPG123_NEW_FORMAT)
	{
		/* Call mpg123_decode again after MPG123_NEW_FORMAT to get decoded data.
		 */
		ex_mpg123_decode(context, NIL, 0, samples, samples.Size(), &samplesDone);
	}

	data.Resize(0);

	samplesDone = samplesDone / format.channels / (format.bits / 8);

	if (samplesDone > (unsigned) delaySamplesLeft)
	{
		data.Resize((samplesDone - delaySamplesLeft) * format.channels * (format.bits / 8));

		memcpy(data, samples + (delaySamplesLeft * format.channels * (format.bits / 8)), data.Size());
	}

	delaySamplesLeft = Math::Max(0, delaySamplesLeft - samplesDone);

	return data.Size();
}

Bool BoCA::DecoderMPG123::SkipID3v2Tag(InStream *in)
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

Bool BoCA::DecoderMPG123::ParseVBRHeaders(InStream *in)
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

	memset(&data, 0, sizeof(XHEADDATA));

	if (GetXingHeader(&data, buffer))
	{
		numBytes  = data.bytes - frameSize;
		numFrames = data.frames;

		/* Check for a LAME header and extract length information.
		 */
		Int		 offset = ((buffer[1] >> 3) & 1 ? (buffer[3] >> 6 != 3 ? 32 : 17) :
								  (buffer[3] >> 6 != 3 ? 17 :  9)) + 4;
		UnsignedByte	*xing	= buffer + offset;

		if (frameSize >= 192 && xing[0x78] == 'L' && xing[0x79] == 'A' && xing[0x7A] == 'M' && xing[0x7B] == 'E')
		{
			delaySamples = ( xing[0x8D]	    << 4) | ((xing[0x8E] & 0xF0) >> 4);
			padSamples   = ((xing[0x8E] & 0x0F) << 8) | ( xing[0x8F]	     );

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

Int BoCA::DecoderMPG123::GetMPEGFrameSize(const Buffer<UnsignedByte> &header)
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

ConfigLayer *BoCA::DecoderMPG123::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMPG123();

	return configLayer;
}
