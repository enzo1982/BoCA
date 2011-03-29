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

#include "lame_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::LAMEIn::GetComponentSpecs()
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
		    <id>lame-in</id>						\
		    <type>decoder</type>					\
		    <format>							\
		      <name>MPEG 1 Audio Layer 3</name>				\
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
	LoadLAMEDLL();
}

Void smooth::DetachDLL()
{
	FreeLAMEDLL();
}

Bool BoCA::LAMEIn::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".mp3");
}

Error BoCA::LAMEIn::GetStreamInfo(const String &streamURI, Track &track)
{
	ex_lame_decode_init();

	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READ);

	Format	 format = track.GetFormat();

	format.order	= BYTE_INTEL;
	format.bits	= 16;
	track.fileSize	= f_in->Size();
	track.length	= -1;

	SkipID3v2Tag(f_in);
	ParseVBRHeaders(f_in);

	Buffer<unsigned char>	 buffer(4096);

	do
	{
		f_in->InputData((void *) buffer, buffer.Size());

		pcm_l.Resize(buffer.Size() * 64);
		pcm_r.Resize(buffer.Size() * 64);

		mp3data_struct	 mp3data;

		memset(&mp3data, 0, sizeof(mp3data));

		Int	 nSamples = ex_lame_decode_headers(buffer, buffer.Size(), pcm_l, pcm_r, &mp3data);

		if (mp3data.header_parsed && nSamples > mp3data.framesize)
		{
			format.channels	= mp3data.stereo;
			format.rate	= mp3data.samplerate;

			if	(mp3data.nsamp	 > 0) track.length = mp3data.nsamp - delaySamples - padSamples;
			else if (mp3data.bitrate > 0) track.approxLength = track.fileSize / (mp3data.bitrate * 1000 / 8) * format.rate;

			break;
		}
	}
	while (f_in->GetPos() < f_in->Size());

	track.SetFormat(format);

	delete f_in;

	ex_lame_decode_exit();

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

BoCA::LAMEIn::LAMEIn()
{
	packageSize	 = 0;

	delaySamples	 = 0;
	padSamples	 = 0;

	/* Initialize to decoder delay.
	 */
	delaySamplesLeft = 529;
}

BoCA::LAMEIn::~LAMEIn()
{
}

Bool BoCA::LAMEIn::Activate()
{
	ex_lame_decode_init();

	InStream	*f_in = new InStream(STREAM_DRIVER, driver);

	SkipID3v2Tag(f_in);
	ParseVBRHeaders(f_in);

	driver->Seek(f_in->GetPos());

	delete f_in;

	return True;
}

Bool BoCA::LAMEIn::Deactivate()
{
	ex_lame_decode_exit();

	return True;
}

Int BoCA::LAMEIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (size <= 0) return -1;

	inBytes += size;

	data.Resize(size);

	size = driver->ReadData(data, size);

	pcm_l.Resize(size * 64);
	pcm_r.Resize(size * 64);

	Int		 nSamples = ex_lame_decode(data, size, pcm_l, pcm_r);
	const Format	&format = track.GetFormat();

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

Bool BoCA::LAMEIn::SkipID3v2Tag(InStream *in)
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

Bool BoCA::LAMEIn::ParseVBRHeaders(InStream *in)
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
