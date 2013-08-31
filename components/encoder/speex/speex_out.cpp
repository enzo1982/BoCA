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

#include <time.h>
#include <stdlib.h>

#include "speex_out.h"
#include "config.h"
#include "dllinterface.h"

const String &BoCA::SpeexOut::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && speexdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Speex Speech Encoder</name>					\
		    <version>1.0</version>						\
		    <id>speex-out</id>							\
		    <type>encoder</type>						\
		    <format>								\
		      <name>Speex Files</name>						\
		      <extension>spx</extension>					\
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
	LoadSpeexDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
	FreeSpeexDLL();
}

BoCA::SpeexOut::SpeexOut()
{
	configLayer = NIL;

	encoder	    = NIL;

	numPackets  = 0;
	frameSize   = 0;

	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));

	memset(&bits, 0, sizeof(bits));
}

BoCA::SpeexOut::~SpeexOut()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::SpeexOut::Activate()
{
	Config		*config = Config::Get();
	const Format	&format = track.GetFormat();
	const Info	&info = track.GetInfo();

	srand(clock());

	ex_ogg_stream_init(&os, rand());

	Int	 modeID = config->GetIntValue("Speex", "Mode", -1);

	if (modeID == -1)
	{
		/* Automatically select Speex mode
		 * depending on sampling rate.
		 */
		if	(format.rate <= 12500) modeID = SPEEX_MODEID_NB;
		else if	(format.rate <= 25000) modeID = SPEEX_MODEID_WB;
		else			       modeID = SPEEX_MODEID_UWB;
	}

	SpeexHeader	 speex_header;

	ex_speex_init_header(&speex_header, format.rate, format.channels, ex_speex_lib_get_mode(modeID));

	encoder = ex_speex_encoder_init(ex_speex_lib_get_mode(modeID));

	/* Set options
	 */
	spx_int32_t	 vbr = config->GetIntValue("Speex", "VBR", 0);
	spx_int32_t	 abr = config->GetIntValue("Speex", "ABR", -16) * 1000;
	spx_int32_t	 complexity = config->GetIntValue("Speex", "Complexity", 3);

	ex_speex_encoder_ctl(encoder, SPEEX_SET_VBR, &vbr);
	ex_speex_encoder_ctl(encoder, SPEEX_SET_COMPLEXITY, &complexity);

	if (abr > 0) ex_speex_encoder_ctl(encoder, SPEEX_SET_ABR, &abr);

	if (vbr)
	{
		float		 vbrq = config->GetIntValue("Speex", "VBRQuality", 80) / 10;
		spx_int32_t	 vbrmax = config->GetIntValue("Speex", "VBRMaxBitrate", -48) * 1000;

		ex_speex_encoder_ctl(encoder, SPEEX_SET_VBR_QUALITY, &vbrq);

		if (vbrmax > 0) ex_speex_encoder_ctl(encoder, SPEEX_SET_VBR_MAX_BITRATE, &vbrmax);
	}
	else
	{
		spx_int32_t	 quality = config->GetIntValue("Speex", "Quality", 8);
		spx_int32_t	 bitrate = config->GetIntValue("Speex", "Bitrate", -16) * 1000;
		spx_int32_t	 vad = config->GetIntValue("Speex", "VAD", 0);

		if (quality > 0) ex_speex_encoder_ctl(encoder, SPEEX_SET_QUALITY, &quality);
		if (bitrate > 0) ex_speex_encoder_ctl(encoder, SPEEX_SET_BITRATE, &bitrate);

		ex_speex_encoder_ctl(encoder, SPEEX_SET_VAD, &vad);
	}

	if (vbr || abr > 0 || config->GetIntValue("Speex", "VAD", 0))
	{
		spx_int32_t	 dtx = config->GetIntValue("Speex", "DTX", 0);

		ex_speex_encoder_ctl(encoder, SPEEX_SET_DTX, &dtx);
	}

	/* Get frame size
	 */
	spx_int32_t	 frame_size = 0;
	spx_int32_t	 rate = format.rate;

	ex_speex_encoder_ctl(encoder, SPEEX_GET_FRAME_SIZE, &frame_size);
	ex_speex_encoder_ctl(encoder, SPEEX_SET_SAMPLING_RATE, &rate);

	frameSize = frame_size;
	packageSize = frame_size * (format.bits / 8) * format.channels;

	numPackets = 0;

	/* Write Speex header
	 */
	int		 bytes;
	unsigned char	*buffer = (unsigned char *) ex_speex_header_to_packet(&speex_header, &bytes);
	ogg_packet	 header = { buffer, bytes, 1, 0, 0, numPackets++ };

	ex_ogg_stream_packetin(&os, &header);

	free(buffer);

	/* Write Vorbis comment header
	 */
	{
		char	*speexVersion = NIL;

		ex_speex_lib_ctl(SPEEX_LIB_GET_VERSION_STRING, &speexVersion);

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
			tagger->SetVendorString(String("Encoded with Speex ").Append(speexVersion));

			if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableVorbisComment", True)) tagger->RenderBuffer(vcBuffer, track);
			else													   tagger->RenderBuffer(vcBuffer, Track());

			boca.DeleteComponent(tagger);
		}

		ogg_packet	 header_comm = { vcBuffer, vcBuffer.Size(), 0, 0, 0, numPackets++ };

		ex_ogg_stream_packetin(&os, &header_comm);
	}

	WriteOggPackets(True);

	ex_speex_bits_init(&bits);

	return True;
}

Bool BoCA::SpeexOut::Deactivate()
{
	/* Write any remaining Ogg packets.
	 */
	WriteOggPackets(True);

	ex_speex_bits_destroy(&bits);

	ex_speex_encoder_destroy(encoder);

	ex_ogg_stream_clear(&os);

	return True;
}

Int BoCA::SpeexOut::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	const Format	&format = track.GetFormat();

	static Int	 totalSamples = 0;
	Int		 samples = size / format.channels / (format.bits / 8);

	totalSamples += samples;

	samplesBuffer.Resize(frameSize * format.channels);
	samplesBuffer.Zero();

	for (int i = 0; i < samples * format.channels; i++)
	{
		if	(format.bits ==  8) samplesBuffer[i] = (data[i] - 128) * 256;
		else if (format.bits == 16) samplesBuffer[i] = ((spx_int16_t *) (unsigned char *) data)[i];
		else if (format.bits == 24) samplesBuffer[i] = (spx_int16_t) (data[3 * i] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256;
		else if (format.bits == 32) samplesBuffer[i] = (spx_int16_t) ((long *) (unsigned char *) data)[i] / 65536;
	}

	if (format.channels == 2) ex_speex_encode_stereo_int(samplesBuffer, frameSize, &bits);

	ex_speex_encode_int(encoder, samplesBuffer, &bits);
	ex_speex_bits_insert_terminator(&bits); 

	Int	 dataLength = ex_speex_bits_nbytes(&bits);

	dataBuffer.Resize(dataLength);

	dataLength = ex_speex_bits_write(&bits, dataBuffer, dataLength);

	op.packet     = (unsigned char *) (char *) dataBuffer;
	op.bytes      = dataLength;
	op.b_o_s      = 0;
	op.e_o_s      = samples < frameSize ? 1 : 0;
	op.granulepos = totalSamples;
	op.packetno   = numPackets++;

	ex_ogg_stream_packetin(&os, &op);

	dataLength = WriteOggPackets(False);

	ex_speex_bits_reset(&bits);

	return dataLength;
}

Int BoCA::SpeexOut::WriteOggPackets(Bool flush)
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

ConfigLayer *BoCA::SpeexOut::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureSpeex();

	return configLayer;
}
