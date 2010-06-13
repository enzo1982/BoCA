 /* BoCA - BonkEnc Component Architecture
  * Copyright (C) 2007-2010 Robert Kausch <robert.kausch@bonkenc.org>
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the "GNU General Public License".
  *
  * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR
  * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
  * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. */

#include <smooth.h>
#include <smooth/dll.h>

#include "vorbis_in.h"
#include "dllinterface.h"

using namespace smooth::IO;

const String &BoCA::VorbisIn::GetComponentSpecs()
{
	static String	 componentSpecs;

	if (oggdll != NIL && vorbisdll != NIL)
	{
		componentSpecs = "				\
								\
		  <?xml version=\"1.0\" encoding=\"UTF-8\"?>	\
		  <component>					\
		    <name>Ogg Vorbis Decoder</name>		\
		    <version>1.0</version>			\
		    <id>vorbis-in</id>				\
		    <type>decoder</type>			\
		    <format>					\
		      <name>Ogg Vorbis Audio</name>		\
		      <extension>ogg</extension>		\
		      <tag mode=\"other\">VorbisComment</tag>	\
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

Bool BoCA::VorbisIn::CanOpenStream(const String &streamURI)
{
	return streamURI.ToLower().EndsWith(".ogg");
}

Error BoCA::VorbisIn::GetStreamInfo(const String &streamURI, Track &track)
{
	InStream	 in(STREAM_FILE, streamURI, IS_READONLY);

	Format	&format = track.GetFormat();

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

				ex_vorbis_info_init(&vi);
				ex_vorbis_comment_init(&vc);

				initialized = True;
			}

			ex_ogg_stream_pagein(&os, &og);

			while (ex_ogg_stream_packetout(&os, &op) == 1 && !done)
			{
				ex_vorbis_synthesis_headerin(&vi, &vc, &op);

				if (packetNum == 0)
				{
					format.rate = vi.rate;
					format.channels = vi.channels;
					track.length = -1;

					Int	 bitrate = 0;

					if	(vi.bitrate_nominal > 0)		       bitrate = vi.bitrate_nominal;
			 		else if (vi.bitrate_lower > 0 && vi.bitrate_upper > 0) bitrate = (vi.bitrate_lower + vi.bitrate_upper) / 2;

					if (bitrate > 0) track.approxLength = track.fileSize / (bitrate / 8) * format.rate * format.channels;
				}

				if (packetNum == 1)
				{
					Buffer<UnsignedByte>	 buffer(op.bytes - 7);

					memcpy(buffer, op.packet + 7, op.bytes - 7);

					AS::Registry		&boca = AS::Registry::Get();
					AS::TaggerComponent	*tagger = (AS::TaggerComponent *) boca.CreateComponentByID("vorbis-tag");

					if (tagger != NIL)
					{
						tagger->ParseBuffer(buffer, track);

						boca.DeleteComponent(tagger);
					}
				}

				if (packetNum >= 2) done = True;

				packetNum++;
			}
		}
	}

	if (initialized)
	{
		ex_vorbis_comment_clear(&vc);
		ex_vorbis_info_clear(&vi);

		ex_ogg_stream_clear(&os);
	}

	ex_ogg_sync_clear(&oy);

	return Success();
}

BoCA::VorbisIn::VorbisIn()
{
	buffer = NIL;

	packageSize = 0;
}

BoCA::VorbisIn::~VorbisIn()
{
}

Bool BoCA::VorbisIn::Activate()
{
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

				ex_vorbis_info_init(&vi);
				ex_vorbis_comment_init(&vc);

				initialized = True;
			}

			ex_ogg_stream_pagein(&os, &og);

			while (ex_ogg_stream_packetout(&os, &op) == 1 && !done)
			{
				ex_vorbis_synthesis_headerin(&vi, &vc, &op);

				if (packetNum >= 2) done = True;

				packetNum++;
			}
		}
	}

	ex_vorbis_synthesis_init(&vd, &vi);
	ex_vorbis_block_init(&vd, &vb);

	return True;
}

Bool BoCA::VorbisIn::Deactivate()
{
	ex_ogg_stream_clear(&os);

	ex_vorbis_block_clear(&vb);
	ex_vorbis_dsp_clear(&vd);
	ex_vorbis_comment_clear(&vc);
	ex_vorbis_info_clear(&vi);

	ex_ogg_sync_clear(&oy);

	return True;
}

Int BoCA::VorbisIn::ReadData(Buffer<UnsignedByte> &data, Int size)
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
		static short	 convbuffer[6144];
		static int	 convsize = 6144 / vi.channels;

		ex_ogg_stream_pagein(&os, &og);

		while (ex_ogg_stream_packetout(&os, &op) == 1)
		{
			float  **pcm;
			int	 samples;

			ex_vorbis_synthesis(&vb, &op);
			ex_vorbis_synthesis_blockin(&vd, &vb);

			while ((samples = ex_vorbis_synthesis_pcmout(&vd, &pcm)) > 0)
			{
				Int	 bout = (samples < convsize ? samples : convsize);

				for (Int i = 0; i < vi.channels; i++)
				{
					short	*ptr = convbuffer + i;
					float	*mono = pcm[i];

					for (Int j = 0; j < bout; j++)
					{
						Int	 val = Int(mono[j] * 32767.f);

						val = Math::Min(val, 32767);
						val = Math::Max(val, -32768);

						*ptr = val;
						ptr += vi.channels;
					}
				}

				if (dataBufferLen < size + (bout * vi.channels * 2))
				{
					dataBufferLen += ((bout * vi.channels * 2) + 131072);

					data.Resize(dataBufferLen);
				}

				memcpy(((unsigned char *) data) + size, convbuffer, bout * vi.channels * 2);

				size += (bout * vi.channels * 2);

				ex_vorbis_synthesis_read(&vd, bout);
			}
		}

		if (ex_ogg_page_eos(&og)) break;
	}

	return size;
}
