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

#include <stdlib.h>

#include "speex.h"

using namespace smooth::IO;

const String &BoCA::DecoderSpeex::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && speexdll != NIL)
	{
		componentSpecs = "							\
											\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>				\
		  <component>								\
		    <name>Speex Speech Decoder %VERSION%</name>				\
		    <version>1.0</version>						\
		    <id>speex-dec</id>							\
		    <type>decoder</type>						\
		    <format>								\
		      <name>Speex Files</name>						\
		      <extension>spx</extension>					\
		      <tag id=\"vorbis-tag\" mode=\"other\">Vorbis Comment</tag>	\
		    </format>								\
		  </component>								\
											\
		";

		const char	*speexVersion = NIL;

		ex_speex_lib_ctl(SPEEX_LIB_GET_VERSION_STRING, &speexVersion);

		componentSpecs.Replace("%VERSION%", String("v").Append(speexVersion));
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

Bool BoCA::DecoderSpeex::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".spx");
}

Error BoCA::DecoderSpeex::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READ);
	Format		 format = track.GetFormat();

	format.bits  = 16;

	track.fileSize = in.Size();

	ogg_sync_state		 oy;
	ogg_stream_state	 os;
	ogg_page		 og;
	ogg_packet		 op;

	ex_ogg_sync_init(&oy);

	/* Get stream format.
	 */
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
				/* Found header packet.
				 */
				if (packetNum == 0)
				{
					SpeexHeader	*header = ex_speex_packet_to_header((char *) op.packet, op.bytes);

					format.rate	= header->rate;
					format.channels	= header->nb_channels;

					track.length = -1;

					free(header);
				}

				/* Found Vorbis Comment packet.
				 */
				if (packetNum == 1)
				{
					comments.Resize(op.bytes);

					memcpy(comments, op.packet, op.bytes);
				}

				/* Done if we reached packet one.
				 */
				if (packetNum >= 1) done = True;

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

	in.Seek(in.Size() - 65536);

	char	*buffer = ex_ogg_sync_buffer(&oy, 65536);
	Int	 size	= in.InputData(buffer, 65536);

	ex_ogg_sync_wrote(&oy, size);

	while (true)
	{
		Int	 seek = ex_ogg_sync_pageseek(&oy, &og);

		if (seek == 0) break;
		if (seek <  0 || ex_ogg_page_serialno(&og) != os.serialno) continue;

		track.length = ex_ogg_page_granulepos(&og);

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

BoCA::DecoderSpeex::DecoderSpeex()
{
	packageSize = 0;

	decoder	    = NIL;

	frameSize   = 0;
	lookAhead   = 0;
	nFrames	    = 0;

	preSkip	    = 0;
	preSkipLeft = 0;

	skipSamples = 0;

	pageNumber  = 0;

	memset(&oy, 0, sizeof(oy));
	memset(&os, 0, sizeof(os));
	memset(&og, 0, sizeof(og));
	memset(&op, 0, sizeof(op));

	memset(&bits, 0, sizeof(bits));
	memset(&stereo, 0, sizeof(stereo));
}

BoCA::DecoderSpeex::~DecoderSpeex()
{
}

Bool BoCA::DecoderSpeex::Activate()
{
	SpeexHeader	*header = NIL;

	ex_ogg_sync_init(&oy);

	Bool	 initialized = False;
	Bool	 done = False;
	Int	 packetNum = 0;

	while (!done)
	{
		Int	 size	= 4096;
		char	*buffer	= ex_ogg_sync_buffer(&oy, size);

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
					header = ex_speex_packet_to_header((char *) op.packet, op.bytes);
				}

				if (packetNum >= 1 + header->extra_headers) done = True;

				packetNum++;
			}
		}
	}

	const SpeexMode	*mode = ex_speex_lib_get_mode(header->mode);

	decoder = ex_speex_decoder_init(mode);

	ex_speex_decoder_ctl(decoder, SPEEX_SET_SAMPLING_RATE, &header->rate);
	ex_speex_decoder_ctl(decoder, SPEEX_GET_FRAME_SIZE, &frameSize);
	ex_speex_decoder_ctl(decoder, SPEEX_GET_LOOKAHEAD, &lookAhead);

	nFrames	   = Math::Max(1, (Int) header->frames_per_packet);
	pageNumber = 0;

	stereo.balance	    = 1;
	stereo.e_ratio	    = 0.5;
	stereo.smooth_left  = 1;
	stereo.smooth_right = 1;
	stereo.reserved1    = 0;
	stereo.reserved2    = 0;

	ex_speex_bits_init(&bits);

	free(header);

	return True;
}

Bool BoCA::DecoderSpeex::Deactivate()
{
	ex_ogg_stream_clear(&os);

	ex_speex_decoder_destroy(decoder);
	ex_speex_bits_destroy(&bits);

	ex_ogg_sync_clear(&oy);

	return True;
}

Bool BoCA::DecoderSpeex::Seek(Int64 samplePosition)
{
	while (ex_ogg_page_granulepos(&og) <= samplePosition || ex_ogg_page_serialno(&og) != os.serialno)
	{
		skipSamples = samplePosition - ex_ogg_page_granulepos(&og);

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

	ex_speex_decoder_ctl(decoder, SPEEX_RESET_STATE, 0);

	return True;
}

Int BoCA::DecoderSpeex::ReadData(Buffer<UnsignedByte> &data)
{
	const Format	&format = track.GetFormat();

	char	*buffer = ex_ogg_sync_buffer(&oy, data.Size());
	Int	 size	= driver->ReadData((unsigned char *) buffer, data.Size());

	if (size <= 0) return -1;

	inBytes += size;

	ex_ogg_sync_wrote(&oy, size);

	size = 0;

	Int	 dataBufferLen = 0;

	while (ex_ogg_sync_pageout(&oy, &og) == 1)
	{
		ex_ogg_stream_pagein(&os, &og);

		if (pageNumber++ == 0)
		{
			if (ex_ogg_page_granulepos(&og) < ex_ogg_page_packets(&og) * nFrames * frameSize) preSkip += ex_ogg_page_packets(&og) * nFrames * frameSize - ex_ogg_page_granulepos(&og);

			preSkip	    += lookAhead;
			preSkipLeft += preSkip;
		}

		while (ex_ogg_stream_packetout(&os, &op) == 1)
		{
			pcmBuffer.Resize(frameSize * format.channels);

			ex_speex_bits_read_from(&bits, (char *) op.packet, op.bytes);

			for (Int i = 0; i < nFrames; i++)
			{
				if (ex_speex_decode_int(decoder, &bits, pcmBuffer) < 0) break;

				if (format.channels == 2) ex_speex_decode_stereo_int(pcmBuffer, frameSize, &stereo);

				if (frameSize > preSkipLeft)
				{
					if (preSkipLeft) memmove((short *) pcmBuffer, (short *) pcmBuffer + preSkipLeft * format.channels, (frameSize - preSkipLeft) * format.channels * 2);

					if (dataBufferLen < size + ((frameSize - preSkipLeft) * format.channels * 2))
					{
						dataBufferLen += (((frameSize - preSkipLeft) * format.channels * 2) + 131072);

						data.Resize(dataBufferLen);
					}

					for (Int j = 0; j < (frameSize - preSkipLeft) * format.channels; j++)
					{
						((short *) (UnsignedByte *) data)[size / 2 + j] = pcmBuffer[j];
					}

					size += ((frameSize - preSkipLeft) * format.channels * 2);
				}

				preSkipLeft = Math::Max(0, preSkipLeft - frameSize);
			}
		}

		if (ex_ogg_page_eos(&og)) break;
	}

	return size;
}
