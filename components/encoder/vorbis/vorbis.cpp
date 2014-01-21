 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2014 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <time.h>
#include <stdlib.h>

#include "vorbis.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderVorbis::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && vorbisdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Ogg Vorbis Encoder</name>					\
		    <version>1.0</version>						\
		    <id>vorbis-enc</id>							\
		    <type>encoder</type>						\
		    <format>								\
		      <name>Ogg Vorbis Audio</name>					\
		      <extension>ogg</extension>					\
		      <extension>oga</extension>					\
		      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>	\
		    </format>								\
		  </component>								\
											\
		";
	}

	return componentSpecs;
}

Void smooth::AttachDLL(Void *instance)
{
	LoadOggDLL();
	LoadVorbisDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
	FreeVorbisDLL();
}

BoCA::EncoderVorbis::EncoderVorbis()
{
	configLayer = NIL;

	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));

	memset(&vi, 0, sizeof(vi));
	memset(&vc, 0, sizeof(vc));
	memset(&vd, 0, sizeof(vd));
	memset(&vb, 0, sizeof(vb));
}

BoCA::EncoderVorbis::~EncoderVorbis()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderVorbis::Activate()
{
	const Format	&format = track.GetFormat();
	const Info	&info = track.GetInfo();

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	Config	*config = Config::Get();

	srand(clock());

	ex_vorbis_info_init(&vi);

	switch (config->GetIntValue("Vorbis", "Mode", 0))
	{
		case 0:
			ex_vorbis_encode_init_vbr(&vi, format.channels, format.rate, ((double) config->GetIntValue("Vorbis", "Quality", 60)) / 100);
			break;
		case 1:
			ex_vorbis_encode_init(&vi, format.channels, format.rate, config->GetIntValue("Vorbis", "SetMinBitrate", False) ? config->GetIntValue("Vorbis", "MinBitrate",  32) * 1000 : -1,
										 config->GetIntValue("Vorbis", "SetBitrate",    True)  ? config->GetIntValue("Vorbis", "Bitrate",    192) * 1000 : -1,
										 config->GetIntValue("Vorbis", "SetMaxBitrate", False) ? config->GetIntValue("Vorbis", "MaxBitrate", 320) * 1000 : -1);
			break;
	}

	ex_vorbis_comment_init(&vc);
	ex_vorbis_analysis_init(&vd, &vi);
	ex_vorbis_block_init(&vd, &vb);

	ex_ogg_stream_init(&os, rand());

	ogg_packet	 header;
	ogg_packet	 header_comm;
	ogg_packet	 header_code;

	ex_vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);

	ex_ogg_stream_packetin(&os, &header); /* automatically placed in its own page */

	/* Write Vorbis comment header
	 */
	{
		/* Read vendor string.
		 */
		InStream	 in(STREAM_BUFFER, header_comm.packet + 7, header_comm.bytes - 7);
		String		 vendor = in.InputString(in.InputNumber(4));

		Buffer<unsigned char>	 vcBuffer;

		/* Render actual Vorbis comment tag.
		 *
		 * An empty tag containing only the vendor string
		 * is rendered if Vorbis comments are disabled.
		 */
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			tagger->SetVendorString(vendor);

			if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableVorbisComment", True)) tagger->RenderBuffer(vcBuffer, track);
			else													   tagger->RenderBuffer(vcBuffer, Track());

			boca.DeleteComponent(tagger);
		}

		vcBuffer.Resize(vcBuffer.Size() + 8);

		memmove(vcBuffer + 7, vcBuffer, vcBuffer.Size() - 8);
		memcpy(vcBuffer + 1, "vorbis", 6);
		vcBuffer[0] = 3;
		vcBuffer[vcBuffer.Size() - 1] = 1;

		ogg_packet	 header_comm2 = { vcBuffer, vcBuffer.Size(), 0, 0, 0, 1 };

		ex_ogg_stream_packetin(&os, &header_comm2);
	}

	ex_ogg_stream_packetin(&os, &header_code);

	WriteOggPackets(True);

	return True;
}

Bool BoCA::EncoderVorbis::Deactivate()
{
	ex_vorbis_analysis_wrote(&vd, 0);

	while (ex_vorbis_analysis_blockout(&vd, &vb) == 1)
	{
		ex_vorbis_analysis(&vb, NULL);
		ex_vorbis_bitrate_addblock(&vb);

		while (ex_vorbis_bitrate_flushpacket(&vd, &op))
		{
			ex_ogg_stream_packetin(&os, &op);

			WriteOggPackets(False);
		}
	}

	ex_ogg_stream_clear(&os);
	ex_vorbis_block_clear(&vb);
	ex_vorbis_dsp_clear(&vd);
	ex_vorbis_comment_clear(&vc);
	ex_vorbis_info_clear(&vi);

	return True;
}

Int BoCA::EncoderVorbis::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format	= track.GetFormat();

	/* Convert samples to 16 bit.
	 */
	Int	 samples_size = size / (format.bits / 8);

	samplesBuffer.Resize(samples_size);

	for (Int i = 0; i < samples_size; i++)
	{
		if	(format.bits ==  8				) samplesBuffer[i] =	   (				 data [i] - 128) * 256;
		else if (format.bits == 16				) samplesBuffer[i] = (int)  ((short *) (unsigned char *) data)[i];
		else if (format.bits == 32				) samplesBuffer[i] = (int) (((long *)  (unsigned char *) data)[i]	 / 65536);

		else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[i] = (int) ((data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 65536);
		else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[i] = (int) ((data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 65536);
	}

	/* Write samples to analysis buffer.
	 */
	float	**buffer = ex_vorbis_analysis_buffer(&vd, samples_size / format.channels);

	if (format.channels == 1)
	{
		for (Int i = 0; i < samples_size; i++)
		{
			if (endianness == EndianLittle) { buffer[0][i] = ((((signed char *) (unsigned short *) samplesBuffer)[i * 2 + 1] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[i * 2 + 0])) / 32768.f; }
			else				{ buffer[0][i] = ((((signed char *) (unsigned short *) samplesBuffer)[i * 2 + 0] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[i * 2 + 1])) / 32768.f; }
		}
	}
	else if (format.channels == 2)
	{
		for (Int i = 0; i < samples_size / 2; i++)
		{
			if (endianness == EndianLittle) { buffer[0][i] = ((((signed char *) (unsigned short *) samplesBuffer)[i * 4 + 1] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[i * 4 + 0])) / 32768.f;
							  buffer[1][i] = ((((signed char *) (unsigned short *) samplesBuffer)[i * 4 + 3] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[i * 4 + 2])) / 32768.f; }
			else				{ buffer[0][i] = ((((signed char *) (unsigned short *) samplesBuffer)[i * 4 + 0] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[i * 4 + 1])) / 32768.f;
							  buffer[1][i] = ((((signed char *) (unsigned short *) samplesBuffer)[i * 4 + 2] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[i * 4 + 3])) / 32768.f; }
		}
	}

	ex_vorbis_analysis_wrote(&vd, samples_size / format.channels);

	/* Output samples to encoder.
	 */
	Int	 dataLength = 0;

	while (ex_vorbis_analysis_blockout(&vd, &vb) == 1)
	{
		ex_vorbis_analysis(&vb, NULL);
		ex_vorbis_bitrate_addblock(&vb);

		while (ex_vorbis_bitrate_flushpacket(&vd, &op))
		{
			ex_ogg_stream_packetin(&os, &op);

			dataLength += WriteOggPackets(False);
		}
	}

	return dataLength;
}

Int BoCA::EncoderVorbis::WriteOggPackets(Bool flush)
{
	Int	 bytes = 0;

	do
	{
		int	 result = 0;

		if (flush) result = ex_ogg_stream_flush(&os, &og);
		else	   result = ex_ogg_stream_pageout(&os, &og);

		if (result == 0) break;

		bytes += driver->WriteData(og.header, og.header_len);
		bytes += driver->WriteData(og.body, og.body_len);
	}
	while (true);

	return bytes;
}

String BoCA::EncoderVorbis::GetOutputFileExtension()
{
	Config	*config = Config::Get();

	switch (config->GetIntValue("Vorbis", "FileExtension", 0))
	{
		default:
		case  0: return "ogg";
		case  1: return "oga";
	}
}

ConfigLayer *BoCA::EncoderVorbis::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureVorbis();

	return configLayer;
}
