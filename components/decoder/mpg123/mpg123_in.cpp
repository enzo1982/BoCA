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

#include "mpg123_in.h"
#include "config.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::MPG123In::GetComponentSpecs()
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
		    <id>mpg123-in</id>						\
		    <type>decoder</type>					\
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

Bool BoCA::MPG123In::CanOpenStream(const String &streamURI)
{
	String	 lcURI = streamURI.ToLower();

	return lcURI.EndsWith(".mp1") ||
	       lcURI.EndsWith(".mp2") ||
	       lcURI.EndsWith(".mp3");
}

Error BoCA::MPG123In::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READ);

	Format	 format = track.GetFormat();

	format.order	= BYTE_INTEL;
	track.fileSize	= f_in->Size();
	track.length	= -1;

	SkipID3v2Tag(f_in);
	ParseVBRHeaders(f_in);

	Buffer<unsigned char>	 buffer(4096);

	Buffer<unsigned char>	 samples(buffer.Size() * 128);
	size_t			 samplesDone;

	mpg123_handle	*context = ex_mpg123_new(NIL, NIL);

	ex_mpg123_open_feed(context);

	do
	{
		f_in->InputData((void *) buffer, buffer.Size());

		int	 result = ex_mpg123_decode(context, buffer, buffer.Size(), samples, samples.Size(), &samplesDone);

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

			/*if	(mp3data.nsamp	 > 0) track.length = mp3data.nsamp - delaySamples - padSamples;
			else if (mp3data.bitrate > 0)*/ track.approxLength = track.fileSize / (mp3data.bitrate * 1000 / 8) * format.rate;

			break;
		}
	}
	while (f_in->GetPos() < f_in->Size());

	ex_mpg123_delete(context);

	track.SetFormat(format);

	delete f_in;

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

BoCA::MPG123In::MPG123In()
{
	configLayer	 = NIL;

	context		 = 0;

	packageSize	 = 0;

	delaySamples	 = 0;
	padSamples	 = 0;

	/* Initialize to decoder delay.
	 */
	delaySamplesLeft = 529;
}

BoCA::MPG123In::~MPG123In()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::MPG123In::Activate()
{
	InStream	*f_in = new InStream(STREAM_DRIVER, driver);

	SkipID3v2Tag(f_in);
	ParseVBRHeaders(f_in);

	driver->Seek(f_in->GetPos());

	delete f_in;

	context = ex_mpg123_new(NIL, NIL);

	Config	*config = Config::Get();

	String	 selectedDecoder = config->GetStringValue("MPG123", "Decoder", NIL);

	if (selectedDecoder != NIL) ex_mpg123_decoder(context, selectedDecoder);

	ex_mpg123_open_feed(context);

	return True;
}

Bool BoCA::MPG123In::Deactivate()
{
	ex_mpg123_delete(context);

	return True;
}

Int BoCA::MPG123In::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (size <= 0) return -1;

	inBytes += size;

	data.Resize(size);

	size = driver->ReadData(data, size);

	Buffer<unsigned char>	 samples(size * 128);
	size_t			 samplesDone;

	ex_mpg123_decode(context, data, size, samples, samples.Size(), &samplesDone);

	const Format	&format = track.GetFormat();

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

Bool BoCA::MPG123In::SkipID3v2Tag(InStream *in)
{
	/* Check for an ID3v2 tag at the beginning of the
	 * file and skip it if it exists as LAME may crash
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

Bool BoCA::MPG123In::ParseVBRHeaders(InStream *in)
{
	/* Check for a LAME header and extract
	 * the number of samples if it exists.
	 */
	Buffer<UnsignedByte>	 buffer(228);

	/* Read data and seek back to before
	 * the Xing header.
	 */
	in->RelSeek(156);
	in->InputData(buffer, 228);
	in->RelSeek(-228);
	in->RelSeek(-156);

	if (buffer[0] == 'L' && buffer[1] == 'A' && buffer[2] == 'M' && buffer[3] == 'E')
	{
		delaySamples = ( buffer[21]	    << 4) | ((buffer[22] & 0xF0) >> 4);
		padSamples   = ((buffer[22] & 0x0F) << 8) | ( buffer[23]	     );

		delaySamplesLeft += delaySamples;

		return True;
	}

	return False;
}

ConfigLayer *BoCA::MPG123In::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureMPG123();

	return configLayer;
}
