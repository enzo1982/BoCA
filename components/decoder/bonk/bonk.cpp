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
		    <name>Bonk Audio Decoder %VERSION%</name>	\
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

		componentSpecs.Replace("%VERSION%", String("v").Append(ex_bonk_get_version_string()));
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

	Buffer<UnsignedByte>	 data(Math::Min(in->Size(), (Int64) 524288));

	in->InputData(data, data.Size());

	ex_bonk_decoder_init(decoder, dataBuffer, data.Size(), &length, &rate, &channels);

	format.rate	= rate;
	format.channels	= channels;
	format.bits	= 16;

	track.SetFormat(format);

	track.length = length / format.channels;
	track.fileSize = in->Size();

	bool		 lossless = False;
	bool		 mid_side = False;

	ex_bonk_decoder_get_stream_info(decoder, &lossless, &mid_side);

	track.lossless = lossless;

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
			tagger->SetConfiguration(GetConfiguration());
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

	Buffer<UnsignedByte>	 data(Math::Min(driver->GetSize(), (Int64) 524288));

	driver->ReadData(data, data.Size());

	ex_bonk_decoder_init(decoder, data, data.Size(), &length, &rate, &channels);

	return True;
}

Bool BoCA::DecoderBonk::Deactivate()
{
	ex_bonk_decoder_finish(decoder);

	ex_bonk_decoder_close(decoder);

	return True;
}

Int BoCA::DecoderBonk::ReadData(Buffer<UnsignedByte> &data)
{
	Int	 size = driver->ReadData(data, data.Size());

	data.Resize(Math::Max(data.Size(), 131072));

	Int	 nSamples = ex_bonk_decoder_decode_packet(decoder, data, Math::Max(0, size), (signed short *) (UnsignedByte *) data, data.Size());

	if (nSamples == -1) return -1;

	data.Resize(nSamples * (track.GetFormat().bits / 8));

	return data.Size();
}
