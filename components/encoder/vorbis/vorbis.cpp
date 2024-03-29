 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2021 Robert Kausch <robert.kausch@freac.org>
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

#include "vorbis.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderVorbis::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && vorbisdll != NIL && vorbisencdll != NIL)
	{
		componentSpecs = "									\
													\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>						\
		  <component>										\
		    <name>Ogg Vorbis Encoder</name>							\
		    <version>1.0</version>								\
		    <id>vorbis-enc</id>									\
		    <type>encoder</type>								\
		    <format>										\
		      <name>Ogg Vorbis Audio</name>							\
		      <extension>ogg</extension>							\
		      <extension>oga</extension>							\
		      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>			\
		    </format>										\
		    <input float=\"true\" rate=\"8000-192000\"/>					\
		    <parameters>									\
		      <range name=\"VBR quality\" argument=\"-q %VALUE\" default=\"60\">		\
			<min alias=\"worst\">0</min>							\
			<max alias=\"best\">100</max>							\
		      </range>										\
		      <range name=\"ABR target bitrate\" argument=\"-b %VALUE\" default=\"192\">	\
			<min alias=\"min\">45</min>							\
			<max alias=\"max\">500</max>							\
		      </range>										\
		    </parameters>									\
		  </component>										\
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
	config	    = NIL;

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
	if (config != NIL) Config::Free(config);

	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderVorbis::Activate()
{
	/* Get configuration.
	 */
	config = Config::Copy(GetConfiguration());

	ConvertArguments(config);

	Int	 mode	 = config->GetIntValue(ConfigureVorbis::ConfigID, "Mode", 0);
	Int	 quality = config->GetIntValue(ConfigureVorbis::ConfigID, "Quality", 60);

	if (ex_vorbis_version_string != NIL && String(ex_vorbis_version_string()).Contains("aoTuV")) quality = Math::Max(-20, Math::Min(quality, 100));
	else											     quality = Math::Max(-10, Math::Min(quality, 100));

	Bool	 setBitrate    = config->GetIntValue(ConfigureVorbis::ConfigID, "SetBitrate", True);
	Bool	 setMinBitrate = config->GetIntValue(ConfigureVorbis::ConfigID, "SetMinBitrate", False);
	Bool	 setMaxBitrate = config->GetIntValue(ConfigureVorbis::ConfigID, "SetMaxBitrate", False);

	Int	 bitrate       = config->GetIntValue(ConfigureVorbis::ConfigID, "Bitrate", 192);
	Int	 minBitrate    = config->GetIntValue(ConfigureVorbis::ConfigID, "MinBitrate", 32);
	Int	 maxBitrate    = config->GetIntValue(ConfigureVorbis::ConfigID, "MaxBitrate", 320);

	/* Init Ogg stream.
	 */
	Math::RandomSeed();

	ex_ogg_stream_init(&os, Math::Random());

	/* Create and configure Vorbis encoder.
	 */
	const Format	&format = track.GetFormat();
	const Info	&info	= track.GetInfo();

	int	 error = -1;

	ex_vorbis_info_init(&vi);

	switch (mode)
	{
		case 0:
			error = ex_vorbis_encode_init_vbr(&vi, format.channels, format.rate, ((double) quality) / 100);
			break;
		case 1:
			error = ex_vorbis_encode_init(&vi, format.channels, format.rate, setMaxBitrate ? maxBitrate * 1000 : -1,
											 setBitrate    ? bitrate    * 1000 : -1,
											 setMinBitrate ? minBitrate * 1000 : -1);
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

	ogg_packet	 header;
	ogg_packet	 header_comm;
	ogg_packet	 header_code;

	ex_vorbis_analysis_headerout(&vd, &vc, &header, &header_comm, &header_code);

	ex_ogg_stream_packetin(&os, &header); /* automatically placed in its own page */

	/* Write Vorbis Comment header
	 */
	{
		/* Read vendor string.
		 */
		InStream	 in(STREAM_BUFFER, header_comm.packet + 7, header_comm.bytes - 7);
		String		 vendor = in.InputString(in.InputNumber(4));

		Buffer<unsigned char>	 vcBuffer;

		/* Render actual Vorbis Comment tag.
		 *
		 * An empty tag containing only the vendor string
		 * is rendered if Vorbis Comment tags are disabled.
		 */
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			tagger->SetConfiguration(config);
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
	/* Finish conversion and write any remaining Ogg packets.
	 */
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

	ex_vorbis_block_clear(&vb);
	ex_vorbis_dsp_clear(&vd);
	ex_vorbis_comment_clear(&vc);
	ex_vorbis_info_clear(&vi);

	ex_ogg_stream_clear(&os);

	/* Fix chapter marks in Vorbis Comments.
	 */
	if (config->GetIntValue("Tags", "EnableVorbisComment", True) && track.tracks.Length() > 0 && config->GetIntValue("Tags", "WriteChapters", True))
	{
		driver->Close();

		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			tagger->UpdateStreamInfo(track.outputFile, track);

			boca.DeleteComponent(tagger);
		}
	}

	return True;
}

Int BoCA::EncoderVorbis::WriteData(Buffer<UnsignedByte> &data)
{
	const Format	&format	= track.GetFormat();

	/* Change to Vorbis channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Default_3_0, Channel::Vorbis_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_0, Channel::Vorbis_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Default_5_1, Channel::Vorbis_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Default_6_1, Channel::Vorbis_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Default_7_1, Channel::Vorbis_7_1);

	/* Write samples to analysis buffer.
	 */
	Int	 numSamples = data.Size() / sizeof(float) / format.channels;
	float	*samples    = (float *) (UnsignedByte *) data;
	float  **buffer	    = ex_vorbis_analysis_buffer(&vd, numSamples);

	for (Int c = 0; c < format.channels; c++)
	{
		for (Int i = 0; i < numSamples; i++) buffer[c][i] = samples[i * format.channels + c];
	}

	ex_vorbis_analysis_wrote(&vd, numSamples);

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

String BoCA::EncoderVorbis::GetOutputFileExtension() const
{
	const Config	*config = GetConfiguration();

	switch (config->GetIntValue(ConfigureVorbis::ConfigID, "FileExtension", 0))
	{
		default:
		case  0: return "ogg";
		case  1: return "oga";
	}
}

Bool BoCA::EncoderVorbis::ConvertArguments(Config *config)
{
	if (!config->GetIntValue("Settings", "EnableConsole", False)) return False;

	static const String	 encoderID = "vorbis-enc";

	/* Set default values.
	 */
	if (!config->GetIntValue("Settings", "UserSpecifiedConfig", False))
	{
		config->SetIntValue(ConfigureVorbis::ConfigID, "Mode", 0);

		config->SetIntValue(ConfigureVorbis::ConfigID, "Quality", 60);
		config->SetIntValue(ConfigureVorbis::ConfigID, "Bitrate", 192);
	}

	/* Get command line settings.
	 */
	Bool	 useABR	 = config->GetIntValue(encoderID, "Set ABR target bitrate", config->GetIntValue(ConfigureVorbis::ConfigID, "Mode", 0) == 1);

	Int	 quality = config->GetIntValue(ConfigureVorbis::ConfigID, "Quality", 60);
	Int	 bitrate = config->GetIntValue(ConfigureVorbis::ConfigID, "Bitrate", 192);

	if (config->GetIntValue(encoderID, "Set VBR quality", False))	     quality = config->GetIntValue(encoderID, "VBR quality", quality);
	if (config->GetIntValue(encoderID, "Set ABR target bitrate", False)) bitrate = config->GetIntValue(encoderID, "ABR target bitrate", bitrate);

	/* Set configuration values.
	 */
	config->SetIntValue(ConfigureVorbis::ConfigID, "Mode", useABR ? 1 : 0);

	config->SetIntValue(ConfigureVorbis::ConfigID, "Quality", Math::Max(0, Math::Min(100, quality)));
	config->SetIntValue(ConfigureVorbis::ConfigID, "Bitrate", Math::Max(45, Math::Min(500, bitrate)));

	return True;
}

ConfigLayer *BoCA::EncoderVorbis::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureVorbis();

	return configLayer;
}
