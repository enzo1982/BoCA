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
#include <stdint.h>

#include "opus.h"
#include "config.h"

using namespace smooth::IO;

const String &BoCA::EncoderOpus::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && opusdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Opus Audio Encoder</name>					\
		    <version>1.0</version>						\
		    <id>opus-enc</id>							\
		    <type>encoder</type>						\
		    <require>resample-dsp</require>					\
		    <format>								\
		      <name>Opus Audio</name>						\
		      <extension>opus</extension>					\
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
	LoadOpusDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
	FreeOpusDLL();
}

namespace BoCA
{
	/* Constants.
	 */
	const Int	 maxPacketSize = 4000;

	/* Opus header definition.
	 */
	struct OpusHeader
	{
		char		 codec_id[8];	  /**< MUST be "OpusHead" */
		uint8_t		 version_id;	  /**< Version number */
		uint8_t		 nb_channels;	  /**< Number of channels */
		uint16_t	 preskip;	  /**< Pre-skip */
		uint32_t	 sample_rate;	  /**< Input sample rate; informational only */
		int16_t		 output_gain;	  /**< Output gain to apply when decoding */
		uint8_t		 channel_mapping; /**< Channel mapping family */

		uint8_t		 nb_streams;	  /**< Stream count */
		uint8_t		 nb_coupled;	  /**< Two-channel stream count */
		uint8_t		 stream_map[255]; /**< Channel mapping */
	};
};

BoCA::EncoderOpus::EncoderOpus()
{
	configLayer = NIL;

	encoder	    = NIL;
	resampler   = NIL;

	frameSize    = 0;

	numPackets   = 0;
	totalSamples = 0;

	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));
}

BoCA::EncoderOpus::~EncoderOpus()
{
	if (configLayer != NIL) Object::DeleteObject(configLayer);
}

Bool BoCA::EncoderOpus::Activate()
{
	Config		*config = Config::Get();
	const Format	&format = track.GetFormat();
	Info		 info = track.GetInfo();

	if (format.channels > 2)
	{
		errorString = "This encoder does not support more than 2 channels!";
		errorState  = True;

		return False;
	}

	frameSize    = Math::Round(48000.0 / (1000000.0 / config->GetIntValue("Opus", "FrameSize", 20000)));
	totalSamples = 0;

	/* Create and init resampler component.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	resampler = (AS::DSPComponent *) boca.CreateComponentByID("resample-dsp");

	if (resampler == NIL)
	{
		errorString = "Could not create resampler component!";
		errorState  = True;

		return False;
	}

	Int	 prevSampleRate = config->GetIntValue("Resample", "Samplerate", 44100);

	config->SetIntValue("Resample", "Samplerate", 48000);

	resampler->SetAudioTrackInfo(track);
	resampler->Activate();

	config->SetIntValue("Resample", "Samplerate", prevSampleRate);

	if (resampler->GetErrorState() == True)
	{
		errorString = resampler->GetErrorString();
		errorState  = resampler->GetErrorState();

		boca.DeleteComponent(resampler);

		return False;
	}

	/* Init Ogg stream.
	 */
	srand(clock());

	ex_ogg_stream_init(&os, rand());

	dataBuffer.Resize(maxPacketSize);

	/* Force 48 kHz as this is Opus' internally used rate.
	 */
	int	 error = 0;

	encoder = ex_opus_encoder_create(48000, format.channels, OPUS_APPLICATION_AUDIO, &error);

	/* Set encoder parameters.
	 */
	if (config->GetIntValue("Opus", "Mode", 0)	!= 0) ex_opus_encoder_ctl(encoder, OPUS_SET_SIGNAL(OPUS_SIGNAL_VOICE + config->GetIntValue("Opus", "Mode", 0) - 1));
	if (config->GetIntValue("Opus", "Bandwidth", 0) != 0) ex_opus_encoder_ctl(encoder, OPUS_SET_BANDWIDTH(OPUS_BANDWIDTH_NARROWBAND + config->GetIntValue("Opus", "Bandwidth", 0) - 1));

	ex_opus_encoder_ctl(encoder, OPUS_SET_BITRATE( config->GetIntValue("Opus", "Bitrate", 128) * 1000));
	ex_opus_encoder_ctl(encoder, OPUS_SET_VBR(config->GetIntValue("Opus", "EnableVBR", True)));
	ex_opus_encoder_ctl(encoder, OPUS_SET_VBR_CONSTRAINT(config->GetIntValue("Opus", "EnableConstrainedVBR", False)));
	ex_opus_encoder_ctl(encoder, OPUS_SET_COMPLEXITY(config->GetIntValue("Opus", "Complexity", 10)));
	ex_opus_encoder_ctl(encoder, OPUS_SET_PACKET_LOSS_PERC(config->GetIntValue("Opus", "PacketLoss", 0)));
	ex_opus_encoder_ctl(encoder, OPUS_SET_DTX(config->GetIntValue("Opus", "EnableDTX", True)));
	ex_opus_encoder_ctl(encoder, OPUS_SET_INBAND_FEC(0));

	numPackets = 0;

	OpusHeader	 setup;

	strncpy(setup.codec_id, "OpusHead", 8);

	setup.version_id	= 1;
	setup.nb_channels	= format.channels;
	setup.preskip		= 0;
	setup.sample_rate	= format.rate;
	setup.output_gain	= 0;
	setup.channel_mapping	= 0;

	ogg_packet	 header = { (unsigned char *) &setup, 19, 1, 0, 0, numPackets };

	ex_ogg_stream_packetin(&os, &header);

	/* Write Vorbis comment header
	 */
	{
		Buffer<unsigned char>	 vcBuffer;

		/* Remove ReplayGain information as per Opus comment spec.
		 */
		info.track_gain = NIL;
		info.track_peak = NIL;
		info.album_gain = NIL;
		info.album_peak = NIL;

		/* Render actual Vorbis comment tag.
		 *
		 * An empty tag containing only the vendor string
		 * is rendered if Vorbis comments are disabled.
		 */
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			const char	*opusVersion = ex_opus_get_version_string();

			tagger->SetVendorString(String(opusVersion).Append("\n"));

			if ((info.artist != NIL || info.title != NIL) && config->GetIntValue("Tags", "EnableVorbisComment", True)) tagger->RenderBuffer(vcBuffer, track);
			else													   tagger->RenderBuffer(vcBuffer, Track());

			boca.DeleteComponent(tagger);
		}

		vcBuffer.Resize(vcBuffer.Size() + 8);

		memmove(vcBuffer + 8, vcBuffer, vcBuffer.Size() - 8);
		memcpy(vcBuffer, "OpusTags", 8);

		ogg_packet	 header_comm = { vcBuffer, vcBuffer.Size(), 0, 0, 0, numPackets++ };

		ex_ogg_stream_packetin(&os, &header_comm);
	}

	WriteOggPackets(True);

	return True;
}

Bool BoCA::EncoderOpus::Deactivate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	Buffer<UnsignedByte>	 data;

	Int	 size = resampler->Flush(data);

	/* Convert samples to 16 bit.
	 */
	const Format	&format	 = track.GetFormat();
	Int		 samples = size / format.channels / (format.bits / 8);

	if (format.bits != 16)
	{
		samplesBuffer.Resize(samples * format.channels);

		for (Int i = 0; i < samples * format.channels; i++)
		{
			if	(format.bits ==  8				) samplesBuffer[i] =	   (				data [i] - 128) * 256;
			else if (format.bits == 32				) samplesBuffer[i] = (int) (((long *) (unsigned char *) data)[i]	/ 65536);

			else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[i] = (int) ((data[3 * i    ] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256);
			else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[i] = (int) ((data[3 * i + 2] + 256 * data[3 * i + 1] + 65536 * data[3 * i    ] - (data[3 * i    ] & 128 ? 16777216 : 0)) / 256);
		}
	}
	else
	{
		samplesBuffer.Resize(samples * format.channels);

		memcpy((signed short *) samplesBuffer, (unsigned char *) data, size);
	}

	EncodeFrames(samplesBuffer, dataBuffer, True);

	/* Write any remaining Ogg packets.
	 */
	WriteOggPackets(True);

	ex_opus_encoder_destroy(encoder);

	ex_ogg_stream_clear(&os);

	/* Clean up resampler component.
	 */
	AS::Registry	&boca = AS::Registry::Get();

	boca.DeleteComponent(resampler);

	return True;
}

Int BoCA::EncoderOpus::WriteData(Buffer<UnsignedByte> &data, Int size)
{
	static Endianness	 endianness = CPU().GetEndianness();

	size = resampler->TransformData(data, size);

	/* Convert samples to 16 bit.
	 */
	const Format	&format	 = track.GetFormat();
	Int		 samples = size / format.channels / (format.bits / 8);

	if (format.bits != 16)
	{
		samplesBuffer.Resize(samples * format.channels);

		for (Int i = 0; i < samples * format.channels; i++)
		{
			if	(format.bits ==  8				) samplesBuffer[i] =	   (				data [i] - 128) * 256;
			else if (format.bits == 32				) samplesBuffer[i] = (int) (((long *) (unsigned char *) data)[i]	/ 65536);

			else if (format.bits == 24 && endianness == EndianLittle) samplesBuffer[i] = (int) ((data[3 * i    ] + 256 * data[3 * i + 1] + 65536 * data[3 * i + 2] - (data[3 * i + 2] & 128 ? 16777216 : 0)) / 256);
			else if (format.bits == 24 && endianness == EndianBig	) samplesBuffer[i] = (int) ((data[3 * i + 2] + 256 * data[3 * i + 1] + 65536 * data[3 * i    ] - (data[3 * i    ] & 128 ? 16777216 : 0)) / 256);
		}
	}
	else
	{
		samplesBuffer.Resize(samples * format.channels);

		memcpy((signed short *) samplesBuffer, (unsigned char *) data, size);
	}

	return EncodeFrames(samplesBuffer, dataBuffer, False);
}

Int BoCA::EncoderOpus::EncodeFrames(const Buffer<signed short> &samplesBuffer, Buffer<unsigned char> &dataBuffer, Bool flush)
{
	backBuffer.Resize(backBuffer.Size() + samplesBuffer.Size());

	memcpy(((signed short *) backBuffer) + backBuffer.Size() - samplesBuffer.Size(), (signed short *) samplesBuffer, sizeof(short) * samplesBuffer.Size());

	const Format	&format = track.GetFormat();

	Int	 nullSamples = 0;

	if (flush)
	{
		nullSamples = (backBuffer.Size() / format.channels) % frameSize;

		backBuffer.Resize(backBuffer.Size() + nullSamples * format.channels);

		memset(((signed short *) backBuffer) + backBuffer.Size() - nullSamples * format.channels, 0, sizeof(short) * nullSamples * format.channels);
	}

	while (backBuffer.Size() >= frameSize * format.channels)
	{
		Int	 dataLength = ex_opus_encode(encoder, backBuffer, frameSize, dataBuffer, dataBuffer.Size());

		totalSamples += frameSize;

		op.packet     = dataBuffer;
		op.bytes      = dataLength;
		op.b_o_s      = 0;
		op.e_o_s      = (flush && backBuffer.Size() <= frameSize * format.channels) ? 1 : 0;
		op.granulepos = (flush && backBuffer.Size() <= frameSize * format.channels) ? totalSamples - nullSamples : totalSamples;
		op.packetno   = numPackets++;

		ex_ogg_stream_packetin(&os, &op);

		memmove((signed short *) backBuffer, ((signed short *) backBuffer) + frameSize * format.channels, sizeof(short) * (backBuffer.Size() - frameSize * format.channels));

		backBuffer.Resize(backBuffer.Size() - frameSize * format.channels);
	}

	return WriteOggPackets(flush);
}

Int BoCA::EncoderOpus::WriteOggPackets(Bool flush)
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

String BoCA::EncoderOpus::GetOutputFileExtension()
{
	Config	*config = Config::Get();

	switch (config->GetIntValue("Opus", "FileExtension", 0))
	{
		default:
		case  0: return "opus";
		case  1: return "oga";
	}
}

ConfigLayer *BoCA::EncoderOpus::GetConfigurationLayer()
{
	if (configLayer == NIL) configLayer = new ConfigureOpus();

	return configLayer;
}
