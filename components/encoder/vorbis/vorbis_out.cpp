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

#include <time.h>

#include "vorbis_out.h"
#include "config.h"
#include "dllinterface.h"

const String &BoCA::VorbisOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && vorbisdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Ogg Vorbis Encoder</name>		\
		    <version>1.0</version>			\
		    <id>vorbis-out</id>				\
		    <type>encoder</type>			\
		    <format>					\
		      <name>Ogg Vorbis Audio</name>		\
		      <extension>ogg</extension>		\
		    </format>					\
		  </component>					\
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

BoCA::VorbisOut::VorbisOut()
{
	configLayer = NIL;
}

BoCA::VorbisOut::~VorbisOut()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::VorbisOut::Activate()
{
	const Format	&format = track.GetFormat();

	if (format.channels > 2)
	{
		Utilities::ErrorMessage("BonkEnc does not support more than 2 channels!");

		errorState = True;

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
			ex_vorbis_encode_init(&vi, format.channels, format.rate, -1, config->GetIntValue("Vorbis", "Bitrate", 192) * 1000, -1);
			break;
	}

	ex_vorbis_comment_init(&vc);

	if (config->enable_vctags)
	{
		char	*prevOutFormat = String::SetOutputFormat(config->vctag_encoding);

		if (track.artist != NIL || track.title != NIL)
		{
			if	(track.title  != NIL) ex_vorbis_comment_add_tag(&vc, (char *) "TITLE", track.title);
			if	(track.artist != NIL) ex_vorbis_comment_add_tag(&vc, (char *) "ARTIST", track.artist);
			if	(track.album  != NIL) ex_vorbis_comment_add_tag(&vc, (char *) "ALBUM", track.album);
			if	(track.track   >   0) ex_vorbis_comment_add_tag(&vc, (char *) "TRACKNUMBER", String(track.track < 10 ? "0" : "").Append(String::FromInt(track.track)));
			if	(track.year    >   0) ex_vorbis_comment_add_tag(&vc, (char *) "DATE", String::FromInt(track.year));
			if	(track.genre  != NIL) ex_vorbis_comment_add_tag(&vc, (char *) "GENRE", track.genre);
			if	(track.label  != NIL) ex_vorbis_comment_add_tag(&vc, (char *) "ORGANIZATION", track.label);
			if	(track.isrc   != NIL) ex_vorbis_comment_add_tag(&vc, (char *) "ISRC", track.isrc);

			if	(track.comment != NIL && !config->replace_comments) ex_vorbis_comment_add_tag(&vc, (char *) "COMMENT", track.comment);
			else if (config->default_comment != NIL)		    ex_vorbis_comment_add_tag(&vc, (char *) "COMMENT", config->default_comment);
		}

		String::SetOutputFormat(prevOutFormat);
	}

	ex_vorbis_analysis_init(&vd, &vi);
	ex_vorbis_block_init(&vd, &vb);

	ex_ogg_stream_init(&os, rand());

	ogg_packet	 header;
	ogg_packet	 header_comm;
	ogg_packet	 header_code;


	ex_vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);

	ex_ogg_stream_packetin(&os, &header); /* automatically placed in its own page */

	ex_ogg_stream_packetin(&os, &header_comm);
	ex_ogg_stream_packetin(&os, &header_code);

	WriteOggPackets(True);

	return true;
}

Bool BoCA::VorbisOut::Deactivate()
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

	return true;
}

Int BoCA::VorbisOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	const Format	&format = track.GetFormat();

	int	 dataLength = 0;
	int	 samples_size = size / (format.bits / 8);

	float	**buffer = ex_vorbis_analysis_buffer(&vd, samples_size / format.channels);

	samplesBuffer.Resize(samples_size);

	for (int i = 0; i < samples_size; i++)
	{
		if	(format.bits ==  8) samplesBuffer[i] = (data[i] - 128) * 256;
		else if (format.bits == 16) samplesBuffer[i] = ((unsigned short *) (unsigned char *) data)[i];
		else if (format.bits == 24) samplesBuffer[i] = (int) (data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256;
		else if (format.bits == 32) samplesBuffer[i] = (int) ((long *) (unsigned char *) data)[i] / 65536;
	}

	if (format.channels == 1)
	{
		for (int j = 0; j < samples_size; j++)
		{
			buffer[0][j] = ((((signed char *) (unsigned short *) samplesBuffer)[j * 2 + 1] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[j * 2 + 0])) / 32768.f;
		}
	}
	else if (format.channels == 2)
	{
		for (int j = 0; j < samples_size / 2; j++)
		{
			buffer[0][j] = ((((signed char *) (unsigned short *) samplesBuffer)[j * 4 + 1] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[j * 4 + 0])) / 32768.f;
			buffer[1][j] = ((((signed char *) (unsigned short *) samplesBuffer)[j * 4 + 3] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[j * 4 + 2])) / 32768.f;
		}
	}

	ex_vorbis_analysis_wrote(&vd, samples_size / format.channels);

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

Int BoCA::VorbisOut::WriteOggPackets(Bool flush)
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

ConfigLayer *BoCA::VorbisOut::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureVorbis();

	return configLayer;
}

Void BoCA::VorbisOut::FreeConfigurationLayer()
{
	if (configLayer != NIL)
	{
		delete configLayer;

		configLayer = NIL;
	}
}
