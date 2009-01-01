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

#include "speex_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::SpeexIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && speexdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Speex Speech Decoder</name>		\
		    <version>1.0</version>			\
		    <id>speex-in</id>				\
		    <type>decoder</type>			\
		    <format>					\
		      <name>Speex Files</name>			\
		      <extension>spx</extension>		\
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
	LoadSpeexDLL();
}

Void smooth::DetachDLL()
{
	FreeOggDLL();
	FreeSpeexDLL();
}

Bool BoCA::SpeexIn::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".spx");
}

Error BoCA::SpeexIn::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READONLY);
	Format		&format = track.GetFormat();

	format.order = BYTE_INTEL;
	format.bits = 16;
	track.fileSize = in.Size();

	ex_ogg_sync_init(&oy);

	Bool	 initialized = False;
	Bool	 done = False;
	Int	 packetNum = 0;

	while (!done)
	{
		Int	 size = Math::Min(4096, track.fileSize - in.GetPos());

		buffer = ex_ogg_sync_buffer(&oy, size);

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
					SpeexHeader	*header = ex_speex_packet_to_header((char *) op.packet, op.bytes);

					format.rate = header->rate;
					format.channels = header->nb_channels;

					track.length = -1;

					free(header);
				}

				if (packetNum == 1)
				{
					Buffer<UnsignedByte>	 buffer(op.bytes);

					memcpy(buffer, op.packet, op.bytes);

					track.ParseVorbisComment(buffer);
				}

				if (packetNum >= 1) done = True;

				packetNum++;
			}
		}
	}

	if (initialized) ex_ogg_stream_clear(&os);

	ex_ogg_sync_clear(&oy);

	return Success();
}

BoCA::SpeexIn::SpeexIn()
{
	packageSize = 0;
}

BoCA::SpeexIn::~SpeexIn()
{
}

Bool BoCA::SpeexIn::Activate()
{
	SpeexHeader	*header = NIL;

	ex_ogg_sync_init(&oy);

	Bool	 initialized = False;
	Bool	 done = False;
	Int	 packetNum = 0;

	while (!done)
	{
		Int	 size = 4096;

		buffer = ex_ogg_sync_buffer(&oy, size);

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

	nFrames = Math::Max(1, (Int) header->frames_per_packet);

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

Bool BoCA::SpeexIn::Deactivate()
{
	ex_ogg_stream_clear(&os);

	ex_speex_decoder_destroy(decoder);
	ex_speex_bits_destroy(&bits);

	ex_ogg_sync_clear(&oy);

	return True;
}

Int BoCA::SpeexIn::ReadData(Buffer<UnsignedByte> &data, Int size)
{
	if (size <= 0) return -1;

	buffer = ex_ogg_sync_buffer(&oy, size);

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
			static Buffer<short>	 pcm;

			int	 samples = frameSize * track.GetFormat().channels;

			pcm.Resize(samples);

			ex_speex_bits_read_from(&bits, (char *) op.packet, op.bytes);

			for (Int i = 0; i < nFrames; i++)
			{
				if (ex_speex_decode_int(decoder, &bits, pcm) < 0) break;

				if (track.GetFormat().channels == 2) ex_speex_decode_stereo_int(pcm, frameSize, &stereo);

				if (dataBufferLen < size + (samples * 2))
				{
					dataBufferLen += ((samples * 2) + 131072);

					data.Resize(dataBufferLen);
				}

				for (Int j = 0; j < samples; j++)
				{
					((short *) (UnsignedByte *) data)[size / 2 + j] = pcm[j];
				}

				size += (samples * 2);
			}
		}

		if (ex_ogg_page_eos(&og)) break;
	}

	return size;
}
