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

#include "lame.h"

using namespace smooth::IO;

const String &BoCA::DecoderLAME::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (lamedll != NIL)
	{
		componentSpecs = "						\
										\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>			\
		  <component>							\
		    <name>LAME MP3 Decoder</name>				\
		    <version>1.0</version>					\
		    <id>lame-dec</id>						\
		    <type>decoder</type>					\
		    <format>							\
		      <name>MPEG 1 Audio Layer 3</name>				\
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
	LoadLAMEDLL();
}

Void smooth::DetachDLL()
{
	FreeLAMEDLL();
}

Bool BoCA::DecoderLAME::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".mp3");
}

Error BoCA::DecoderLAME::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READ);

	Format	 format = track.GetFormat();

	format.bits	= 16;
	track.fileSize	= f_in->Size();
	track.length	= -1;

	SkipID3v2Tag(f_in);
	ParseVBRHeaders(f_in);

	Buffer<unsigned char>	 buffer(4096);

	Buffer<short>		 pcm_l(buffer.Size() * 64);
	Buffer<short>		 pcm_r(buffer.Size() * 64);

	hip_t	 context = ex_hip_decode_init();
	Int	 offset	 = f_in->GetPos();

	do
	{
		f_in->InputData((void *) buffer, buffer.Size());

		mp3data_struct	 mp3data;

		memset(&mp3data, 0, sizeof(mp3data));

		Int	 nSamples = ex_hip_decode_headers(context, buffer, buffer.Size(), pcm_l, pcm_r, &mp3data);

		if (mp3data.header_parsed && nSamples > mp3data.framesize)
		{
			format.channels	= mp3data.stereo;
			format.rate	= mp3data.samplerate;

			if	(mp3data.nsamp	 > 0) track.length = mp3data.nsamp - delaySamples - padSamples;
			else if (mp3data.bitrate > 0) track.approxLength = (track.fileSize - offset) / (mp3data.bitrate * 1000 / 8) * format.rate;

			break;
		}
	}
	while (f_in->GetPos() < f_in->Size());

	ex_hip_decode_exit(context);

	track.SetFormat(format);

	delete f_in;

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

BoCA::DecoderLAME::DecoderLAME()
{
	context		 = 0;

	packageSize	 = 0;

	delaySamples	 = 0;
	padSamples	 = 0;

	/* Initialize to decoder delay.
	 */
	delaySamplesLeft = 529;
}

BoCA::DecoderLAME::~DecoderLAME()
{
}

Bool BoCA::DecoderLAME::Activate()
{
	InStream	*f_in = new InStream(STREAM_DRIVER, driver);

	SkipID3v2Tag(f_in);
	ParseVBRHeaders(f_in);

	driver->Seek(f_in->GetPos());

	delete f_in;

	context = ex_hip_decode_init();

	return True;
}

Bool BoCA::DecoderLAME::Deactivate()
{
	ex_hip_decode_exit(context);

	return True;
}

Int BoCA::DecoderLAME::ReadData(Buffer<UnsignedByte> &data)
{
	pcm_l.Resize(data.Size() * 64);
	pcm_r.Resize(data.Size() * 64);

	Int	 size = driver->ReadData(data, data.Size());

	if (size <= 0) return -1;

	inBytes += size;

	const Format	&format	  = track.GetFormat();
	Int		 nSamples = ex_hip_decode(context, data, size, pcm_l, pcm_r);

	/* Try harder in case we got no data yet.
	 */
	for (Int i = 0; nSamples == 0 && i < size / 384; i++) nSamples = ex_hip_decode(context, NIL, 0, pcm_l, pcm_r);

	data.Resize(0);

	if (nSamples > delaySamplesLeft)
	{
		data.Resize((nSamples - delaySamplesLeft) * format.channels * (format.bits / 8));

		for (Int i = delaySamplesLeft; i < nSamples; i++)
		{
			for (Int j = 0; j < format.channels; j++) ((short *) (unsigned char *) data)[format.channels * (i - delaySamplesLeft) + j] = (j == 0) ? pcm_l[i] : pcm_r[i];
		}
	}

	delaySamplesLeft = Math::Max(0, delaySamplesLeft - nSamples);

	return data.Size();
}

Bool BoCA::DecoderLAME::SkipID3v2Tag(InStream *in)
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

Bool BoCA::DecoderLAME::ParseVBRHeaders(InStream *in)
{
	/* Check for a LAME header and extract the number of samples if it exists.
	 */
	Buffer<UnsignedByte>	 buffer(192);

	/* Read data and seek back to before the Xing header.
	 */
	in->InputData(buffer, 192);
	in->RelSeek(-192);

	if (buffer[0x9C] == 'L' && buffer[0x9D] == 'A' && buffer[0x9E] == 'M' && buffer[0x9F] == 'E')
	{
		delaySamples = ( buffer[0xB1]	      << 4) | ((buffer[0xB2] & 0xF0) >> 4);
		padSamples   = ((buffer[0xB2] & 0x0F) << 8) | ( buffer[0xB3]		 );

		delaySamplesLeft += delaySamples;

		return True;
	}

	return False;
}
