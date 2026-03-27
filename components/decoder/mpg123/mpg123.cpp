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

#include "mpg123.h"
#include "config.h"

#include "../../shared/mp3frame.h"
#include "../../shared/vbrtag.h"

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
		    <name>mpg123 Decoder %VERSION%</name>			\
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

		if (ex_mpg123_distversion != NIL) componentSpecs.Replace("%VERSION%", String("v").Append(ex_mpg123_distversion(NIL, NIL, NIL)));
		else				  componentSpecs.Replace(" %VERSION%", String());
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

	SkipID3v2Tag(in);
	ParseVBRHeaders(in);

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
	/* Get configuration.
	 */
	const Config	*config = GetConfiguration();

	String	 selectedDecoder = config->GetStringValue(ConfigureMPG123::ConfigID, "Decoder", NIL);

	/* Skip headers and get data offset.
	 */
	InStream	 in(STREAM_DRIVER, driver);

	SkipID3v2Tag(in);
	ParseVBRHeaders(in);

	dataOffset = in.GetPos();

	driver->Seek(dataOffset);

	/* Create decoder.
	 */
	context = ex_mpg123_new(NIL, NIL);

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

Bool BoCA::DecoderMPG123::SkipID3v2Tag(InStream &in)
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

Bool BoCA::DecoderMPG123::ParseVBRHeaders(InStream &in)
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

ConfigLayer *BoCA::DecoderMPG123::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMPG123();

	return configLayer;
}
