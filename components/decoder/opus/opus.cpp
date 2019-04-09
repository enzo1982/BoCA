 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2019 Robert Kausch <robert.kausch@freac.org>
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

#include <stdint.h>

#include "opus.h"

using namespace smooth::IO;

const String &BoCA::DecoderOpus::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && opusdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Opus Audio Decoder %VERSION%</name>				\
		    <version>1.0</version>						\
		    <id>opus-dec</id>							\
		    <type>decoder</type>						\
		    <format>								\
		      <name>Opus Audio</name>						\
		      <extension>opus</extension>					\
		      <extension>oga</extension>					\
		      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>	\
		    </format>								\
		  </component>								\
											\
		";

		componentSpecs.Replace("%VERSION%", String("v").Append(String(ex_opus_get_version_string()).Replace("libopus ", NIL)));
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
	const Int	 maxFrameSize  = 5760;

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

Bool BoCA::DecoderOpus::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (in.InputString(4) != "OggS") return False;

	in.Seek(0);

	ogg_sync_state		 oy;
	ogg_stream_state	 os;
	ogg_page		 og;
	ogg_packet		 op;

	ex_ogg_sync_init(&oy);

	Bool	 result	     = False;
	Bool	 initialized = False;

	do
	{
		Int	 size	= Math::Min((Int64) 4096, in.Size() - in.GetPos());
		char	*buffer	= ex_ogg_sync_buffer(&oy, size);

		in.InputData(buffer, size);

		ex_ogg_sync_wrote(&oy, size);

		if (ex_ogg_sync_pageout(&oy, &og) == 1)
		{
			ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

			initialized = True;

			ex_ogg_stream_pagein(&os, &og);

			if (ex_ogg_stream_packetout(&os, &op) == 1)
			{
				if (op.packet[0] == 'O' && op.packet[1] == 'p' && op.packet[2] == 'u' && op.packet[3] == 's' && op.packet[4] == 'H' && op.packet[5] == 'e' && op.packet[6] == 'a' && op.packet[7] == 'd')
				{
					/* Check Opus version and number of channels.
					 */
					OpusHeader	*setup = (OpusHeader *) op.packet;

					if (setup->version_id >> 4 == 0 && setup->channel_mapping <= 1 && setup->nb_channels <= 8) result = True;
				}

				break;
			}
		}
	}
	while (in.GetPos() < in.Size());

	if (initialized) ex_ogg_stream_clear(&os);

	ex_ogg_sync_clear(&oy);

	return result;
}

Error BoCA::DecoderOpus::GetStreamInfo(const String &streamURI, Track &track)
{
	static Endianness	 endianness = CPU().GetEndianness();

	InStream	 in(STREAM_FILE, streamURI, IS_READ);
	Format		 format;

	format.bits  = 16;
	format.rate  = 48000;

	track.fileSize = in.Size();

	ogg_sync_state		 oy;
	ogg_stream_state	 os;
	ogg_page		 og;
	ogg_packet		 op;

	ex_ogg_sync_init(&oy);

	/* Get stream format.
	 */
	OpusHeader		*setup = NIL;
	Buffer<UnsignedByte>	 comments;

	Bool	 initialized = False;
	Bool	 done	     = False;
	Int	 packetNum   = 0;

	do
	{
		Int	 size	= Math::Min((Int64) 4096, track.fileSize - in.GetPos());
		char	*buffer	= ex_ogg_sync_buffer(&oy, size);

		in.InputData(buffer, size);

		ex_ogg_sync_wrote(&oy, size);

		while (!done && ex_ogg_sync_pageout(&oy, &og) == 1)
		{
			if (!initialized)
			{
				ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

				initialized = True;
			}

			ex_ogg_stream_pagein(&os, &og);

			while (!done && ex_ogg_stream_packetout(&os, &op) == 1)
			{
				/* Found header packet.
				 */
				if (packetNum == 0)
				{
					setup = (OpusHeader *) op.packet;

					if (endianness != EndianLittle)
					{
						BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup->preskip, sizeof(setup->preskip));
						BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup->sample_rate, sizeof(setup->sample_rate));
						BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup->output_gain, sizeof(setup->output_gain));
					}

					format.channels = setup->nb_channels;

					if (setup->sample_rate != 0)
					{
						if	(setup->sample_rate <=  8000) format.rate =  8000;
						else if	(setup->sample_rate <= 12000) format.rate = 12000;
						else if	(setup->sample_rate <= 16000) format.rate = 16000;
						else if	(setup->sample_rate <= 24000) format.rate = 24000;
						else				      format.rate = 48000;
					}

					preSkip = setup->preskip;

					track.length = -1;
				}

				/* Found Vorbis Comment packet.
				 */
				if (packetNum == 1)
				{
					if (op.packet[0] == 'O' && op.packet[1] == 'p' && op.packet[2] == 'u' && op.packet[3] == 's' && op.packet[4] == 'T' && op.packet[5] == 'a' && op.packet[6] == 'g' && op.packet[7] == 's')
					{
						comments.Resize(op.bytes - 8);

						memcpy(comments, op.packet + 8, op.bytes - 8);
					}
				}

				/* Found audio packet.
				 */
				if (packetNum == 3)
				{
					int		 error	    = 0;
					unsigned char	 mapping[2] = { 0, 1 };

					OpusMSDecoder	*decoder    = NIL;

					if (setup->channel_mapping == 0) decoder = ex_opus_multistream_decoder_create(format.rate, setup->nb_channels, 1,		  setup->nb_channels - 1, mapping,	     &error);
					else				 decoder = ex_opus_multistream_decoder_create(format.rate, setup->nb_channels, setup->nb_streams, setup->nb_coupled,	  setup->stream_map, &error);

					if (error == 0)
					{
						Buffer<signed short>	 samples(maxFrameSize * format.channels);

						track.approxLength = track.fileSize / (op.bytes * 1.05) * ex_opus_multistream_decode(decoder, op.packet, op.bytes, samples, maxFrameSize, 0);

						ex_opus_multistream_decoder_destroy(decoder);
					}
				}

				/* Done if we reached packet three.
				 */
				if (packetNum >= 3) done = True;

				packetNum++;
			}
		}

		if (done) break;
	}
	while (in.GetPos() < in.Size());

	track.SetFormat(format);

	/* Find real length of stream.
	 */
	ex_ogg_sync_reset(&oy);

	Int	 size   = Math::Min(in.Size(), Int64(65536));
	char    *buffer = ex_ogg_sync_buffer(&oy, size);

	in.Seek(in.Size() - size);
	in.InputData(buffer, size);

	ex_ogg_sync_wrote(&oy, size);

	while (true)
	{
		Int	 seek = ex_ogg_sync_pageseek(&oy, &og);

		if (seek == 0) break;
		if (seek <  0 || ex_ogg_page_serialno(&og) != os.serialno) continue;

		track.length = (ex_ogg_page_granulepos(&og) - preSkip) / (48000 / format.rate);

		if (ex_ogg_page_eos(&og)) break;
	}

	/* Read tags.
	 */
	if (comments.Size() > 0)
	{
		AS::Registry		&boca = AS::Registry::Get();
		AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

		if (tagger != NIL)
		{
			tagger->SetConfiguration(GetConfiguration());
			tagger->ParseBuffer(comments, track);

			boca.DeleteComponent(tagger);
		}
	}

	/* Clean up.
	 */
	if (initialized) ex_ogg_stream_clear(&os);

	ex_ogg_sync_clear(&oy);

	return Success();
}

BoCA::DecoderOpus::DecoderOpus()
{
	decoder	    = NIL;

	sampleRate  = 48000;

	preSkip	    = 0;
	preSkipLeft = 0;

	skipSamples = 0;

	memset(&oy, 0, sizeof(oy));
	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));
}

BoCA::DecoderOpus::~DecoderOpus()
{
}

Bool BoCA::DecoderOpus::Activate()
{
	static Endianness	 endianness = CPU().GetEndianness();

	ex_ogg_sync_init(&oy);

	Bool	 initialized = False;
	Bool	 done = False;
	Int	 packetNum = 0;

	while (!done)
	{
		Int	 size	= 4096;
		char	*buffer = ex_ogg_sync_buffer(&oy, size);

		size = driver->ReadData((unsigned char *) buffer, 4096);

		inBytes += size;

		ex_ogg_sync_wrote(&oy, size);

		while (!done && ex_ogg_sync_pageout(&oy, &og) == 1)
		{
			if (!initialized)
			{
				ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

				initialized = True;
			}

			ex_ogg_stream_pagein(&os, &og);

			while (!done && ex_ogg_stream_packetout(&os, &op) == 1)
			{
				if (packetNum == 0)
				{
					OpusHeader	*setup = (OpusHeader *) op.packet;

					if (endianness != EndianLittle)
					{
						BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup->preskip, sizeof(setup->preskip));
						BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup->sample_rate, sizeof(setup->sample_rate));
						BoCA::Utilities::SwitchByteOrder((UnsignedByte *) &setup->output_gain, sizeof(setup->output_gain));
					}

					if (setup->sample_rate != 0)
					{
						if	(setup->sample_rate <=  8000) sampleRate =  8000;
						else if	(setup->sample_rate <= 12000) sampleRate = 12000;
						else if	(setup->sample_rate <= 16000) sampleRate = 16000;
						else if	(setup->sample_rate <= 24000) sampleRate = 24000;
						else				      sampleRate = 48000;
					}

					int		 error	    = 0;
					unsigned char	 mapping[2] = { 0, 1 };

					if (setup->channel_mapping == 0) decoder = ex_opus_multistream_decoder_create(sampleRate, setup->nb_channels, 1,		 setup->nb_channels - 1, mapping,	    &error);
					else				 decoder = ex_opus_multistream_decoder_create(sampleRate, setup->nb_channels, setup->nb_streams, setup->nb_coupled,	 setup->stream_map, &error);

					preSkip	    = setup->preskip / (48000 / sampleRate);
					preSkipLeft = setup->preskip / (48000 / sampleRate);
				}

				if (packetNum >= 1) done = True;

				packetNum++;
			}
		}
	}

	return True;
}

Bool BoCA::DecoderOpus::Deactivate()
{
	ex_ogg_stream_clear(&os);

	ex_opus_multistream_decoder_destroy(decoder);

	ex_ogg_sync_clear(&oy);

	return True;
}

Bool BoCA::DecoderOpus::Seek(Int64 samplePosition)
{
	while (ex_ogg_page_granulepos(&og) / (48000 / sampleRate) - preSkip <= samplePosition || ex_ogg_page_serialno(&og) != os.serialno)
	{
		skipSamples = preSkip + samplePosition - ex_ogg_page_granulepos(&og) / (48000 / sampleRate);

		while (ex_ogg_sync_pageseek(&oy, &og) == 0)
		{
			char	*buffer = ex_ogg_sync_buffer(&oy, 131072);
			Int	 size	= driver->ReadData((unsigned char *) buffer, 131072);

			inBytes += size;

			ex_ogg_sync_wrote(&oy, size);

			if (size == 0) return False;
		}
	}

	ex_ogg_stream_pagein(&os, &og);

	preSkipLeft += skipSamples;

	ex_opus_multistream_decoder_ctl(decoder, OPUS_RESET_STATE);

	return True;
}

Int BoCA::DecoderOpus::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	Int	 size	       = 0;
	Int	 dataBufferLen = 0;

	while (ex_ogg_sync_pageout(&oy, &og) == 1)
	{
		ex_ogg_stream_pagein(&os, &og);

		while (ex_ogg_stream_packetout(&os, &op) == 1)
		{
			if (dataBufferLen < size + maxFrameSize * format.channels * (format.bits / 8))
			{
				dataBufferLen += 2 * maxFrameSize * format.channels * (format.bits / 8);

				data.Resize(dataBufferLen);
			}

			Int	 frameSize = ex_opus_multistream_decode(decoder, op.packet, op.bytes, (signed short *) (unsigned char *) (data + size), maxFrameSize, 0);

			if (frameSize > preSkipLeft)
			{
				if (preSkipLeft) memmove((unsigned char *) data + size, (unsigned char *) data + size + preSkipLeft * format.channels * (format.bits / 8), (frameSize - preSkipLeft) * format.channels * (format.bits / 8));

				size += Math::Max(0, (frameSize - preSkipLeft) * format.channels * (format.bits / 8));
			}

			preSkipLeft = Math::Max(0, preSkipLeft - frameSize);
		}

		if (ex_ogg_page_eos(&og)) break;
	}

	data.Resize(size);

	/* Bail out if no more data available.
	 */
	char	*buffer = ex_ogg_sync_buffer(&oy, 8192);
	Int	 bytes	= driver->ReadData((unsigned char *) buffer, 8192);

	if (size == 0 && bytes <= 0) return -1;

	inBytes += bytes;

	ex_ogg_sync_wrote(&oy, bytes);

	/* Change to default channel order.
	 */
	if	(format.channels == 3) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_3_0, Channel::Default_3_0);
	else if (format.channels == 5) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_5_0, Channel::Default_5_0);
	else if (format.channels == 6) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_5_1, Channel::Default_5_1);
	else if (format.channels == 7) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_6_1, Channel::Default_6_1);
	else if (format.channels == 8) Utilities::ChangeChannelOrder(data, format, Channel::Vorbis_7_1, Channel::Default_7_1);

	return size;
}
