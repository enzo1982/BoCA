 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2013 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "bonk.h"

using namespace smooth::IO;

const String &BoCA::DecoderBonk::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (bonkdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Bonk Audio Decoder</name>		\
		    <version>1.0</version>			\
		    <id>bonk-dec</id>				\
		    <type>decoder</type>			\
		    <format>					\
		      <name>Bonk Audio Files</name>		\
		      <extension>bonk</extension>		\
		    </format>					\
		  </component>					\
								\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadBonkDLL();
}

Void smooth::DetachDLL()
{
	FreeBonkDLL();
}

Bool BoCA::DecoderBonk::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".bonk");
}

Error BoCA::DecoderBonk::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*in		= new InStream(STREAM_FILE, streamURI, IS_READ);
	unsigned int	 length		= 0;
	unsigned int	 rate		= 0;
	int		 channels	= 0;
	void		*decoder	= ex_bonk_decoder_create();

	Format		 format		= track.GetFormat();

	int		 bytes = Math::Min(in->Size(), (Int64) 524288);

	Buffer<unsigned char>	 dataBuffer(bytes);

	in->InputData(dataBuffer, bytes);

	ex_bonk_decoder_init(decoder, dataBuffer, bytes, &length, &rate, &channels);

	format.rate = rate;
	format.channels = channels;
	format.bits = 16;

	track.SetFormat(format);

	track.length = length / format.channels;
	track.fileSize = in->Size();

	unsigned char	*id3tag = NIL;
	int		 id3tag_size = 0;

	ex_bonk_decoder_get_id3_data(decoder, &id3tag, &id3tag_size);

	if (id3tag_size > 0)
	{
		Buffer<unsigned char>	 buffer(id3tag_size);

		memcpy(buffer, id3tag, id3tag_size);

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("id3v2-tag");

		if (tagger != NIL)
		{
			tagger->ParseBuffer(buffer, track);

			boca.DeleteComponent(tagger);
		}
	}

	ex_bonk_decoder_close(decoder);

	delete in;

	return Success();
}

BoCA::DecoderBonk::DecoderBonk()
{
	decoder	= NIL;

	packageSize = 0;
}

BoCA::DecoderBonk::~DecoderBonk()
{
}

Bool BoCA::DecoderBonk::Activate()
{
	unsigned int	 length		= 0;
	unsigned int	 rate		= 0;
	int		 channels	= 0;

	decoder = ex_bonk_decoder_create();

	int		 bytes = Math::Min(driver->GetSize(), (Int64) 524288);

	dataBuffer.Resize(bytes);

	driver->ReadData(dataBuffer, bytes);

	ex_bonk_decoder_init(decoder, dataBuffer, bytes, &length, &rate, &channels);

	return True;
}

Bool BoCA::DecoderBonk::Deactivate()
{
	ex_bonk_decoder_finish(decoder);

	ex_bonk_decoder_close(decoder);

	return True;
}

Int BoCA::DecoderBonk::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	size = driver->ReadData(dataBuffer, size >= 0 ? size : 0);

	data.Resize(131072);

	Int	 nSamples = ex_bonk_decoder_decode_packet(decoder, dataBuffer, size, (signed short *) (unsigned char *) data, data.Size());

	return (nSamples == -1) ? 0 : nSamples * (track.GetFormat().bits / 8);
}