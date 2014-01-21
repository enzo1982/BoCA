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

#include "speex.h"
#include "config.h"

const String &BoCA::EncoderSpeex::GetComponentSpecs()
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
		    <id>speex-enc</id>							\
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

BoCA::EncoderSpeex::EncoderSpeex()
{
	configLayer = NIL;

	encoder	    = NIL;

	frameSize    = 0;

	numPackets   = 0;
	totalSamples = 0;

	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));

	memset(&bits, 0, sizeof(bits));
}

BoCA::EncoderSpeex::~EncoderSpeex()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderSpeex::Activate()
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
	spx_int32_t	 rate = format.rate;

	ex_speex_encoder_ctl(encoder, SPEEX_SET_SAMPLING_RATE, &rate);
	ex_speex_encoder_ctl(encoder, SPEEX_GET_FRAME_SIZE, &frameSize);
	ex_speex_encoder_ctl(encoder, SPEEX_GET_LOOKAHEAD, &lookAhead);

	totalSamples = 0;
	numPackets   = 0;

	/* Write Speex header
	 */
	speex_header.frames_per_packet = 1;
	speex_header.vbr	       = vbr;

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

Bool BoCA::EncoderSpeex::Deactivate()
{
	/* Output remaining samples to encoder.
	 */
	samplesBuffer.Resize(0);

	EncodeFrames(samplesBuffer, dataBuffer, True);

	/* Write any remaining Ogg packets.
	 */
	WriteOggPackets(True);

	ex_speex_bits_destroy(&bits);

	ex_speex_encoder_destroy(encoder);

	ex_ogg_stream_clear(&os);

	return True;
}

Int BoCA::EncoderSpeex::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	/* Convert samples to 16 bit.
	 */
	const Format	&format	 = track.GetFormat();
	Int		 samples = size / format.channels / (format.bits / 8);

	samplesBuffer.Resize(samples * format.channels);

	for (Int i = 0; i < samples * format.channels; i++)
	{
		if	(format.bits ==  8				) samplesBuffer[i] =		   (				 data [i] - 128) * 256;
		else if (format.bits == 16				) samplesBuffer[i] = (spx_int16_t)  ((short *) (unsigned char *) data)[i];
		else if (format.bits == 32				) samplesBuffer[i] = (spx_int16_t) (((long *)  (unsigned char *) data)[i]	 / 65536);

		else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[i] = (spx_int16_t) ((data[3 * i + 2] << 24 | data[3 * i + 1] << 16 | data[3 * i    ] << 8) / 65536);
		else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[i] = (spx_int16_t) ((data[3 * i    ] << 24 | data[3 * i + 1] << 16 | data[3 * i + 2] << 8) / 65536);
	}

	/* Output samples to encoder.
	 */
	return EncodeFrames(samplesBuffer, dataBuffer, False);
}

Int BoCA::EncoderSpeex::EncodeFrames(const Buffer<signed short> &samplesBuffer, Buffer<unsigned char> &dataBuffer, Bool flush)
{
	backBuffer.Resize(backBuffer.Size() + samplesBuffer.Size());

	memcpy(((signed short *) backBuffer) + backBuffer.Size() - samplesBuffer.Size(), (signed short *) samplesBuffer, sizeof(short) * samplesBuffer.Size());

	const Format	&format = track.GetFormat();

	Int	 nullSamples = 0;

	if (flush)
	{
		nullSamples = frameSize;

		if ((backBuffer.Size() / format.channels) % frameSize > 0) nullSamples += frameSize - (backBuffer.Size() / format.channels) % frameSize;

		backBuffer.Resize(backBuffer.Size() + nullSamples * format.channels);

		memset(((signed short *) backBuffer) + backBuffer.Size() - nullSamples * format.channels, 0, sizeof(short) * nullSamples * format.channels);
	}

	while (backBuffer.Size() >= frameSize * format.channels)
	{
		if (format.channels == 2) ex_speex_encode_stereo_int(backBuffer, frameSize, &bits);

		ex_speex_encode_int(encoder, backBuffer, &bits);
		ex_speex_bits_insert_terminator(&bits);

		Int	 dataLength = ex_speex_bits_nbytes(&bits);

		dataBuffer.Resize(dataLength);

		dataLength = ex_speex_bits_write(&bits, (char *) (unsigned char *) dataBuffer, dataLength);

		ex_speex_bits_reset(&bits);

		totalSamples += frameSize;

		op.packet     = dataBuffer;
		op.bytes      = dataLength;
		op.b_o_s      = 0;
		op.e_o_s      =  (flush && backBuffer.Size() <=     frameSize * format.channels) ? 1 : 0;
		op.granulepos =  (flush && backBuffer.Size() <=     frameSize * format.channels) ? totalSamples		    - nullSamples : 
				((flush && backBuffer.Size() <= 2 * frameSize * format.channels) ? totalSamples + frameSize - nullSamples - lookAhead : totalSamples - lookAhead);
		op.packetno   = numPackets++;

		ex_ogg_stream_packetin(&os, &op);

		memmove((signed short *) backBuffer, ((signed short *) backBuffer) + frameSize * format.channels, sizeof(short) * (backBuffer.Size() - frameSize * format.channels));

		backBuffer.Resize(backBuffer.Size() - frameSize * format.channels);
	}

	return WriteOggPackets(flush);
}

Int BoCA::EncoderSpeex::WriteOggPackets(Bool flush)
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

ConfigLayer *BoCA::EncoderSpeex::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureSpeex();

	return configLayer;
}
