 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2015 Robert Kausch <robert.kausch@freac.org>
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
	const Info	&info	= track.GetInfo();

	const Config	*config = GetConfiguration();

	srand(clock());

	int	 error = -1;

	ex_vorbis_info_init(&vi);

	switch (config->GetIntValue("Vorbis", "Mode", 0))
	{
		case 0:
			error = ex_vorbis_encode_init_vbr(&vi, format.channels, format.rate, ((double) config->GetIntValue("Vorbis", "Quality", 60)) / 100);
			break;
		case 1:
			error = ex_vorbis_encode_init(&vi, format.channels, format.rate, config->GetIntValue("Vorbis", "SetMinBitrate", False) ? config->GetIntValue("Vorbis", "MinBitrate",  32) * 1000 : -1,
											 config->GetIntValue("Vorbis", "SetBitrate",    True)  ? config->GetIntValue("Vorbis", "Bitrate",    192) * 1000 : -1,
											 config->GetIntValue("Vorbis", "SetMaxBitrate", False) ? config->GetIntValue("Vorbis", "MaxBitrate", 320) * 1000 : -1);
			break;
	}

	if (error != 0)
	{
		errorString = "Could not initialize Vorbis encoder! Please check the configuration!";
		errorState  = True;

		ex_vorbis_info_clear(&vi);

		return False;
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
			tagger->SetConfiguration(GetConfiguration());
			tagger->SetVendorString(vendor);

			if (config->GetIntValue("Tags", "EnableVorbisComment", True) && (info.HasBasicInfo() || (track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True)))) tagger->RenderBuffer(vcBuffer, track);
			else																					    tagger->RenderBuffer(vcBuffer, Track());

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

	/* Fix chapter marks in Vorbis Comments.
	 */
	FixChapterMarks();

	return True;
}

Int BoCA::EncoderVorbis::WriteData(Buffer<UnsignedByte> &data)
{
	static Endianness	 endianness = CPU().GetEndianness();

	const Format	&format	= track.GetFormat();

	/* Change to Vorbis channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Default_3_0, Channel::Vorbis_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_0, Channel::Vorbis_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::Vorbis_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Default_6_1, Channel::Vorbis_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Default_7_1, Channel::Vorbis_7_1);

	/* Convert samples to 16 bit.
	 */
	Int	 samples_size = data.Size() / (format.bits / 8);

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

	for (Int i = 0; i < samples_size / format.channels; i++)
	{
		for (Int c = 0; c < format.channels; c++)
		{
			if (endianness == EndianLittle) { buffer[c][i] = ((((signed char *) (unsigned short *) samplesBuffer)[i * 2 * format.channels + 2 * c + 1] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[i * 2 * format.channels + 2 * c + 0])) / 32768.f; }
			else				{ buffer[c][i] = ((((signed char *) (unsigned short *) samplesBuffer)[i * 2 * format.channels + 2 * c + 0] << 8) | (0x00ff & ((signed char *) (unsigned short *) samplesBuffer)[i * 2 * format.channels + 2 * c + 1])) / 32768.f; }
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

Bool BoCA::EncoderVorbis::FixChapterMarks()
{
	if (track.tracks.Length() == 0 || !GetConfiguration()->GetIntValue("Tags", "WriteChapters", True)) return True;

	driver->Seek(0);

	/* Skip first Ogg page and read second into buffer.
	 */
	Buffer<UnsignedByte>	 buffer;
	Int			 position;
	ogg_page		 og;

	for (Int i = 0; i < 2; i++)
	{
		driver->Seek(driver->GetPos() + 26);

		Int		 dataSize    = 0;
		UnsignedByte	 segments    = 0;
		UnsignedByte	 segmentSize = 0;

		driver->ReadData(&segments, 1);

		for (Int i = 0; i < segments; i++) { driver->ReadData(&segmentSize, 1); dataSize += segmentSize; }

		buffer.Resize(27 + segments + dataSize);
		position = driver->GetPos() - segments - 27;

		driver->Seek(position);
		driver->ReadData(buffer, buffer.Size());

		og.header     = buffer;
		og.header_len = 27 + segments;
		og.body	      = buffer + og.header_len;
		og.body_len   = dataSize;
	}

	/* Update chapter marks.
	 */
	if (buffer.Size() > 0)
	{
		Int64	 offset = 0;

		for (Int i = 0; i < track.tracks.Length(); i++)
		{
			const Track	&chapterTrack  = track.tracks.GetNth(i);
			const Format	&chapterFormat = chapterTrack.GetFormat();

			for (Int b = 0; b < buffer.Size() - 23; b++)
			{
				if (buffer[b + 0] != 'C' || buffer[b + 1] != 'H' || buffer[b + 2] != 'A' || buffer[b +  3] != 'P' ||
				    buffer[b + 4] != 'T' || buffer[b + 5] != 'E' || buffer[b + 6] != 'R' || buffer[b + 10] != '=') continue;

				String	 id;

				id[0] = buffer[b + 7];
				id[1] = buffer[b + 8];
				id[2] = buffer[b + 9];

				if (id.ToInt() != i + 1) continue;

				String	 value	= String(offset / chapterFormat.rate / 60 / 60 < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate / 60 / 60)).Append(":")
						 .Append(offset / chapterFormat.rate / 60 % 60 < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate / 60 % 60)).Append(":")
						 .Append(offset / chapterFormat.rate % 60      < 10 ? "0" : "").Append(String::FromInt(offset / chapterFormat.rate % 60)).Append(".")
						 .Append(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) < 100 ?
							(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate) <  10 ?  "00" : "0") : "").Append(String::FromInt(Math::Round(offset % chapterFormat.rate * 1000.0 / chapterFormat.rate)));

				for (Int p = 0; p < 12; p++) buffer[b + 11 + p] = value[p];

				break;
			}

			if	(chapterTrack.length	   >= 0) offset += chapterTrack.length;
			else if (chapterTrack.approxLength >= 0) offset += chapterTrack.approxLength;
		}

		/* Write page back to file.
		 */
		ex_ogg_page_checksum_set(&og);

		driver->Seek(position);
		driver->WriteData(buffer, buffer.Size());
	}

	driver->Seek(driver->GetSize());

	return True;
}

String BoCA::EncoderVorbis::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

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
