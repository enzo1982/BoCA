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

#include "lame_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::LAMEIn::GetComponentSpecs()
{

	static String	 componentSpecs;

	if (lamedll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>LAME MP3 Decoder</name>		\
		    <version>1.0</version>			\
		    <id>lame-in</id>				\
		    <type>decoder</type>			\
		    <format>					\
		      <name>MPEG 1 Audio Layer 3</name>		\
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

Error BoCA::LAMEIn::GetStreamInfo(const String &streamURI, Track &format)
{
	ex_lame_decode_init();

	InStream	*f_in = new InStream(STREAM_FILE, streamURI, IS_READONLY);

	format.order	= BYTE_INTEL;
	format.bits	= 16;
	format.fileSize	= f_in->Size();
	format.length	= -1;

	SkipID3v2Tag(f_in);

	Buffer<unsigned char>	 buffer(4096);

	do
	{
		f_in->InputData((void *) buffer, buffer.Size());

		pcm_l.Resize(buffer.Size() * 64);
		pcm_r.Resize(buffer.Size() * 64);

		mp3data_struct	 mp3data;

		ZeroMemory(&mp3data, sizeof(mp3data));

		Int	 nSamples = ex_lame_decode_headers(buffer, buffer.Size(), pcm_l, pcm_r, &mp3data);

		if (mp3data.header_parsed && nSamples > mp3data.framesize)
		{
			format.channels	= mp3data.stereo;
			format.rate	= mp3data.samplerate;

			if	(mp3data.nsamp	 > 0) format.length = mp3data.nsamp * format.channels;
			else if (mp3data.bitrate > 0) format.approxLength = format.fileSize / (mp3data.bitrate * 1000 / 8) * format.rate * format.channels;

			break;
		}
	}
	while (f_in->GetPos() < f_in->Size());

	delete f_in;

	ex_lame_decode_exit();

	if (Config::Get()->enable_id3)
	{
		format.track = -1;
		format.outfile = NIL;

		format.ParseID3Tag(streamURI);
	}

	return Success();
}

BoCA::LAMEIn::LAMEIn()
{
	packageSize = 0;
}

BoCA::LAMEIn::~LAMEIn()
{
}

Bool BoCA::LAMEIn::Activate()
{
	ex_lame_decode_init();

	InStream	*f_in = new InStream(STREAM_DRIVER, driver);

	SkipID3v2Tag(f_in);

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

	Int	 nSamples = ex_lame_decode(data, size, pcm_l, pcm_r);

	data.Resize(nSamples * format.channels * (format.bits / 8));

	for (Int i = 0; i < nSamples; i++)
	{
		for (Int j = 0; j < 2; j++) ((short *) (unsigned char *) data)[2 * i + j] = (j == 0) ? pcm_l[i] : pcm_r[i];
	}

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
