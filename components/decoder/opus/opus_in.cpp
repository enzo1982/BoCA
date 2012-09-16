 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2012 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include <stdint.h>

#include "opus_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::OpusIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && opusdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Opus Audio Decoder</name>					\
		    <version>1.0</version>						\
		    <id>opus-in</id>							\
		    <type>decoder</type>						\
		    <format>								\
		      <name>Opus Audio</name>						\
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
	const Int	 maxFrameSize  = 2880;
	const Int	 maxPacketSize = 1275;

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

Bool BoCA::OpusIn::CanOpenStream(const String &streamURI)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);

	if (in.InputString(4) != "OggS") return False;

	in.Seek(0);

	ogg_sync_state		 oy;
	ogg_stream_state	 os;
	ogg_page		 og;
	ogg_packet		 op;

	ex_ogg_sync_init(&oy);

	Bool	 result = False;

	Bool	 initialized = False;
	Bool	 done = False;

	while (!done)
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
				if (op.packet[0] == 'O' && op.packet[1] == 'p' && op.packet[2] == 'u' && op.packet[3] == 's' && op.packet[4] == 'H' && op.packet[5] == 'e' && op.packet[6] == 'a' && op.packet[7] == 'd') result = True;

				done = True;
			}
		}
	}

	if (initialized) ex_ogg_stream_clear(&os);

	ex_ogg_sync_clear(&oy);

	return result;
}

Error BoCA::OpusIn::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);
	Format		 format;

	format.order = BYTE_INTEL;
	format.bits  = 16;
	format.rate  = 48000;

	track.fileSize = in.Size();

	ogg_sync_state		 oy;
	ogg_stream_state	 os;
	ogg_page		 og;
	ogg_packet		 op;

	ex_ogg_sync_init(&oy);

	Bool	 initialized = False;
	Bool	 done = False;
	Int	 packetNum = 0;

	while (!done)
	{
		Int	 size	= Math::Min((Int64) 4096, track.fileSize - in.GetPos());
		char	*buffer	= ex_ogg_sync_buffer(&oy, size);

		in.InputData(buffer, size);

		ex_ogg_sync_wrote(&oy, size);

		while (ex_ogg_sync_pageout(&oy, &og) == 1 && !done)
		{
			if (!initialized)
			{
				ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

				initialized = True;
			}

			ex_ogg_stream_pagein(&os, &og);

			while (ex_ogg_stream_packetout(&os, &op) == 1 && !done)
			{
				if (packetNum == 0)
				{
					OpusHeader	*setup = (OpusHeader *) op.packet;

					format.channels = setup->nb_channels;

					track.length = -1;
				}

				if (packetNum == 1)
				{
					if (op.packet[0] == 'O' && op.packet[1] == 'p' && op.packet[2] == 'u' && op.packet[3] == 's' && op.packet[4] == 'T' && op.packet[5] == 'a' && op.packet[6] == 'g' && op.packet[7] == 's')
					{
						Buffer<UnsignedByte>	 buffer(op.bytes - 8);

						memcpy(buffer, op.packet + 8, op.bytes - 8);

						AS::Registry		&boca = AS::Registry::Get();
						AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

						if (tagger != NIL)
						{
							tagger->ParseBuffer(buffer, track);

							boca.DeleteComponent(tagger);
						}
					}
				}

				if (packetNum == 3)
				{
					int			 error	 = 0;
					OpusDecoder		*decoder = ex_opus_decoder_create(48000, format.channels, &error);

					if (error == 0)
					{
						Buffer<signed short>	 samples(maxFrameSize * format.channels);

						track.approxLength = track.fileSize / (op.bytes * 1.05) * ex_opus_decode(decoder, op.packet, op.bytes, samples, maxFrameSize, 0);

						ex_opus_decoder_destroy(decoder);
					}
				}

				if (packetNum >= 3) done = True;

				packetNum++;
			}
		}
	}

	track.SetFormat(format);

	if (initialized) ex_ogg_stream_clear(&os);

	ex_ogg_sync_clear(&oy);

	return Success();
}

BoCA::OpusIn::OpusIn()
{
	decoder	= NIL;

	packageSize = 0;
}

BoCA::OpusIn::~OpusIn()
{
}

Bool BoCA::OpusIn::Activate()
{
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

		while (ex_ogg_sync_pageout(&oy, &og) == 1 && !done)
		{
			if (!initialized)
			{
				ex_ogg_stream_init(&os, ex_ogg_page_serialno(&og));

				initialized = True;
			}

			ex_ogg_stream_pagein(&os, &og);

			while (ex_ogg_stream_packetout(&os, &op) == 1 && !done)
			{
				if (packetNum == 0)
				{
					OpusHeader	*setup = (OpusHeader *) op.packet;
					int		 error = 0;

					decoder = ex_opus_decoder_create(48000, setup->nb_channels, &error);
				}

				if (packetNum >= 1) done = True;

				packetNum++;
			}
		}
	}

	return True;
}

Bool BoCA::OpusIn::Deactivate()
{
	ex_ogg_stream_clear(&os);

	ex_opus_decoder_destroy(decoder);

	ex_ogg_sync_clear(&oy);

	return True;
}

Int BoCA::OpusIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (size <= 0) return -1;

	const Format	&format = track.GetFormat();

	char	*buffer = ex_ogg_sync_buffer(&oy, size);

	size = driver->ReadData((unsigned char *) buffer, size);

	inBytes += size;

	ex_ogg_sync_wrote(&oy, size);

	size = 0;

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

			Int	 frameSize = ex_opus_decode(decoder, op.packet, op.bytes, (signed short *) (unsigned char *) (data + size), maxFrameSize, 0);

			size += Math::Max(0, frameSize * format.channels * (format.bits / 8));
		}

		if (ex_ogg_page_eos(&og)) break;
	}

	return size;
}
