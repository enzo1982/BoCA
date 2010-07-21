 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "bonk_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::BonkIn::GetComponentSpecs()
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
		    <id>bonk-in</id>				\
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

Bool BoCA::BonkIn::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".bonk");
}

Error BoCA::BonkIn::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	*in		= new InStream(STREAM_FILE, streamURI, IS_READ);
	unsigned int	 length		= 0;
	unsigned int	 rate		= 0;
	int		 channels	= 0;
	void		*decoder	= ex_bonk_decoder_create();

	Format		 format		= track.GetFormat();

	int		 bytes = Math::Min(in->Size(), 524288);

	dataBuffer.Resize(bytes);

	in->InputData(dataBuffer, bytes);

	ex_bonk_decoder_init(decoder, dataBuffer, bytes, &length, &rate, &channels);

	format.rate = rate;
	format.channels = channels;
	format.order = BYTE_INTEL;
	format.bits = 16;

	track.SetFormat(format);

	track.length = length;
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

BoCA::BonkIn::BonkIn()
{
	decoder	= NIL;

	packageSize = 0;
}

BoCA::BonkIn::~BonkIn()
{
}

Bool BoCA::BonkIn::Activate()
{
	unsigned int	 length		= 0;
	unsigned int	 rate		= 0;
	int		 channels	= 0;

	decoder = ex_bonk_decoder_create();

	int		 bytes = Math::Min(driver->GetSize(), 524288);

	dataBuffer.Resize(bytes);

	driver->ReadData(dataBuffer, bytes);

	ex_bonk_decoder_init(decoder, dataBuffer, bytes, &length, &rate, &channels);

	return True;
}

Bool BoCA::BonkIn::Deactivate()
{
	ex_bonk_decoder_finish(decoder);

	ex_bonk_decoder_close(decoder);

	return True;
}

Int BoCA::BonkIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	size = driver->ReadData(dataBuffer, size >= 0 ? size : 0);

	data.Resize(131072);

	Int	 nSamples = ex_bonk_decoder_decode_packet(decoder, dataBuffer, size, (signed short *) (unsigned char *) data, data.Size());

	return (nSamples == -1) ? 0 : nSamples * (track.GetFormat().bits / 8);
}
